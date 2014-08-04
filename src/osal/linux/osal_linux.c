/* ----
 * ---- file   : osal_mmap.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : Linux OS abstraction layer. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 18Sep2013, 21Sep2013, 13Dec2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>

#define __USE_GNU defined
#include <pthread.h>

#include "../../../include/types.h"
#include "../../../include/log.h"
#include "../../../include/osal_err.h"
#include "../../../include/osal.h"


#ifndef HAVE_OS_LINUX
#error osal_linux.c must be compiled for Linux (no cygwin support here)
#endif


/*--------------------------------------------------------------------------- module vars */
static int fd_mem;

static struct timeval tv_start;

static sBool b_initialized = S_FALSE;


/*--------------------------------------------------------------------------- loc_milliseconds_init() */
static void loc_milliseconds_init(void) {
   gettimeofday(&tv_start, 0);

   srand(tv_start.tv_usec);
}


/*--------------------------------------------------------------------------- osal_init() */
osal_error_t S_CALL osal_init(sBool _bRootRequired) {
   osal_error_t ret;

   if(!b_initialized)
   {
      /* Initialize milliseconds/perfcounter */
      loc_milliseconds_init();
      
      if(_bRootRequired)
      {
         fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
         
         if(-1 != fd_mem)
         {
            /* Succeeded */
            ret = OSAL_ERR_OK;
         }
         else
         {
            // Failed: open()
            log_printf(LOG_ERROR "osal_init: open(\"/dev/mem\") failed (errno=%d). Are you root?\n", errno);
            
            ret = OSAL_ERR_MEM_OPEN;
         }
      }
      else
      {
         /* Succeeded */
         fd_mem = -1;

         ret = OSAL_ERR_OK;
      }

      b_initialized = S_TRUE;
   }
   else
   {
      /* already initialized */

      ret = OSAL_ERR_OK;
   }

   return ret;
}


/*--------------------------------------------------------------------------- osal_exit() */
void S_CALL osal_exit(void) {
   if(b_initialized)
   {
      if(-1 != fd_mem)
      {
         close(fd_mem);
         
         fd_mem = -1;
      }

      b_initialized = S_FALSE;
   }
}


/*--------------------------------------------------------------------------- osal_alloc() */
void * S_CALL osal_alloc(sSize _numBytes) {
   void *ret;

   ret = malloc(_numBytes);

   if(NULL == ret)
   {
      log_printf(LOG_ERROR "osal_alloc: failed to allocate %u bytes.\n", _numBytes);
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- osal_free() */
void S_CALL osal_free(void *_ptr) {
   if(NULL != _ptr)
   {
      free(_ptr);
   }
}


/*--------------------------------------------------------------------------- osal_mmap() */
void * S_CALL osal_mmap(void *_physAddr, sSize _numBytes) {
   void *ret;

   ret = mmap(NULL /*addr*/,
              _numBytes,
              PROT_READ | PROT_WRITE,
              MAP_SHARED,
              fd_mem,
              (off_t) _physAddr
              );

   if(MAP_FAILED == ret)
   {
      log_printf(LOG_ERROR "osal_mmap: mmap() failed (errno=%d)\n", errno);

      ret = NULL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- osal_munmap() */
osal_error_t S_CALL osal_munmap(void *_virtAddr, sSize _numBytes) {
   osal_error_t ret;

   int r = munmap(_virtAddr, _numBytes);
      
   if(0 == r)
   {
      // Succeeded
      ret = OSAL_ERR_OK;
   }
   else
   {
      // Failed: munmap()
      ret = OSAL_ERR_MEM_UNMAP;
   }

   return ret;
}


/*--------------------------------------------------------------------------- osal_usleep() */
void osal_usleep(sUI _numMicrosecs) {

   usleep(_numMicrosecs);
}


/*--------------------------------------------------------------------------- osal_msleep() */
void osal_msleep(sUI _numMillisecs) {
   fd_set fdSet;
   struct timeval timeout;
   int ret;

   FD_ZERO(&fdSet);
   timeout.tv_sec  = 0;
   timeout.tv_usec = 1000 * _numMillisecs;

   ret = select(0, &fdSet, NULL, NULL, &timeout);

   (void)ret;
}


/*--------------------------------------------------------------------------- osal_yield() */
S_EXTERN void osal_yield(void) {
#if 1
   pthread_yield();
#else
   osal_usleep(10u);
#endif
}


/*--------------------------------------------------------------------------- osal_milliseconds_get() */
sUI osal_milliseconds_get(void) {
   struct timeval c;

   /* (todo) move to OSAL */
   gettimeofday(&c, 0);

   return (sUI) ( (c.tv_sec - tv_start.tv_sec) * 1000 + (c.tv_usec - tv_start.tv_usec)/1000 );
}
