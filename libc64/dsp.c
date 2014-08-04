/* ----
 * ---- file   : dsp.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Arch-independent DSP utility functions. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 18Sep2013, 19Sep2013, 20Sep2013, 21Sep2013, 22Sep2013
 * ----          23Sep2013, 02Oct2013, 03Oct2013, 04Oct2013, 05Oct2013, 15Oct2013, 16Oct2013
 * ----          18Oct2013, 19Oct2013, 20Oct2013, 21Oct2013, 22Oct2013, 23Oct2013, 27Oct2013
 * ----          02Nov2013, 08Dec2013, 13Dec2013
 * ----
 * ----
 */

#define MAX_RPC_READ_RETRIES        (100000u)
#define MASK_RPC_READ_YIELD_RETRIES  (    3u)    /* yield() every "n" retries. must be >0 and POT -1. */

#define MAX_RPC_WRITE_RETRIES       (100000u)
#define MASK_RPC_WRITE_YIELD_RETRIES (    3u)    /* yield() every "n" retries. must be >0 and POT -1. */


#define RPC_RECV_TIMEOUT_SEC   (5u)
#define RPC_RECV_TIMEOUT_USEC  (0u)


#define MAX_RPC_FCBUSY_RETRIES  (1000u)
#define RPC_FCBUSY_MSLEEP       (  10u)


#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <sys/select.h>

#define MORECORE my_morecore
#define MSPACES 1
#define ONLY_MSPACES 1
#define LACKS_FCNTL_H 1
#define HAVE_MORECORE 1
#define SKIP_SYS_ALLOC 1
#include "dl_malloc.h"

#include "../include/types.h"
#include "../include/log.h"

#include "../include/dsp_err.h"
#include "../include/dsp_common.h"
#include "../include/dsp_config.h"
#include "../include/msg.h"
#include "../dsp/libc64_dsp/include/com.h"
#include "../include/dsp.h"
#include "../include/dsp_priv.h"
#include "../include/c64_dev.h"
#include "../include/osal_err.h"
#include "../include/osal.h"
#include "../include/coff_err.h"
#include "../include/coff.h"

#include "../include/arch/dm3730/dm3730.h"

#include "../dsp/libc64_dsp/include/com.h"
#include "../dsp/core/components.h"

#ifndef MAP_HUGETLB
//#define MAP_HUGETLB  (DSP_HUGETLB_PAGESIZE)
#define MAP_HUGETLB  0x40000
#endif


#ifndef HAVE_OS_LINUX
#error dsp.c must be compiled for Linux (no cygwin support here)
#endif


#define USE_FAST_MMAP defined


#define FC_REG(a) (*(volatile sU32*)(l1sram_fshm.virt_addr + (a)))


typedef struct {
   sUI                sec_idx;
   dsp_mem_region_t   mem;
   dsp_component_id_t compid;
} overlay_t;


typedef struct {
   c64_dev_ioctl_shm_alloc_t shm_alloc_req;
} shared_mem_t;


typedef struct {
   mspace *msp;
   sU32    base_addr;
   sU32    size;
   sSI     total_num_bytes;
   sSI     total_allocs;
   sSI     total_frees;
} dsp_mspace_t;


/*--------------------------------------------------------------------------- module vars */
static sBool b_open = S_FALSE;

sBool b_skip_dsp_open = S_FALSE;


#ifdef USE_FAST_MMAP
static sBool b_dsp_mmap_force_real;


static dsp_mem_region_t fast_mmap[] = {

   /* DSP memory (updated in dsp_init_root() with actual dsp_config)
    *   must be the first entry in this array!
    */
   { 0,0,0 },

   /* SCM / PRM DSP regs */
   {
      .phys_addr = 0x48000000u,
      .virt_addr = 0u,
      .size      = 0x00400000u
   }
};

#define NUM_FAST_MMAP_ENTRIES  (sizeof(fast_mmap) / sizeof(dsp_mem_region_t))

#endif /* USE_FAST_MMAP */


int c64_fd = -1; /* make this is a global var, just in case */

static sUI rpc_count = 0;

static shared_mem_t shared_mem[NUM_DSP_CACHE_TYPES];

static dsp_mem_region_t l1sram_fshm;

static dsp_mem_region_t l2sram_fshm;

static sU32 hugetlb_page_phys_addr[DSP_HUGETLB_MAXPAGES];

static sBool b_poll;

static sBool b_autoresume = S_TRUE;

static overlay_t overlays[MLB_COM_NUM_SECTION_INDICES];

#include "../dsp/linker_scripts/overlay_sections.c"

static dsp_mem_region_t logbuf;
static sU32             logbuf_offset_read;


/*--------------------------------------------------------------------------- loc_get_cache_type_by_mem() */
dsp_cache_t loc_get_cache_type_by_mem(dsp_mem_region_t _mem) {
   dsp_cache_t type;

   for(type=0; type < NUM_DSP_CACHE_TYPES; type++) /* should be "shm index". maybe later. */
   {
      if(_mem.phys_addr == shared_mem[type].shm_alloc_req.mem.phys_addr)
      {
         type = shared_mem[type].shm_alloc_req.type;
         break;
      }
   }

   if(NUM_DSP_CACHE_TYPES == type)
   {
      /* not found */
      type = -1;
   }

   return type;
}


/*--------------------------------------------------------------------------- loc_overlays_free() */
static void loc_overlays_free(void) {
   sUI i;

   if(LOG_DEBUG_LEVEL(5))
   {
      log_printf(LOG_TRACE "loc_overlays_free: ENTER\n");
   }

   for(i=0; i<MLB_COM_NUM_SECTION_INDICES; i++)
   {
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_TRACE "loc_overlays_free: section[%u] compid=%04x secIdx=%1u phys=0x%08x virt=0x%08x size=0x%08x\n",
                    i,
                    overlays[i].compid,
                    overlays[i].sec_idx,
                    overlays[i].mem.phys_addr,
                    overlays[i].mem.virt_addr,
                    overlays[i].mem.size
                    );
      }

      /* Unmap overlay code memory */
      if(0/*NULL*/ != overlays[i].mem.virt_addr)
      {
         munmap((void*)overlays[i].mem.virt_addr,
                overlays[i].mem.size
                );

         overlays[i].mem.virt_addr = 0;
      }

      /* Unregister component */
      if(0 != overlays[i].compid)
      {
         dsp_msg_t req;

         if(LOG_DEBUG_LEVEL(5))
         {
            log_printf(LOG_TRACE "loc_overlays_free: unregister component %u, section %u\n",
                       overlays[i].compid,
                       overlays[i].sec_idx
                       );
         }

         DSP_MSG_INIT_CORE(&req, CORE, CMD_COM_OVERLAY_INITIATE, 0, 0);

         if(0 == dsp_rpc_send(&req))
         {
            sU32 coreErr = ~0;

            dsp_fastcall_rpc(CORE_FC_CMD_COM_OVERLAY_UNREGISTER,
                             overlays[i].sec_idx, 0,
                             &coreErr, NULL
                             );

            dsp_fastcall_end();

            if(CORE_ERR_OK == coreErr)
            {
               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_DEBUG "loc_overlays_free: ok, unregistered component %u, section=%u.\n",
                             overlays[i].compid,
                             overlays[i].sec_idx
                             );
               }

               /* ok */
               overlays[i].compid = 0;
            }
            else
            {
               log_printf(LOG_ERROR "loc_overlays_free: failed to unregister component %u, section=%u.\n",
                          overlays[i].compid,
                          overlays[i].sec_idx
                          );
            }
         }
         else
         {
            log_printf(LOG_ERROR "loc_overlays_free: failed to send OVERLAY_INITIATE RPC to DSP. errno=%d (\"%s\")\n",
                       errno, strerror(errno)
                       );
         }
                       
      }

      /* Free overlay section */
      if(0 != overlays[i].mem.phys_addr)
      {
         if(0 == ioctl(c64_fd, C64_IOCTL_OVERLAY_FREE, overlays[i].sec_idx))
         {
            /* ok */
            overlays[i].sec_idx = 0;
         }
         else
         {
            log_printf(LOG_ERROR "loc_overlays_free: ioctl [OVERLAY_FREE] failed. sec_idx=%u. errno=%d (\"%s\")\n",
                       overlays[i].sec_idx,
                       errno, strerror(errno)
                       );
         }

         overlays[i].mem.phys_addr = 0;
      }

      /* Next overlay section / component */
   }

   if(LOG_DEBUG_LEVEL(5))
   {
      log_printf(LOG_TRACE "loc_overlays_free: LEAVE\n");
   }
}


/*--------------------------------------------------------------------------- loc_l1sram_fshm_map() */
static int loc_l1sram_fshm_map(void) {
   int ret;

   l1sram_fshm.phys_addr = DSP_L1SRAM_FSHM_BASE_GPP;
   l1sram_fshm.size      = DSP_L1SRAM_FSHM_SIZE;
   
   l1sram_fshm.virt_addr = (sU32) mmap(0,  /* addr */
                                       l1sram_fshm.size,
                                       (PROT_READ | PROT_WRITE),
                                       MAP_SHARED,
                                       c64_fd,
                                       l1sram_fshm.phys_addr
                                       );
         
   if(l1sram_fshm.virt_addr != (sU32)MAP_FAILED)
   {
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_open: OK, mmap()'d L1SRAM FSHM area. virt_addr=0x%08x\n",
                    l1sram_fshm.virt_addr
                    );
      }
      
      /* Succeeded */
      ret = 0;
   }
   else
   {
      /* Failed: mmap() */
      log_printf(LOG_ERROR "dsp_open: failed to mmap L1SRAM FSHM area.\n");
      
      ret = 10;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- loc_l1sram_fshm_unmap() */
static int loc_l1sram_fshm_unmap(void) {
   int ret;

   if(0/*NULL*/ != l1sram_fshm.virt_addr)
   {
      ret = munmap((void*)l1sram_fshm.virt_addr,
                   l1sram_fshm.size
                   );

      if(0 != ret)
      {
         log_printf(LOG_ERROR "loc_l1sram_fshm_unmap: munmap() failed. virt_addr=0x%08x size=0x%08x.\n",
                    l1sram_fshm.virt_addr,
                    l1sram_fshm.size
                    );
      }
      else
      {
         l1sram_fshm.virt_addr = 0/*NULL*/;
      }
   }
   else
   {
      ret = 0;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_l2sram_fshm_map() */
static int loc_l2sram_fshm_map(void) {
   int ret;

   l2sram_fshm.phys_addr = DSP_L2SRAM_FSHM_BASE_GPP;
   l2sram_fshm.size      = DSP_L2SRAM_FSHM_SIZE;
   
   l2sram_fshm.virt_addr = (sU32) mmap(0,  /* addr */
                                       l2sram_fshm.size,
                                       (PROT_READ | PROT_WRITE),
                                       MAP_SHARED,
                                       c64_fd,
                                       l2sram_fshm.phys_addr
                                       );
         
   if(l2sram_fshm.virt_addr != (sU32)MAP_FAILED)
   {
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_open: OK, mmap()'d L2SRAM FSHM area. virt_addr=0x%08x\n",
                    l2sram_fshm.virt_addr
                    );
      }
      
      /* Succeeded */
      ret = 0;
   }
   else
   {
      /* Failed: mmap() */
      log_printf(LOG_ERROR "dsp_open: failed to mmap L2SRAM FSHM area.\n");
      
      ret = 10;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- loc_l2sram_fshm_unmap() */
static int loc_l2sram_fshm_unmap(void) {
   int ret;

   if(0/*NULL*/ != l2sram_fshm.virt_addr)
   {
      ret = munmap((void*)l2sram_fshm.virt_addr,
                   l2sram_fshm.size
                   );

      if(0 != ret)
      {
         log_printf(LOG_ERROR "loc_l2sram_fshm_unmap: munmap() failed. virt_addr=0x%08x size=0x%08x.\n",
                    l2sram_fshm.virt_addr,
                    l2sram_fshm.size
                    );
      }
      else
      {
         l2sram_fshm.virt_addr = 0/*NULL*/;
      }
   }
   else
   {
      ret = 0;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_logbuf_map() */
static int loc_logbuf_map(void) {
   int ret;

   logbuf.phys_addr = DSP_LOGBUF_BASE_GPP;
   logbuf.size      = DSP_LOGBUF_SIZE;
   
   logbuf.virt_addr = (sU32) mmap(0,  /* addr */
                                  logbuf.size,
                                  (PROT_READ | PROT_WRITE),
                                  MAP_SHARED,
                                  c64_fd,
                                  logbuf.phys_addr
                                  );
         
   if(logbuf.virt_addr != (sU32)MAP_FAILED)
   {
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_open: OK, mmap()'d logbuf area. virt_addr=0x%08x\n",
                    logbuf.virt_addr
                    );
      }
      
      /* Succeeded */
      ret = 0;
   }
   else
   {
      /* Failed: mmap() */
      log_printf(LOG_ERROR "dsp_open: failed to mmap logbuf area.\n");
      
      ret = 10;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- loc_logbuf_startup_sync() */
static void loc_logbuf_startup_sync(void) {
   /* Find current start of buffer */
   sSI cntMax = (sSI) DSP_LOGBUF_SIZE;
   sBool bLZ = S_FALSE;
   const char *buf = (const char *)logbuf.virt_addr;

   logbuf_offset_read = 0;

/*    printf("xxx sync buf[0]=%u\n", buf[0]); */

   if(1 && (0 != buf[logbuf_offset_read]))
   {
      do
      {
         char c = buf[logbuf_offset_read];

         if((0 == c) && bLZ)
         {
            /* Found end of buffer */
            break;
         }

         logbuf_offset_read++;

         bLZ = (0 == c);

      } while(--cntMax > 0);
   }

/*    printf("xxx startup_sync: logbuf_offset_read=%u\n", */
/*           logbuf_offset_read */
/*           ); */

   if(DSP_LOGBUF_SIZE == logbuf_offset_read)
   {
      /* entire logbuf does not contain an end-of-buffer marker ? sth went wrong here. */
      logbuf_offset_read = 0;
   }
   
}


/*--------------------------------------------------------------------------- loc_logbuf_unmap() */
static int loc_logbuf_unmap(void) {
   int ret;

   if(0/*NULL*/ != logbuf.virt_addr)
   {
      ret = munmap((void*)logbuf.virt_addr,
                   logbuf.size
                   );

      if(0 != ret)
      {
         log_printf(LOG_ERROR "loc_logbuf_unmap: munmap() failed. virt_addr=0x%08x size=0x%08x.\n",
                    logbuf.virt_addr,
                    logbuf.size
                    );
      }
      else
      {
         logbuf.virt_addr = 0/*NULL*/;
      }
   }
   else
   {
      ret = 0;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_strerror() */
const char * S_CALL dsp_strerror(dsp_error_t _err) {
   switch(_err)
   {
      default:
         return NULL;

      case DSP_ERR_OK:           return "DSP_ERR_OK";

      case DSP_ERR_MEM:          return "DSP_ERR_MEM";
      case DSP_ERR_MEM_OPEN:     return "DSP_ERR_MEM_OPEN";
      case DSP_ERR_MEM_MAP:      return "DSP_ERR_MEM_MAP";
      case DSP_ERR_MEM_UNMAP:    return "DSP_ERR_MEM_UNMAP";
      case DSP_ERR_MEM_CHECK:    return "DSP_ERR_MEM_CHECK";
   }
}


/*--------------------------------------------------------------------------- dsp_autoresume_set() */
void dsp_autoresume_set(sBool _bAutoResume) {
   b_autoresume = _bAutoResume ? S_TRUE : S_FALSE;

   log_printf(LOG_INFO "dsp_autoresume_set: autoresume is now %s.\n",
              _bAutoResume ? "enabled" : "disabled"
              );
}


/*--------------------------------------------------------------------------- dsp_open() */
int dsp_open(void) {
   int ret;
   const char *envDebug;

   if(S_FALSE == b_open)
   {
      /* Initialize OS abstraction layer */
      ret = osal_init(S_FALSE);
      
      /* Determine log debug level */
      envDebug = getenv("C64_DEBUG");
      
      if(NULL != envDebug)
      {
         sscanf(envDebug, "%u", &log_debug_level);
         
         if(log_debug_level >= 30)
         {
            log_debug_level = 30;
         }
      }
      else
      {
         log_debug_level = 0;
      }
      
      /* Zerofill shared memory struct */
      memset(shared_mem, 0, sizeof(shared_mem_t) * NUM_DSP_CACHE_TYPES);
      
      /* Zerofill L1DSRAM/FSHM struct */
      memset(&l1sram_fshm, 0, sizeof(dsp_mem_region_t));

      /* Zerofill L2SRAM/FSHM struct */
      memset(&l2sram_fshm, 0, sizeof(dsp_mem_region_t));

      /* Zerofill hugetlb physical page addresses */
      memset(hugetlb_page_phys_addr, 0, sizeof(sU32) * DSP_HUGETLB_MAXPAGES);
      
      /* Zerofill overlays */
      memset(overlays, 0, sizeof(overlays));
      
      /* Disable polling mode */
      b_poll = S_FALSE;
      
      /* Open the c64 device */
      c64_fd = open("/dev/c64", O_RDWR | O_SYNC);
      
      if(-1 != c64_fd)
      {
         ret = loc_l1sram_fshm_map();
         
         if(0 == ret)
         {
            ret = loc_l2sram_fshm_map();
         
            if(0 == ret)
            {
               ret = loc_logbuf_map();
               
               if(0 == ret)
               {
                  /* Find start of log ringbuffer (set logbuf_offset_read) */
                  loc_logbuf_startup_sync();
                  
                  if(b_autoresume)
                  {
                     ret = dsp_resume();
                     
                     if(0 == ret)
                     {
                        if(LOG_DEBUG_LEVEL(1))
                        {
                           log_printf(LOG_INFO "dsp_open: DSP program execution resumed OK.\n");
                        }
                        
                        loc_logbuf_startup_sync();
                        
                        /* Succeeded */
                        b_open = S_TRUE;
                     }
                     else
                     {
                        /* Failed: dsp_resume() */
                        if(EBUSY == errno)
                        {
                           /* Succeeded (already running) */
                           ret = 0;
                        }
                        else
                        {
                           log_printf(LOG_ERROR "dsp_open: failed to resume DSP program execution.\n");

                           (void)loc_l2sram_fshm_unmap();
                           
                           loc_l1sram_fshm_unmap();
                           
                           close(c64_fd);
                           
                           c64_fd = -1;
                        }
                     }
                  }
                  else
                  {
                     /* Succeeded (no resume, no logbuf) */
                  }
               }
               else
               {
                  /* Failed: loc_logbuf_map() */
                  (void)loc_l2sram_fshm_unmap();

                  (void)loc_l1sram_fshm_unmap();
                  
                  close(c64_fd);
                  
                  c64_fd = -1;
               }
            }
            else
            {
               /* Failed: loc_l2sram_fshm_map() */
               (void)loc_l1sram_fshm_unmap();

               close(c64_fd);
               
               c64_fd = -1;
            }
         }
         else
         {
            /* Failed: loc_l1sram_fshm_map() */
            close(c64_fd);
            
            c64_fd = -1;
         }
      }
      else
      {
         /* Failed: open() */
         log_printf(LOG_ERROR "dsp_open: failed to open \"/dev/c64\". errno=%d (\"%s\").\n",
                    errno,
                    strerror(errno)
                    );
         ret = 10;
      }
   }
   else
   {
      log_printf(LOG_WARN "dsp_open: ignoring redundant call (already open).\n");

      /* Succeeded */
      ret = 0;
   }
      
   return ret;
}


/*--------------------------------------------------------------------------- dsp_close() */
void dsp_close(void) {

   if(LOG_DEBUG_LEVEL(5))
   {
      log_printf(LOG_TRACE "dsp_close: ENTER\n");
   }

   if(-1 != c64_fd)
   {
      dsp_cache_t type;

      /* Power on DSP in case it's off */
      (void)dsp_resume();

      /* Free shared memory in case client forgot to */
      for(type=0; type < NUM_DSP_CACHE_TYPES; type++) /* should be index. maybe later. */
      {
         dsp_shm_free(shared_mem[type].shm_alloc_req.mem);
      }

      /* Unregister components and unmap overlay sections */
      loc_overlays_free();

      /* Unmap logbuf */
      (void)loc_logbuf_unmap();

      /* Unmap L2SRAM FSHM */
      (void)loc_l2sram_fshm_unmap();

      /* Unmap L1SRAM FSHM */
      (void)loc_l1sram_fshm_unmap();

      /* Disconnect client */
      close(c64_fd);

      c64_fd = -1;
   }

   /* Shutdown OS abstraction layer */
   osal_exit();

   if(LOG_DEBUG_LEVEL(5))
   {
      log_printf(LOG_TRACE "dsp_close: LEAVE\n");
   }

   b_open = S_FALSE;
}


/*--------------------------------------------------------------------------- dsp_shm_alloc() */
dsp_mem_region_t dsp_shm_alloc(dsp_cache_t _type, sU32 _numBytes) {
   dsp_mem_region_t ret = {0, 0, 0};
   int r;

   if( (_type >= 0) && (_type < NUM_DSP_CACHE_TYPES) )
   {
      c64_dev_ioctl_shm_alloc_t *allocReq = &shared_mem[_type].shm_alloc_req;

      if(0 == allocReq->mem.phys_addr)
      {
         /* Round up req.size to page size 
            xxx hardcoded pagesize
         */
         if(DSP_CACHE_HUGETLB == _type)
         {
            /* round up to next 2mb page boundary */
            _numBytes = (((_numBytes + (DSP_HUGETLB_PAGESIZE - 1u)) >> DSP_HUGETLB_PAGESHIFT) << DSP_HUGETLB_PAGESHIFT);
         }
         else
         {
            /* round up to next 4kb page boundary */
            _numBytes = (((_numBytes + (DSP_PAGESIZE - 1u)) >> DSP_PAGESHIFT) << DSP_PAGESHIFT);
         }

         allocReq->mem.size = _numBytes;
         allocReq->type     = _type;

         if(DSP_CACHE_HUGETLB == _type)
         {
            if(LOG_DEBUG_LEVEL(10))
            {
               log_printf(LOG_DEBUG "dsp_shm_alloc: MAP_HUGETLB=0x%08x\n", MAP_HUGETLB);
            }

            /* Map/allocate hugetlb shared memory (not done via c64.ko ATM) */
            allocReq->mem.virt_addr = (sU32) mmap(0, /* addr */
                                                  allocReq->mem.size,
                                                  (PROT_READ | PROT_WRITE),
                                                  (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB),
                                                  0, /* fd */
                                                  0  /* offset */
                                                  );

            if((sU32)MAP_FAILED == allocReq->mem.virt_addr)
            {
               log_printf(LOG_ERROR "dsp_shm_alloc: failed to allocate HUGETLB memory (size=0x%08x =~ %.3f kBytes)\n",
                          allocReq->mem.virt_addr,
                          (allocReq->mem.virt_addr / 1024.0f)
                          );

               r = 10;
            }
            else
            {
               r = 0;

               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_DEBUG "dsp_shm_alloc: MAP_HUGETLB virt_addr=0x%08x size=0x%08x\n",
                             allocReq->mem.virt_addr,
                             allocReq->mem.size
                             );
               }

               memset((void*)allocReq->mem.virt_addr, 0, allocReq->mem.size);
            }
         }
         else
         {
            r = 0;
         }
         
         if(0 == r)
         { 
            r = ioctl(c64_fd, C64_IOCTL_SHM_ALLOC, allocReq);
         
            if(0 == r)
            {
               if(LOG_DEBUG_LEVEL(30))
               {
                  log_printf(LOG_INFO "dsp_shm_alloc: ioctl() succeeded.\n");
               }

               if(DSP_CACHE_HUGETLB != _type)
               {
                  /* Map shared memory (allocated in kernel space) to user space */
                  allocReq->mem.virt_addr = (sU32) mmap(0,  /* addr */
                                                        allocReq->mem.size,
                                                        (PROT_READ | PROT_WRITE),
                                                        MAP_SHARED,
                                                        c64_fd,
                                                        allocReq->mem.phys_addr
                                                        );
               }
            
               if(allocReq->mem.virt_addr != (sU32)MAP_FAILED)
               {
                  if(DSP_CACHE_HUGETLB == _type)
                  {
                     /* Determine physical addresses of HUGETLB pages */
                     sUI  numPages = ( (_numBytes + DSP_HUGETLB_PAGESIZE - 1u) >> DSP_HUGETLB_PAGESHIFT );
                     sU32 addr;
                     sU32 virtAddr;
                     sUI  pageNr;

                     for(pageNr=0; pageNr<numPages; pageNr++)
                     {
                        addr = virtAddr = allocReq->mem.virt_addr + (pageNr << DSP_HUGETLB_PAGESHIFT);

                        if(LOG_DEBUG_LEVEL(15))
                        {
                           log_printf(LOG_TRACE "dsp_shm_alloc: query hugetlb phys for virt=0x%08x\n", addr);
                        }

                        r = ioctl(c64_fd, C64_IOCTL_HUGETLB_V2P, &addr);

                        if(0 == r)
                        {
                           if(LOG_DEBUG_LEVEL(15))
                           {
                              log_printf(LOG_DEBUG "dsp_shm_alloc: hugetlb page %u/%u virt=0x%08x phys=0x%08x\n",
                                         (pageNr + 1), numPages,
                                         virtAddr,
                                         addr
                                         );
                           }

                           hugetlb_page_phys_addr[pageNr] = addr;
                        }
                        else
                        {
                           log_printf(LOG_ERROR "dsp_shm_alloc: ioctl[HUGETLB_V2P] failed. errno=%d (\"%s\").\n",
                                      errno, strerror(errno)
                                      );
                           break;
                        }
                     }

                  }

                  if(0 == r)
                  {
                     /* Succeded */
                     if(LOG_DEBUG_LEVEL(30))
                     {
                        log_printf(LOG_INFO "dsp_shm_alloc: mmap() succeeded. virt_addr=0x%08x.\n",
                                   allocReq->mem.virt_addr
                                   );
                     }
                     
                     if(LOG_DEBUG_LEVEL(5))
                     {
                        log_printf(LOG_INFO "dsp_shm_alloc: allocated mem type=%d phys_addr=0x%08x virt_addr=0x%08x size=0x%08x\n",
                                   _type,
                                   allocReq->mem.phys_addr,
                                   allocReq->mem.virt_addr,
                                   allocReq->mem.size
                                   );
                     }
                     
                     
                     ret = allocReq->mem;
                  }
                  else
                  {
                     /* Error while determining hugetlb physical page addresses */
                     (void) munmap((void*)allocReq->mem.virt_addr,
                                   allocReq->mem.size
                                   );
                  }
               }
               else
               {
                  /* Failed: mmap() */
                  log_printf(LOG_ERROR "dsp_shm_alloc: mmap() failed. errno=%d (\"%s\").\n",
                             errno, strerror(errno)
                             );
               }
            }
            else
            {
               /* Failed: ioctl() */
               log_printf(LOG_ERROR "dsp_shm_alloc: ioctl() failed. errno=%d (\"%s\").\n",
                          errno,
                          strerror(errno)
                          );
            }
         }
         else
         {
            /* Failed: mmap() (MAP_HUGETLB) */
            log_printf(LOG_ERROR "dsp_shm_alloc: mmap() failed (HUGETLB). errno=%d (\"%s\").\n",
                       errno, strerror(errno)
                       );
         }
      }
      else
      {
         /* Failed: shm already initialized */
         log_printf(LOG_ERROR "dsp_shm_alloc: shm type %d is already initialized.\n");
      }
   }
   else
   {
      /* Failed: invalid shm type */
      log_printf(LOG_ERROR "dsp_shm_alloc: invalid shm type %d.\n", _type);
   }
      
   return ret;
}


/*--------------------------------------------------------------------------- dsp_shm_free() */
int dsp_shm_free(dsp_mem_region_t _mem) {
   int ret;
   dsp_cache_t type;

   type = loc_get_cache_type_by_mem(_mem);

   if( (type >= 0) && (type < NUM_DSP_CACHE_TYPES) )
   {
      c64_dev_ioctl_shm_alloc_t *allocReq = &shared_mem[type].shm_alloc_req;

      if(0/*NULL*/ != allocReq->mem.virt_addr)
      {
         ret = munmap((void*)allocReq->mem.virt_addr,
                      allocReq->mem.size
                      );

         if(0 != ret)
         {
            log_printf(LOG_ERROR "dsp_shm_free: munmap() failed. virt_addr=0x%08x size=0x%08x.\n",
                       allocReq->mem.virt_addr,
                       allocReq->mem.size
                       );
         }
      }

      ret = ioctl(c64_fd, C64_IOCTL_SHM_FREE, type);  /* type should be index. maybe later. */

      if(0 == ret)
      {
         /* Succeded */
         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf(LOG_INFO "dsp_shm_free: ioctl() succeeded.\n");
         }
      }
      else
      {
         /* Failed: ioctl() */
         log_printf(LOG_ERROR "dsp_shm_free: ioctl() failed. errno=%d (\"%s\").\n",
                    errno,
                    strerror(errno)
                    );
      }

      if(LOG_DEBUG_LEVEL(5))
      {
         log_printf(LOG_INFO "dsp_shm_free: freed mem type=%d phys_addr=0x%08x virt_addr=0x%08x size=0x%08x\n",
                    type,
                    allocReq->mem.phys_addr,
                    allocReq->mem.virt_addr,
                    allocReq->mem.size
                    );
      }

      memset(&shared_mem[type], 0, sizeof(shared_mem_t));
   }
   else
   {
      /* Failed: invalid shared memory type */
      log_printf(LOG_ERROR "dsp_shm_free: invalid mem argument (phys_addr=0x%08x not allocated).\n",
                 _mem.phys_addr
                 );
      ret = 10;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_mspace_create() */
dsp_mspace_handle_t dsp_mspace_create(sU32 _baseAddr, sU32 _size) {
   dsp_mspace_t *ret = malloc(sizeof(dsp_mspace_t));

   if(NULL != ret)
   {
      ret->msp = create_mspace_with_base((void*)_baseAddr, _size, 1);

      if(NULL == ret->msp)
      {
         /* Failed */
         
         log_printf(LOG_ERROR "dsp_mspace_create: create_mspace_with_base() failed (base=0x%08x size=0x%08x).\n",
                    _baseAddr,
                    _size
                    );

         free(ret);
         
         ret = NULL;
      }
      else
      {
         /* Succeeded */
         
         ret->base_addr = _baseAddr;
         ret->size      = _size;
         
         ret->total_num_bytes = 0;
         ret->total_allocs    = 0;
         ret->total_frees     = 0;
         
         if(LOG_DEBUG_LEVEL(5))
         {
            log_printf(LOG_TRACE "dsp_mspace_create: created mspace 0x%08x (base=0x%08x, size=0x%08x)\n",
                       (sU32)ret,
                       _baseAddr,
                       _size
                       );
         }
         
      }
   }
   else
   {
      /* Failed */

      log_printf(LOG_ERROR "dsp_mspace_create: failed to allocate mspace struct (%u bytes).\n",
                 sizeof(dsp_mspace_t)
                 );
   }

   return (dsp_mspace_handle_t) ret;
}


/*--------------------------------------------------------------------------- dsp_mspace_malloc() */
void *dsp_mspace_malloc(dsp_mspace_handle_t _mspace, sU32 _numBytes) {
   void *ret;

   if(NULL != _mspace)
   {
      dsp_mspace_t *dms = (dsp_mspace_t*)_mspace;
      sU32 *p;

      p = mspace_malloc(dms->msp, _numBytes + sizeof(sU32));

      if(NULL == p)
      {
         /* Failed */

         log_printf(LOG_ERROR "dsp_mspace_malloc: failed to allocate %u bytes (%.3f KBytes).\n",
                    _numBytes,
                    (_numBytes / 1024.0f)
                    );

         ret = NULL;
      }
      else
      {
         /* Succeeded */

         p[0] = _numBytes;

         dms->total_allocs++;
         dms->total_num_bytes += (sSI)_numBytes;

         ret = p + 1;

         if(LOG_DEBUG_LEVEL(15))
         {
            log_printf(LOG_TRACE "dsp_mspace_malloc: allocated %u bytes (%.3f KBytes) @0x%08x (total=%.3f KBytes)\n",
                       _numBytes,
                       (_numBytes / 1024.0f),
                       (sU32) ret,
                       (dms->total_num_bytes / 1024.0f)
                       );
         }

      }
   }
   else
   {
      /* Failed: mspace is NULL */
      log_printf(LOG_ERROR "dsp_mspace_malloc: invalid mspace handle (NULL).\n");

      ret = NULL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_mspace_memalign() */
void *dsp_mspace_memalign(dsp_mspace_handle_t _mspace, sU32 _alignment, sU32 _numBytes) {
   void *ret;

   if(NULL != _mspace)
   {
      dsp_mspace_t *dms = (dsp_mspace_t*)_mspace;
      sU32 *p;

      p = mspace_memalign(dms->msp, _alignment, _numBytes + sizeof(sU32));
      
      if(NULL == p)
      {
         /* Failed */

         log_printf(LOG_ERROR "dsp_mspace_memalign: failed to allocate %u bytes (%.3f KBytes) (align=%u).\n",
                    _numBytes,
                    (_numBytes / 1024.0f),
                    _alignment
                    );

         ret = NULL;
      }
      else
      {
         /* Succeeded */

         p[0] = _numBytes;

         dms->total_allocs++;
         dms->total_num_bytes += (sSI)_numBytes;

         ret = p + 1;

         if(LOG_DEBUG_LEVEL(15))
         {
            log_printf(LOG_TRACE "dsp_mspace_memalign: allocated %u bytes (%.3f KBytes) @0x%08x (total=%.3f KBytes)\n",
                       _numBytes,
                       (_numBytes / 1024.0f),
                       (sU32) ret,
                       (dms->total_num_bytes / 1024.0f)
                       );
         }

      }
   }
   else
   {
      /* Failed: mspace is NULL */
      log_printf(LOG_ERROR "dsp_mspace_memalign: invalid mspace handle (NULL).\n");

      ret = NULL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_mspace_free() */
void dsp_mspace_free(dsp_mspace_handle_t _mspace, void *_ptr) {

   if(NULL != _ptr)
   {
      if(NULL != _mspace)
      {
         dsp_mspace_t *dms = (dsp_mspace_t*)_mspace;

         sU32 *p = (sU32*)_ptr;

         sU32 numBytes = p[-1];
         
         if(LOG_DEBUG_LEVEL(15))
         {
            log_printf(LOG_TRACE "dsp_mspace_free: ptr=0x%p size=%u (%.3f KBytes) (total=%.3f KBytes)\n", 
                       _ptr,
                       numBytes,
                       (numBytes / 1024.0f),
                       (dms->total_num_bytes / 1024.0f)
                       );
         }

         mspace_free(dms->msp, &p[-1]);

         dms->total_frees++;
         dms->total_num_bytes -= (sSI)numBytes;
      }
      else
      {
         /* Failed: mspace is NULL */
         log_printf(LOG_ERROR "dsp_mspace_free: invalid mspace handle (NULL).\n");
      }
   }
   else
   {
      if(LOG_DEBUG_LEVEL(1))
      {
         log_printf(LOG_WARN "dsp_mspace_free: trying to free NULL ptr.\n");
      }
   }

}


/*--------------------------------------------------------------------------- dsp_mspace_destroy() */
void dsp_mspace_destroy(dsp_mspace_handle_t _mspace) {

   if(NULL != _mspace)
   {
      dsp_mspace_t *dms = (dsp_mspace_t*)_mspace;

      if(LOG_DEBUG_LEVEL(5))
      {
         log_printf(LOG_TRACE "dsp_mspace_destroy: destroying mspace 0x%p (base=0x%08x size=0x%08x)\n",
                    _mspace,
                    dms->base_addr,
                    dms->size
                    );
      }

      destroy_mspace(dms->msp);

      dms->msp       = NULL;
      dms->base_addr = 0;
      dms->size      = 0;
      
      free(dms);
   }
   else
   {
      /* Failed: mspace is NULL */
      log_printf(LOG_ERROR "dsp_mspace_free: invalid mspace handle (NULL).\n");
   }
}


/*--------------------------------------------------------------------------- dsp_l1sram_scratchbuffer_get() */
dsp_mem_region_t dsp_l1sram_scratchbuffer_get(void) {
   dsp_mem_region_t ret;

   ret = l1sram_fshm;

   ret.size = DSP_L1SRAM_SCRATCH_SIZE;

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fshm_scratchbuffer_get() */
dsp_mem_region_t dsp_fshm_scratchbuffer_get(void) {
   /* **deprecated** */
   return dsp_l1sram_scratchbuffer_get();
}


/*--------------------------------------------------------------------------- dsp_l1sram_alloc() */
dsp_mem_region_t dsp_l1sram_alloc(sU32 _numBytes) {
   dsp_mem_region_t mem;
   int r;

   mem.phys_addr = 0; /* paranoia */
   mem.virt_addr = 0; /* paranoia */
   mem.size      = _numBytes;

   r = ioctl(c64_fd, C64_IOCTL_L1SRAM_ALLOC, &mem);

   if(0 == r)
   {
      /* Succeeded */
      mem.virt_addr = (mem.phys_addr - l1sram_fshm.phys_addr) + l1sram_fshm.virt_addr;
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l1sram_alloc: failed to allocate %u L1DSRAM bytes. errno=%d (\"%s\").\n",
                 _numBytes,
                 errno, strerror(errno)
                 );
   }

   return mem;
}


/*--------------------------------------------------------------------------- dsp_fshm_alloc() */
dsp_mem_region_t dsp_fshm_alloc(sU32 _numBytes) {
   /* **deprecated** */
   return dsp_l1sram_alloc(_numBytes);
}


/*--------------------------------------------------------------------------- dsp_l1sram_debug() */
int dsp_l1sram_debug(sUI _sequenceId) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_L1SRAM_DEBUG, _sequenceId);

   if(0 == ret)
   {
      /* Succeeded */
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l1sram_debug: ioctl() failed. errno=%d (\"%s\").\n",
                 errno, strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fshm_debug() */
int dsp_fshm_debug(sUI _sequenceId) {
   /* **deprecated** */
   return dsp_l1sram_debug(_sequenceId);
}


/*--------------------------------------------------------------------------- dsp_l1sram_free() */
int dsp_l1sram_free (dsp_mem_region_t _mem) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_L1SRAM_FREE, &_mem);

   if(0 == ret)
   {
      /* Succeeded */
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l1sram_free: failed to free %u L1DSRAM bytes (phys=0x%08x, virt=0x%08x). errno=%d (\"%s\").\n",
                 _mem.size,
                 _mem.phys_addr,
                 _mem.virt_addr,
                 errno, strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fshm_free() */
int dsp_fshm_free (dsp_mem_region_t _mem) {
   /* **deprecated **/
   return dsp_l1sram_free(_mem);
}


/*--------------------------------------------------------------------------- dsp_l2sram_alloc() */
dsp_mem_region_t dsp_l2sram_alloc(sU32 _numBytes) {
   dsp_mem_region_t mem;
   int r;

   mem.phys_addr = 0; /* paranoia */
   mem.virt_addr = 0; /* paranoia */
   mem.size      = _numBytes;

   r = ioctl(c64_fd, C64_IOCTL_L2SRAM_ALLOC, &mem);

   if(0 == r)
   {
      /* Succeeded */
      mem.virt_addr = (mem.phys_addr - l2sram_fshm.phys_addr) + l2sram_fshm.virt_addr;
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l2sram_alloc: failed to allocate %u L2SRAM bytes. errno=%d (\"%s\").\n",
                 _numBytes,
                 errno, strerror(errno)
                 );
   }

   return mem;
}


/*--------------------------------------------------------------------------- dsp_l2sram_free() */
int dsp_l2sram_free (dsp_mem_region_t _mem) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_L2SRAM_FREE, &_mem);

   if(0 == ret)
   {
      /* Succeeded */
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l2sram_free: failed to free %u L2SRAM bytes (phys=0x%08x, virt=0x%08x). errno=%d (\"%s\").\n",
                 _mem.size,
                 _mem.phys_addr,
                 _mem.virt_addr,
                 errno, strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_l2sram_debug() */
int dsp_l2sram_debug(sUI _sequenceId) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_L2SRAM_DEBUG, _sequenceId);

   if(0 == ret)
   {
      /* Succeeded */
   }
   else
   {
      log_printf(LOG_ERROR "dsp_l2sram_debug: ioctl() failed. errno=%d (\"%s\").\n",
                 errno, strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_virt_to_phys() */
sU32 dsp_virt_to_phys(void *_virt) {
   sU32 ret = 0;
   sU32 virt = (sU32)_virt;

   if( (virt >= l1sram_fshm.virt_addr) && (virt < (l1sram_fshm.virt_addr + l1sram_fshm.size)) )
   {
      ret = (virt - l1sram_fshm.virt_addr) + l1sram_fshm.phys_addr;
   }
   else if( (virt >= l2sram_fshm.virt_addr) && (virt < (l2sram_fshm.virt_addr + l2sram_fshm.size)) )
   {
      ret = (virt - l2sram_fshm.virt_addr) + l2sram_fshm.phys_addr;
   }
   else
   {
      dsp_cache_t type;

      for(type=0; type < NUM_DSP_CACHE_TYPES; type++)
      {
         if( (virt >= shared_mem[type].shm_alloc_req.mem.virt_addr) &&
             (virt < (shared_mem[type].shm_alloc_req.mem.virt_addr +
                      shared_mem[type].shm_alloc_req.mem.size))
             )
         {
            if(DSP_CACHE_HUGETLB == type)
            {
               sU32 memOff  = (virt - shared_mem[type].shm_alloc_req.mem.virt_addr);
               sUI  pageNr  = (memOff >> DSP_HUGETLB_PAGESHIFT);
               sU32 pageOff = memOff - (pageNr << DSP_HUGETLB_PAGESHIFT);
               
               ret = hugetlb_page_phys_addr[pageNr] + pageOff; 
            }
            else
            {
               ret = 
                  (virt - shared_mem[type].shm_alloc_req.mem.virt_addr) + 
                  shared_mem[type].shm_alloc_req.mem.phys_addr;
            }
            break;
         }
      }

   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_phys_to_virt() */
void *dsp_phys_to_virt(sU32 _phys) {
   void *ret = NULL;

   if( (_phys >= l1sram_fshm.phys_addr) && (_phys < (l1sram_fshm.phys_addr + l1sram_fshm.size)) )
   {
      ret = (void*) ( (_phys - l1sram_fshm.phys_addr) + l1sram_fshm.virt_addr );
   }
   else if( (_phys >= l2sram_fshm.phys_addr) && (_phys < (l2sram_fshm.phys_addr + l2sram_fshm.size)) )
   {
      ret = (void*) ( (_phys - l2sram_fshm.phys_addr) + l2sram_fshm.virt_addr );
   }
   else
   {
      dsp_cache_t type;

      for(type=0; type < NUM_CONTIG_DSP_CACHE_TYPES; type++)
      {
         if( (_phys >= shared_mem[type].shm_alloc_req.mem.phys_addr) &&
             (_phys < (shared_mem[type].shm_alloc_req.mem.phys_addr +
                       shared_mem[type].shm_alloc_req.mem.size))
             )
         {
            ret = (void*)
               (
                  (_phys - shared_mem[type].shm_alloc_req.mem.phys_addr) + 
                  shared_mem[type].shm_alloc_req.mem.virt_addr
                )
               ;
            break;
         }
      }

      if(NULL == ret)
      {
         /* Try hugetlb pages */
         if(0 != shared_mem[DSP_CACHE_HUGETLB].shm_alloc_req.mem.phys_addr)
         {
            sUI i;
            
            for(i=0; i<DSP_HUGETLB_MAXPAGES; i++)
            {
               if( (_phys >= hugetlb_page_phys_addr[i]) && (_phys < (hugetlb_page_phys_addr[i] + DSP_HUGETLB_PAGESIZE)) )
               {
                  sU32 pageOff = (_phys - hugetlb_page_phys_addr[i]);

                  ret = (void*)
                     (
                        shared_mem[DSP_CACHE_HUGETLB].shm_alloc_req.mem.virt_addr
                        + (i << DSP_HUGETLB_PAGESHIFT)
                        + pageOff
                      )
                     ;

                  break;
               }
            }
         } 
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_physgpp_to_physdsp() */
sU32 dsp_physgpp_to_physdsp(sU32 _physGPP) {
   sU32 ret;

   if((_physGPP >= IVA2_L1DSRAM_BASE_GPP) && (_physGPP < (IVA2_L1DSRAM_BASE_GPP + IVA2_L1DSRAM_SIZE)))
   {
      /* Translate L3 to local interconnect */
      ret = (_physGPP - IVA2_L1DSRAM_BASE_GPP) + IVA2_L1DSRAM_BASE_DSP;
   }
   else if((_physGPP >= IVA2_L2SRAM_BASE_GPP) && (_physGPP < (IVA2_L2SRAM_BASE_GPP + IVA2_L2SRAM_SIZE)))
   {
      /* Translate L3 to local interconnect */
      ret = (_physGPP - IVA2_L2SRAM_BASE_GPP) + IVA2_L2SRAM_BASE_DSP;
   }
   else
   {
      /* Regular RAM address, no translation needed */
      ret = _physGPP;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_component_id_find_by_name() */
int dsp_component_id_find_by_name(const char *_name, sU32 *_retSecIdxAndComponentId, sBool _bIncRef) {
   int ret;

   if(NULL != _name)
   {
      if(NULL != _retSecIdxAndComponentId)
      {
         sU32 coreErr;

         /* Copy component name to L1DSRAM */
         strncpy((void*)l1sram_fshm.virt_addr,
                _name,
                MLB_MAX_COMPONENT_NAME_LENGTH
                );

         if(LOG_DEBUG_LEVEL(20))
         {
            log_printf(LOG_TRACE "dsp_componend_id_find_by_name: _name=\"%s\" shmName=\"%s\".\n",
                       _name,
                       (const char*) (l1sram_fshm.virt_addr)
                       );
         }

         if(S_FALSE == _bIncRef)
         {
            ret = dsp_rpc_2a_2r(COMPONENT_CORE,
                                CORE_CMD_COM_FIND,
                                l1sram_fshm.phys_addr,
                                S_FALSE, /*bIncRef*/
                                &coreErr,
                                _retSecIdxAndComponentId
                                );
         }
         else
         {
            ret = dsp_fastcall_rpc(CORE_FC_CMD_COM_OVERLAY_FIND,
                                   l1sram_fshm.phys_addr,
                                   S_TRUE, /*bIncRef*/
                                   &coreErr,
                                   _retSecIdxAndComponentId
                                   );
         }

         if(0 == ret)
         {
            if(CORE_ERR_OK == coreErr)
            {
               /* Succeeded */

               ret = 0;
            }
            else
            {
               /* Failed: component not found */
               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_WARN "dsp_component_id_find_by_name: not found. name=\"%s\", coreErr=0x%08x\n",
                             _name,
                             coreErr
                             );
               }

               ret = -ENOENT;
            }
         }
         else
         {
            log_printf(LOG_ERROR "dsp_component_id_find_by_name: dsp_rpc() [CORE_CMD_COM_FIND] failed. ret=%d, errno=%d (\"%s\")\n",
                       ret,
                       errno, strerror(errno)
                       );
         }
      }
      else
      {
         /* Failed: return address is NULL */
         ret = -EINVAL;
      }
   }
   else
   {
      /* Failed: name is NULL */
      ret = -EINVAL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_determine_area_and_basename() */
static sUI loc_determine_area_and_basename(const char *_searchPath,
                                           const char *_componentName,
                                           char *fname,
                                           char *fbasename
                                           ) {
   sUI area = 0;
   FILE *fh;

   /* Determine area from .out filename */
   
   if(NULL != _searchPath)
   {
      sprintf(fname, "%s/%s_area2_1.out", _searchPath, _componentName);
      
      fh = fopen(fname, "rb");
      if(NULL != fh)
      {
         fclose(fh);
         
         area = C64_OVERLAY_AREA2;
         
         sprintf(fbasename, "%s/%s_area2_", _searchPath, _componentName);
      }
      else
      {
         sprintf(fname, "%s/%s_area3.out", _searchPath, _componentName);
         
         fh = fopen(fname, "rb");
         if(NULL != fh)
         {
            fclose(fh);
            
            area = C64_OVERLAY_AREA3;
         }
      }
   }
   else
   {
      /* try current working directory (area2) */
      sprintf(fname, "%s_area2_1.out", _componentName);
      
      fh = fopen(fname, "rb");
      if(NULL != fh)
      {
         fclose(fh);
         
         area = C64_OVERLAY_AREA2;
         
         sprintf(fbasename, "%s_area2_", _componentName);
      }
      else
      {
         /* try C64_IMAGE_SITE_PATH (area2) */
         sprintf(fname, "%s/%s_area2_1.out", C64_IMAGE_SITE_PATH, _componentName);
         
         fh = fopen(fname, "rb");
         if(NULL != fh)
         {
            fclose(fh);
            
            area = C64_OVERLAY_AREA2;
            
            sprintf(fbasename, "%s/%s_area2_", C64_IMAGE_SITE_PATH, _componentName);
         }
         else
         {
            /* try current working directory (area3) */
            sprintf(fname, "%s_area3.out", _componentName);
            
            fh = fopen(fname, "rb");
            if(NULL != fh)
            {
               fclose(fh);
               
               area = C64_OVERLAY_AREA3;
            }
            else
            {
               /* try C64_IMAGE_SITE_PATH (area3) */
               sprintf(fname, "%s/%s_area3.out", C64_IMAGE_SITE_PATH, _componentName);
               
               fh = fopen(fname, "rb");
               if(NULL != fh)
               {
                  fclose(fh);
                  
                  area = C64_OVERLAY_AREA3;
               }
               else
               {
                  /* Nope, nothing. Overlay image not found */
                  log_printf(LOG_ERROR "dsp_component_load: component \"%s\" overlay DSP image (.out) not found.\n",
                             _componentName
                             );
                  
                  /* Failed */
                  area = 0;
               }
            }
         }
      }
   }

   return area;
}


/*--------------------------------------------------------------------------- loc_overlay_component_register_begin() */
static int loc_overlay_component_register_begin(void) {
   dsp_msg_t req;
   int ret;
   
   DSP_MSG_INIT_CORE(&req, CORE, CMD_COM_OVERLAY_INITIATE, 0, 0);

   ret = dsp_rpc_send(&req);
   
   if(0 == ret)
   {
      /* Succeeded */
   }
   else
   {
      log_printf(LOG_ERROR "loc_overlay_component_register_begin: failed to send OVERLAY_INITIATE RPC to DSP. errno=%d (\"%s\")\n",
                 errno, strerror(errno)
                 );
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- loc_overlay_component_register_new() */
static int loc_overlay_component_register_new(sU32 _secIdx, sU32 *_retMainComponentId) {
   int ret;
   
   sU32 coreErr = ~0;

   ret = dsp_fastcall_rpc(CORE_FC_CMD_COM_OVERLAY_REGISTER,
                          _secIdx, 0,
                          &coreErr, _retMainComponentId
                          );
   
   if(0 == ret)
   {
      if(CORE_ERR_OK == coreErr)
      {
         if(LOG_DEBUG_LEVEL(10))
         {
            log_printf(LOG_DEBUG "loc_overlay_component_register_new: ok, registered section %u main component (id=%u).\n",
                       _secIdx, *_retMainComponentId
                       );
         }
         
         /* Succeeded */
      }
      else
      {
         log_printf(LOG_ERROR "loc_overlay_component_register_new: failed to register section %u main component. ret=%d\n",
                    _secIdx, ret
                    );
         
         ret = (int) coreErr;
      }
   }
   else
   {
      log_printf(LOG_ERROR "loc_overlay_component_register_new: failed to send OVERLAY_REGISTER fastcall to DSP. errno=%d (\"%s\"). ret=%d\n",
                 errno, strerror(errno), ret
                 );
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- dsp_component_load() */
int dsp_component_load(const char *_searchPath,
                       const char *_componentName,
                       dsp_component_id_t *_retComponentId
                       ) {
   int ret;
   sU32 secIdxAndComponentId;
   sU16 secIdx;

   if(LOG_DEBUG_LEVEL(20))
   {
      log_printf(LOG_TRACE "dsp_component_load: trying to locate component \"%s\".\n",
                 _componentName
                 );
   }

   /* Lock down DSP during overlay query resp. registration + code update */
   ret = (int) loc_overlay_component_register_begin();

   if(0 == ret)
   {
      ret = dsp_component_id_find_by_name(_componentName,
                                          &secIdxAndComponentId,
                                          S_TRUE /* bIncRef */
                                          );
      if(0 == ret)
      {
         /* ok, that was easy. The component is already loaded. */
         
         *_retComponentId = (dsp_component_id_t) (secIdxAndComponentId & 65535u);
         
         secIdx = (secIdxAndComponentId >> 16);
         
         /* Will be unregistered (resp. refcount decreased) at exit */
         overlays[secIdx].compid  = *_retComponentId;
         overlays[secIdx].sec_idx = secIdx;
         
         /* Set phys_addr so that OVERLAY_FREE is called at exit */
         overlays[secIdx].mem = overlay_sections[secIdx];
         
         ret = ioctl(c64_fd, C64_IOCTL_OVERLAY_REF, secIdx);
         
         if(0 == ret)
         {
            if(LOG_DEBUG_LEVEL(5))
            {
               log_printf(LOG_TRACE "dsp_component_load: OK, component \"%s\" already loaded, id=%u secIdx=%u.\n",
                          _componentName,
                          *_retComponentId,
                          secIdx
                          );
            }
            
            /* Succeeded */
         }
         else
         {
            log_printf(LOG_ERROR "dsp_component_load: ioctl [OVERLAY_REF] secIdx=%u failed. errno=%d (\"%s\")\n",
                       secIdx,
                       errno, strerror(errno)
                       );
         }
      }
      else if(-ENOENT == ret) /* not registered, yet ? */
      {
         char fname[1024];
         char fbasename[1024];
         sUI area; /* one of C64_OVERLAY_AREAx */
         
         area = loc_determine_area_and_basename(_searchPath, _componentName, fname, fbasename);
         
         if(LOG_DEBUG_LEVEL(20))
         {
            log_printf(LOG_TRACE "dsp_component_load: component \"%s\" area is %u.\n",
                       _componentName,
                       area
                       );
         }
         
         if(0 != area)
         {
            c64_dev_ioctl_overlay_alloc_t req;
            
            req.overlay_area = area;
            
            ret = ioctl(c64_fd, C64_IOCTL_OVERLAY_ALLOC, &req);

            if(0 == ret)
            {
               overlays[req.ret.section_idx].sec_idx = req.ret.section_idx;
               overlays[req.ret.section_idx].mem     = req.ret.mem;
               
               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_DEBUG "dsp_component_load: ok, overlay area %u section_idx=%u, phys_addr=0x%08x, size=0x%08x\n",
                             area,
                             req.ret.section_idx,
                             req.ret.mem.phys_addr,
                             req.ret.mem.size
                             );
               }
               
               if(C64_OVERLAY_AREA2 == area)
               {
                  sprintf(fname, "%s%u.out", fbasename, (req.ret.section_idx + 1));
               }
               
               if(LOG_DEBUG_LEVEL(1))
               {
                  log_printf(LOG_DEBUG "dsp_component_load: loading overlay image from \"%s\".\n",
                             fname
                             );
               }
               
               /* Map DSP code area */
               req.ret.mem.virt_addr = (sU32) mmap(0,
                                                   req.ret.mem.size,
                                                   (PROT_READ | PROT_WRITE),
                                                   MAP_SHARED,
                                                   c64_fd,
                                                   req.ret.mem.phys_addr
                                                   );
               
               if(MAP_FAILED != (void*)req.ret.mem.virt_addr)
               {
                  overlays[req.ret.section_idx].mem = req.ret.mem; /* update virt_addr */
                  
                  /* Load COFF sections */
                  ret = coff_load_overlay(fname,
                                          (void*)req.ret.mem.virt_addr,
                                          req.ret.mem.phys_addr,
                                          req.ret.mem.size
                                          );
                  
                  if(0 == ret)
                  {
                     /* Register component on DSP side */
                     sU32 mcompid;
                     
                     if(LOG_DEBUG_LEVEL(10))
                     {
                        log_printf(LOG_DEBUG "dsp_component_load: registering new overlay component(s).\n");
                     }
                     
                     ret = (int) loc_overlay_component_register_new(req.ret.section_idx, &mcompid);
                     
                     if(0 == ret)
                     {
                        *_retComponentId = (dsp_component_id_t) mcompid;
                        
                        overlays[req.ret.section_idx].compid = (dsp_component_id_t) mcompid;
                        
                        if(LOG_DEBUG_LEVEL(1))
                        {
                           log_printf(LOG_INFO "dsp_component_load: overlay loaded OK. main component id=%u\n",
                                      mcompid
                                      );
                        }
                        
                        /* Succeeded */
                     }
                     else
                     {
                        /* Failed: loc_overlay_component_register_new() */
                        log_printf(LOG_ERROR "dsp_component_load: failed to register overlay main component (img=\"%s\"). ret=%d\n",
                                   fname,
                                   ret
                                   );
                        
                        munmap((void*)req.ret.mem.virt_addr,
                               req.ret.mem.size
                               );
                        
                        memset(&overlays[req.ret.section_idx].mem, 0, sizeof(dsp_mem_region_t));
                        
                        ioctl(c64_fd, C64_IOCTL_OVERLAY_FREE, req.ret.section_idx);
                     }
                  }
                  else
                  {
                     /* Failed: coff_load_overlay() */
                     log_printf(LOG_ERROR "dsp_component_load: coff_load_overlay(fname=\"%s\", vaddr=0x%08x, size=0x%08x) failed. ret=%d\n",
                                fname,
                                req.ret.mem.virt_addr,
                                req.ret.mem.size,
                                ret
                                );

                     munmap((void*)req.ret.mem.virt_addr,
                            req.ret.mem.size
                            );
                     
                     memset(&overlays[req.ret.section_idx].mem, 0, sizeof(dsp_mem_region_t));
                     
                     ioctl(c64_fd, C64_IOCTL_OVERLAY_FREE, req.ret.section_idx);
                  }
               }
               else
               {
                  /* Failed: mmap() */
                  log_printf(LOG_ERROR "dsp_component_load: failed to mmap overlay region. errno=%d (\"%s\").\n",
                             errno, strerror(errno)
                             );
                  
                  ioctl(c64_fd, C64_IOCTL_OVERLAY_FREE, req.ret.section_idx);
                  
                  memset(&overlays[req.ret.section_idx].mem, 0, sizeof(dsp_mem_region_t));
                  
                  ret = -EFAULT;
               }
               
            }
            else
            {
               /* Failed: ioctl() */
               log_printf(LOG_ERROR "dsp_component_load: ioctl [OVERLAY_ALLOC] failed. errno=%d \"%s\".\n",
                          errno, strerror(errno)
                          );
               
            }
            
         }
         else
         {
            /* Failed: loc_determine_area_and_basename() */
            ret = -ENOENT;
         }
      }

      dsp_fastcall_end();
   }
   else
   {
      /* Failed: loc_overlay_component_register_begin() */
      
      log_printf(LOG_ERROR "dsp_component_load: failed to start DSP side component query/registration.\n");
   }
   

   return ret;
}


/*--------------------------------------------------------------------------- dsp_ioctl_test() */
int dsp_ioctl_test(void) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_TEST);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_ioctl_test: ioctl() succeeded.\n");
      }
   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_ioctl_test: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_debug_internal_print() */
void dsp_debug_internal_print(void) {
   sU32 val;

   ssize_t numRead = read(c64_fd, &val, 4);
   
   {
      log_printf(LOG_DEBUG "dsp_print_debug_internal: DSP numRead=%u, debug=0x%08x\n", (sUI)numRead, val);
   }
}


/*--------------------------------------------------------------------------- dsp_debug_usr_get() */
int dsp_debug_usr_get(sU32 _idx, sU32 *_retVal) {
   int ret;
   c64_dev_ioctl_debug_usr_t req;

   req.idx = _idx;
   
   ret = ioctl(c64_fd, C64_IOCTL_DEBUG_USR, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_debug_usr_get: ioctl() succeeded val=0x%08x.\n", req.val);
      }

      if(NULL != _retVal)
      {
         *_retVal = req.val;
      }
   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_debug_usr_get: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_debug_usr_print() */
void dsp_debug_usr_print(sU32 _idx) {
   sU32 val;
   
   if(0 == dsp_debug_usr_get(_idx, &val))
   {
      log_printf(LOG_DEBUG "dsp_debug_usr_print: debug_usr[%u]=0x%08x.\n", _idx, val);
   }
}


/*--------------------------------------------------------------------------- dsp_logbuf_print() */
void dsp_logbuf_print(void) {
   const char *buf = (const char *)logbuf.virt_addr;
   sUI off = logbuf_offset_read;

/*    printf("xxx dsp_logbuf_print: logbuf_offset_read=%u\n", logbuf_offset_read); */

   if(0 != buf[off])
   {
      sSI cntMax = (sSI) DSP_LOGBUF_SIZE;
      sBool bLF = S_FALSE;
      sBool bLZ = S_FALSE;
      sBool bLS = S_TRUE;

      if(LOG_DEBUG_LEVEL(20))
      {
         log_printf(LOG_TRACE "dsp_logbuf_print: <BEGIN>\n");
      }
      
      /* Slow, char-wise buffer traversal (but hey, it's just a debugging aid, right) */
      do
      {
         char c = buf[off];

         if((0 == c) && bLZ)
         {
            /* two successive ASCIIZ == end of buffer */
            break;
         }

         off++;
         cntMax--;
         
         if(off >= DSP_LOGBUF_SIZE)
         {
            off -= DSP_LOGBUF_SIZE;
         }
         
         if(0 != c)
         {
            bLZ = S_FALSE;
            bLF = ('\n' == c);
            
            if(bLS)
            {
               log_printf("DSP: ");

               bLS = S_FALSE;
            }

            log_putc(c);

            bLS = bLF;
         }
         else
         {
            /* end of string */

            bLZ = S_TRUE;

#if 0
            if(!bLF)
            {
               log_putc('\n');

               bLF = S_TRUE;
            }
#endif
         }
         
      } while(--cntMax > 0);

      logbuf_offset_read = off;

/*       printf("xxx after read: logbuf_offset_read=%u c=%u lc=%u\n", */
/*              logbuf_offset_read, */
/*              buf[logbuf_offset_read], */
/*              buf[logbuf_offset_read-1] */
/*              ); */
      
      if(!bLF)
      {
         log_putc('\n');
      }
      
      if(LOG_DEBUG_LEVEL(20))
      {
         log_printf(LOG_TRACE "dsp_logbuf_print: <END>\n");
      }
   }
   else
   {
      /* No new messages */
      if(LOG_DEBUG_LEVEL(20))
      {
         log_printf(LOG_TRACE "dsp_logbuf_print: <no new messages>\n");
      }
   }
}


/*--------------------------------------------------------------------------- dsp_rpc_send() */
int dsp_rpc_send(const dsp_msg_t *_msgTo) {
   /* Send RPC message to DSP */
   int ret;
   sUI fcBusyRetryCount = 0;

  dsp_rpc_send_fcbusy_retry:
   if(DSP_FSHM_FASTCALL_CTL_DONE == FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF))
   {
      ssize_t numWritten;
      sUI retryCount = 0;

      rpc_count++;

      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_TRACE "dsp_rpc_send[%08x]: compid=%u cmd=%u arg1=0x%08x arg2=0x%08x\n",
                    rpc_count,
                    _msgTo->req.fields.dsp_component_id,
                    _msgTo->req.fields.dsp_cmd_id,
                    _msgTo->data[0].arg,
                    _msgTo->data[1].arg
                    );
      }

      FC_REG(DSP_FSHM_MSG_TODSP_CTL_GPP_OFF) = DSP_FSHM_FASTCALL_CTL_IDLE;

      do
      {
         /* (todo) implement select() for write ops */
         numWritten = write(c64_fd, _msgTo, sizeof(dsp_msg_t));

         retryCount++;

         if(0 == (retryCount & MASK_RPC_WRITE_YIELD_RETRIES))
         {
            osal_yield();
         }
      }
      while( (sizeof(dsp_msg_t) != numWritten) && (EAGAIN == errno) && (retryCount < MAX_RPC_WRITE_RETRIES) );

      if(MAX_RPC_WRITE_RETRIES != retryCount)
      {
         if(sizeof(dsp_msg_t) == numWritten)
         {
            if(LOG_DEBUG_LEVEL(10))
            {
               log_printf(LOG_TRACE "dsp_rpc_send[%08x]: ok, message sent.\n",
                          rpc_count
                          );
            }

            /* Succeeded */
            ret = 0;
         }
         else
         {
            /* Failed: write() */
            log_printf(LOG_ERROR "dsp_rpc_send[%08x]: write() failed. errno=%d (\"%s\").\n", 
                       rpc_count,
                       errno,
                       strerror(errno)
                       );
         
            ret = 10;
         }
      }
      else
      {
         /* Failed: max. number of write retries exceeded */
         log_printf(LOG_ERROR "dsp_rpc_send[%08x]: write() timeout after %u retries.\n",
                    rpc_count,
                    MAX_RPC_WRITE_RETRIES
                    );

         ret = 10;
      }
   }
   else
   {
      /* Failed: DSP is currently tied up in a fastcall sequence ==> Retry */
      fcBusyRetryCount++;

      if(fcBusyRetryCount < MAX_RPC_FCBUSY_RETRIES)
      {
         osal_msleep(RPC_FCBUSY_MSLEEP);

         goto dsp_rpc_send_fcbusy_retry;
      }
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send: DSP fastcall in progress, cannot send RPC (after %u retries).\n",
                    fcBusyRetryCount
                    );
      }

      ret = 10;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_rpc_recv() */
int dsp_rpc_recv(dsp_msg_t *_msgFrom) {
   int ret;
   sUI retryCount = 0;
   ssize_t numRead;
   fd_set fdSet;
   struct timeval timeout;
   dsp_msg_t tmpReply;

   /* Read RPC return values */

   if(S_FALSE == b_poll)
   {
      FD_ZERO(&fdSet);
      FD_SET(c64_fd, &fdSet);
      timeout.tv_sec  = RPC_RECV_TIMEOUT_SEC;
      timeout.tv_usec = RPC_RECV_TIMEOUT_USEC;
      
      ret = select(c64_fd + 1, 
                   &fdSet,     /* read  fds */
                   NULL,       /* write fds */
                   NULL,       /* except fds */
                   &timeout
                   );
      
      if(ret > 0)
      {
         /* ok, reply message is available */
         if(NULL != _msgFrom)
         {
            numRead = read(c64_fd, _msgFrom, sizeof(dsp_msg_t));
         }
         else
         {
            numRead = read(c64_fd, &tmpReply, sizeof(dsp_msg_t));
         }
         
         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf(LOG_TRACE "dsp_rpc_recv[%08x]: [%04u] numRead=%u\n", rpc_count, retryCount, numRead);
         }
      }
      else if(0 == ret)
      {
         /* timeout */
         retryCount = MAX_RPC_READ_RETRIES;
         
         log_printf(LOG_ERROR "dsp_rpc_recv[0x%08x]: timeout after %u seconds.\n",
                    rpc_count,
                    RPC_RECV_TIMEOUT_SEC
                    );
         
         numRead = 0;
      }
      else
      {
         /* Failed: select() */
         log_printf(LOG_ERROR "dsp_rpc_recv: select() failed. ret=%d. errno=%d (\"%s\").\n",
                    ret, errno, strerror(errno)
                    );
         
         numRead = 0;
      }
   }
   else
   {
      /* CPU-intensive polling mode (but lower message latencies) */

      do
      {
         /* (note) read() can fail with EAGAIN while DSP reply is not available, yet */
         if(NULL != _msgFrom)
         {
            numRead = read(c64_fd, _msgFrom, sizeof(dsp_msg_t));
         }
         else
         {
            numRead = read(c64_fd, &tmpReply, sizeof(dsp_msg_t));
         }
         
         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf("dsp_rpc_recv[%08x]: [%04u] numRead=%u\n", rpc_count, retryCount, numRead);
         }
      
         retryCount++;
         
         if(0 == (retryCount & MASK_RPC_READ_YIELD_RETRIES))
         {
            osal_yield();
         }
      }
      while((sizeof(dsp_msg_t) != numRead) && (EAGAIN == errno) && (0 != numRead) && (retryCount < MAX_RPC_READ_RETRIES));
      
      if(MAX_RPC_READ_RETRIES == retryCount)
      {
         log_printf(LOG_ERROR "dsp_rpc_recv[%08x]: read() timeout after %u retries.\n",
                    rpc_count,
                    MAX_RPC_READ_RETRIES
                    );
      }
   }
   
   if(sizeof(dsp_msg_t) == numRead)
   {
      if(LOG_DEBUG_LEVEL(10))
      {
         if(NULL != _msgFrom)
         {
            log_printf(LOG_INFO "dsp_rpc_recv[%08x]: ok, got reply from DSP. ret1=0x%08x ret2=0x%08x\n", 
                       rpc_count,
                       _msgFrom->data[0].ret,
                       _msgFrom->data[1].ret
                       );
         }
         else
         {
            log_printf(LOG_INFO "dsp_rpc_recv[%08x]: ok, got reply from DSP. ret1=0x%08x ret2=0x%08x (discarded)\n", 
                       rpc_count,
                       tmpReply.data[0].ret,
                       tmpReply.data[1].ret
                       );
         }
      }
      
      /* Succeeded */
      ret = 0;
   }
   else
   {
      /* Failed: read() reply */
      log_printf(LOG_ERROR "dsp_rpc_recv[%08x]: read() failed. poll=%d retryCount=%u numRead=%u errno=%d (\"%s\").\n", 
                 b_poll,
                 rpc_count,
                 retryCount,
                 numRead,
                 errno,
                 strerror(errno)
                 );
      
      dsp_debug_internal_print();
      
      ret = 10;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_rpc() */
int dsp_rpc(const dsp_msg_t *_msgTo,
                  dsp_msg_t *_msgFrom
            ) {

   int ret;

   ret = dsp_rpc_send(_msgTo);

   if(0 == ret)
   {
      ret = dsp_rpc_recv(_msgFrom);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_rpc_2a_1r() */
int dsp_rpc_2a_1r(sU16 _compId, 
                  sU16 _cmdId, 
                  sU32 _arg1, 
                  sU32 _arg2, 
                  sU32 *_ret
                  ) {
   dsp_msg_t msg;
   dsp_msg_t reply;
   int ret;

   msg.req.fields.dsp_component_id = _compId;
   msg.req.fields.dsp_cmd_id       = _cmdId;

   msg.data[0].arg = _arg1;
   msg.data[1].arg = _arg2;

   ret = dsp_rpc(&msg, &reply);

   if(0 == ret)
   {
      if(NULL != _ret)
      {
         *_ret = reply.data[0].ret;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_rpc_2a_2r() */
int dsp_rpc_2a_2r(sU16 _compId, 
                  sU16 _cmdId, 
                  sU32 _arg1, 
                  sU32 _arg2, 
                  sU32 *_ret1,
                  sU32 *_ret2
                  ) {
   dsp_msg_t msg;
   dsp_msg_t reply;
   int ret;

   msg.req.fields.dsp_component_id = _compId;
   msg.req.fields.dsp_cmd_id       = _cmdId;

   msg.data[0].arg = _arg1;
   msg.data[1].arg = _arg2;

   ret = dsp_rpc(&msg, &reply);

   if(0 == ret)
   {
      if(NULL != _ret1)
      {
         *_ret1 = reply.data[0].ret;
      }

      if(NULL != _ret2)
      {
         *_ret2 = reply.data[1].ret;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fastcall_rpc_send() */
int dsp_fastcall_rpc_send(sU32 _cmdFC,
                          sU32 _arg1, sU32 _arg2
                          ) {
   int ret;
   sU32 fcCtl;
   sU32 timeout = 0;

/*    log_printf("xxx read FSHM_MSG_CTL\n"); */

   /* Wait until DSP has initiated fastcall sequence */
   fcCtl = FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);

   while((DSP_FSHM_FASTCALL_CTL_IDLE != fcCtl) &&
         (timeout < DSP_FASTCALL_TIMEOUT)
         )
      
   {
      fcCtl = FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);
      timeout++;

      asm("\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          );
   }

   if(DSP_FSHM_FASTCALL_CTL_IDLE == fcCtl)
   {
      FC_REG(DSP_FSHM_MSG_TODSP_CMD_GPP_OFF)  = _cmdFC;
      FC_REG(DSP_FSHM_MSG_TODSP_ARG1_GPP_OFF) = _arg1;
      FC_REG(DSP_FSHM_MSG_TODSP_ARG2_GPP_OFF) = _arg2;

      FC_REG(DSP_FSHM_MSG_TODSP_CTL_GPP_OFF) = DSP_FSHM_FASTCALL_CTL_REQ;

      ret = 0;
   }
   else
   {
      /* Failed: DSP fastcall sequence has not been initiated by DSP */
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_ERROR "dsp_fastcall_rpc: fastcall sequence has not been initiated by DSP.\n");
      }

      ret = 10;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fastcall_rpc_recv() */
int dsp_fastcall_rpc_recv(sU32 *_r1FC, sU32 *_r2FC) {
   int ret;
   sU32 fcCtl;
   sU32 timeout;

   timeout = 0;

   do
   {
      /* Wait for reply from DSP */
      fcCtl = FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);

      timeout++;

      asm("\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          );
   }
   while((fcCtl != DSP_FSHM_FASTCALL_CTL_ACK) &&
         (timeout < DSP_FASTCALL_TIMEOUT)
         );
      
   FC_REG(DSP_FSHM_MSG_TODSP_CTL_GPP_OFF) = DSP_FSHM_FASTCALL_CTL_IDLE;

   if(timeout < DSP_FASTCALL_TIMEOUT)
   {
      if(1)
      {
         /* Succeeded */

         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf(LOG_INFO "dsp_fastcall_rpc: succeeded!\n");
         }

         if(NULL != _r1FC)
         {
            *_r1FC = FC_REG(DSP_FSHM_MSG_TOGPP_RET1_GPP_OFF);
         }

         if(NULL != _r2FC)
         {
            *_r2FC = FC_REG(DSP_FSHM_MSG_TOGPP_RET2_GPP_OFF);
         }

            
         ret = 0;
      }
      else
      {
         /* Failed */
         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf(LOG_ERROR "dsp_fastcall_rpc: failed, fcCtl=0x%08x\n", fcCtl);
         }

         ret = 10;
      }
   }
   else
   {
      /* Failed: DSP timeout */
      log_printf(LOG_ERROR "dsp_fastcall_rpc: DSP timeout waiting for reply.\n");
         
      ret = 10;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fastcall_rpc() */
int dsp_fastcall_rpc(sU32 _cmdFC,
                     sU32 _arg1, sU32 _arg2,
                     sU32 *_r1FC, sU32 *_r2FC
                     ) {

   int ret;

   ret = dsp_fastcall_rpc_send(_cmdFC, _arg1, _arg2);

   if(0 == ret)
   {
      ret = dsp_fastcall_rpc_recv(_r1FC, _r2FC);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_fastcall_end() */
void dsp_fastcall_end(void) {

   sU32 fcCtl;
   sU32 timeout = 0;

#if 1
   /* Wait until DSP has acknowledged last fastcall */
   fcCtl = FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);

   while((DSP_FSHM_FASTCALL_CTL_IDLE != fcCtl) &&
         (timeout < DSP_FASTCALL_TIMEOUT)
         )
      
   {
      fcCtl = FC_REG(DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);
      timeout++;

      asm("\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          "\n\t nop"
          );
   }

   if(DSP_FASTCALL_TIMEOUT == timeout)
   {
      log_printf(LOG_ERROR "dsp_fastcall_end: timeout while waiting for DSP to ack. last request. fcCtl=0x%08x\n", fcCtl);
   }
#endif
  
   FC_REG(DSP_FSHM_MSG_TODSP_CTL_GPP_OFF) = DSP_FSHM_FASTCALL_CTL_DONE;

   {
      dsp_msg_t msg;

      dsp_rpc_recv(&msg);
   }
}


/*--------------------------------------------------------------------------- dsp_cache_wb() */
void dsp_poll_enable(sBool _bEnabled) {
   b_poll = _bEnabled;
}


/*--------------------------------------------------------------------------- dsp_cache_wb() */
int dsp_cache_wb(sU32 _physAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_t req;

   req.phys_addr = _physAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_WB;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_wb: ioctl() succeeded.\n");
      }
   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_wb: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_wb_virt() */
int dsp_cache_wb_virt (void *_virtAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_virt_t req;

   req.virt_addr = (sU32)_virtAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_WB;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE_VIRT, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_wb_virt: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_wb_virt: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_inv() */
int dsp_cache_inv(sU32 _physAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_t req;

   req.phys_addr = _physAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_INV;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_inv: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_inv: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_inv_virt() */
int dsp_cache_inv_virt (void *_virtAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_virt_t req;

   req.virt_addr = (sU32)_virtAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_INV;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE_VIRT, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_inv_virt: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_inv_virt: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_wbinv() */
int dsp_cache_wbinv(sU32 _physAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_t req;

   req.phys_addr = _physAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_WBINV;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_wbinv: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_wbinv: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_wbinv_virt() */
int dsp_cache_wbinv_virt (void *_virtAddr, sU32 _size) {
   /* _size is #bytes */
   int ret;
   c64_dev_ioctl_cache_virt_t req;

   req.virt_addr = (sU32)_virtAddr;
   req.size      = _size;
   req.action    = C64_CACHE_AC_WBINV;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE_VIRT, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_wbinv_virt: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_wbinv_virt: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_cache_wbinvall() */
int dsp_cache_wbinvall(void) {
   int ret;
   c64_dev_ioctl_cache_t req;

   req.phys_addr = 0;
   req.size      = 0;
   req.action    = C64_CACHE_AC_WBINVALL;
   
   ret = ioctl(c64_fd, C64_IOCTL_CACHE, &req);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_INFO "dsp_cache_wbinvall: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_cache_wbinvall: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_send_forced_message() */
void dsp_send_forced_message(sU32 _val) {
   
   ssize_t numWritten = write(c64_fd, &_val, 4);

   if(LOG_DEBUG_LEVEL(10))
   {
      log_printf(LOG_INFO "dsp_send_forced_message: %u bytes written.\n", numWritten);
   }
   
   if(LOG_DEBUG_LEVEL(20))
   {
      dsp_debug_internal_print();
   }
}


/*--------------------------------------------------------------------------- dsp_power_on_and_reset() */
dsp_error_t S_CALL dsp_power_on_and_reset(void) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_PWR_ON_RST);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_power_on_and_reset: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_power_on_and_reset: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_start() */
dsp_error_t S_CALL dsp_start(sU32 _entryPoint) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_START, _entryPoint);
   
   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_start: ioctl() succeeded.\n");
      }
   }
   else
   {
      /* Failed: ioctl() */
      log_printf(LOG_ERROR "dsp_start: ioctl() failed. errno=%d (\"%s\").\n",
                 errno,
                 strerror(errno)
                 );
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- dsp_suspend() */
int S_CALL dsp_suspend(void) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_PWR_OFF);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_suspend: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      if(LOG_DEBUG_LEVEL(2))
      {
         log_printf(LOG_WARN "dsp_suspend: ioctl() failed. errno=%d (\"%s\").\n",
                    errno,
                    strerror(errno)
                    );
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_power_off() */
int dsp_power_off(void) {
   /* deprecated */

   return dsp_suspend();
}


/*--------------------------------------------------------------------------- dsp_resume() */
int S_CALL dsp_resume(void) {
   int ret;

   ret = ioctl(c64_fd, C64_IOCTL_RESUME);

   if(0 == ret)
   {
      /* Succeded */
      if(LOG_DEBUG_LEVEL(10))
      {
         log_printf(LOG_INFO "dsp_resume: ioctl() succeeded.\n");
      }

   }
   else
   {
      /* Failed: ioctl() */
      if(EBUSY != errno)
      {
         log_printf(LOG_ERROR "dsp_resume: ioctl() failed. errno=%d (\"%s\").\n",
                    errno,
                    strerror(errno)
                    );
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_set_enable_skip_dsp_open() */
void dsp_set_enable_skip_dsp_open(sBool _bEnabled) {
   b_skip_dsp_open = _bEnabled;
}


/*--------------------------------------------------------------------------- dsp_init_root() */
dsp_error_t dsp_init_root(void) {
   dsp_error_t ret;

   /* Initialize OS abstraction layer */
   ret = osal_init(S_TRUE);

   if(DSP_ERR_OK == ret)
   {
      if(!b_skip_dsp_open)
      {
         ret = dsp_open();
      }
      else
      {
         ret = 0;
      }

      if(0 == ret)
      {
#ifdef USE_FAST_MMAP
         /* Init fast mmap */
         sUI mapIdx;

         b_dsp_mmap_force_real = S_TRUE;

         for(mapIdx=0; mapIdx < NUM_FAST_MMAP_ENTRIES; mapIdx++)
         {
            fast_mmap[mapIdx].virt_addr = 0;
         }

         fast_mmap[0].phys_addr = dsp_config.reset_vector.phys_addr;
         fast_mmap[0].size      = (dsp_config.ram.phys_addr + dsp_config.ram.size) - dsp_config.reset_vector.phys_addr;

         for(mapIdx=0; mapIdx < NUM_FAST_MMAP_ENTRIES; mapIdx++)
         {
            ret = dsp_mmap(fast_mmap[mapIdx].phys_addr, fast_mmap[mapIdx].size, &fast_mmap[mapIdx].virt_addr);

            if(DSP_ERR_OK == ret)
            {
               if(log_debug_level >= 10)
               {
                  log_printf(LOG_INFO "dsp_init: initialized fast_mmap[%u] physAddr=0x%08x size=0x%08x\n",
                             mapIdx,
                             fast_mmap[mapIdx].phys_addr,
                             fast_mmap[mapIdx].size
                             );
               }
            }
            else
            {
               log_printf(LOG_ERROR "dsp_init: failed to mmap() fast_mmap region phys_addr=0x%08x size=0x%08x\n",
                          fast_mmap[mapIdx].phys_addr,
                          fast_mmap[mapIdx].size
                          );
               break;
            }
            /* Next entry */
         }

         b_dsp_mmap_force_real = S_FALSE;
#endif /* USE_FAST_MMAP */
      }
      else
      {
         /* Failed: dsp_open() */
      }
   }
   else
   {
      /* Failed: osal_init() */
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_exit_root() */
void dsp_exit_root(void) {

#ifdef USE_FAST_MMAP
   /* Teardown fast mmap */
   {
      sUI mapIdx;
      dsp_error_t err;
      b_dsp_mmap_force_real = S_TRUE;
      
      for(mapIdx=0; mapIdx < NUM_FAST_MMAP_ENTRIES; mapIdx++)
      {
         if(0 != fast_mmap[mapIdx].virt_addr)
         {
            err = dsp_munmap(fast_mmap[mapIdx].virt_addr, fast_mmap[mapIdx].size);
            
            if(DSP_ERR_OK == err)
            {
               if(log_debug_level >= 10)
               {
                  log_printf(LOG_INFO "dsp_exit: de-initialized fast_mmap[%u] physAddr=0x%08x virtAddr=0x%08x size=0x%08x\n",
                             mapIdx,
                             fast_mmap[mapIdx].phys_addr,
                             fast_mmap[mapIdx].virt_addr,
                             fast_mmap[mapIdx].size
                             );
               }

            }
            else
            {
               log_printf(LOG_ERROR "dsp_exit: failed to munmap() fast_mmap region virt_addr=0x%08x size=0x%08x\n",
                          fast_mmap[mapIdx].virt_addr,
                          fast_mmap[mapIdx].size
                          );
               break;
            }
         }

         /* Next entry */
      }
   }
#endif /* USE_FAST_MMAP */

   /* Close /dev/c64 connection */
   if(!b_skip_dsp_open)
   {
      dsp_close();
   }

   /* Shutdown OS abstraction layer */
   osal_exit();
}


/*--------------------------------------------------------------------------- dsp_mmap() */
dsp_error_t dsp_mmap(sUI _physAddr, sUI _numBytes, sUI *_retAddr) {
   dsp_error_t ret;

#ifdef USE_FAST_MMAP
   if(!b_dsp_mmap_force_real)
   {
      sUI mapIdx;

      for(mapIdx=0; mapIdx < NUM_FAST_MMAP_ENTRIES; mapIdx++)
      {
         if( (_physAddr >= fast_mmap[mapIdx].phys_addr) &&
             (_physAddr <  (fast_mmap[mapIdx].phys_addr + fast_mmap[mapIdx].size))
             )
         {
            *_retAddr = (_physAddr - fast_mmap[mapIdx].phys_addr) + fast_mmap[mapIdx].virt_addr;

            if(LOG_DEBUG_LEVEL(30))
            {
               log_printf(LOG_TRACE "**dsp_mmap(physAddr=0x%08x numBytes=0x%08x (%u))\n",
                          _physAddr,
                          _numBytes,
                          _numBytes
                          );
            }

            return DSP_ERR_OK;
         }
      }

      log_printf(LOG_WARN "dsp_mmap: fallback to mmap() physAddr=0x%08x numBytes=0x%08x\n",
                 _physAddr,
                 _numBytes
                 );
   }
#endif /* USE_FAST_MMAP */

   if(LOG_DEBUG_LEVEL(30))
   {
      log_printf(LOG_TRACE "dsp_mmap(physAddr=0x%08x numBytes=0x%08x (%u))\n",
                 _physAddr,
                 _numBytes,
                 _numBytes
                 );
   }

   if(NULL != _retAddr)
   {
      
      union {
         void *ptr;
         sUI   u32;
      } virtAddr;

      sU32 physAddrAlign = DSP_PAGESIZE_ALIGN(_physAddr);
      sU32 numBytesRound = DSP_PAGESIZE_ROUND(_numBytes);

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_mmap: physAddrAlign=0x%08x numBytesRound=0x%08x (%u)\n",
                    physAddrAlign,
                    numBytesRound,
                    numBytesRound
                    );
      }
 

      virtAddr.ptr = osal_mmap((void*)physAddrAlign, (sSize)numBytesRound);

      if(NULL != virtAddr.ptr)
      {
         /* Succeeded */
         virtAddr.u32 = (_physAddr - physAddrAlign) + virtAddr.u32;

         ret = DSP_ERR_OK;
      }
      else
      {
         /* Failed: mmap() */
         log_puts(LOG_ERROR "dsp_mmap: osal_mmap() failed.\n");

         ret = DSP_ERR_MEM_MAP;
      }

      *_retAddr = virtAddr.u32;

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_DEBUG "dsp_mmap: virtAddr=0x%08x\n", virtAddr.u32);
      }
   }
   else
   {
      /* Failed: retAddr is NULL */
      ret = DSP_ERR_MEM;
   }

   if(LOG_DEBUG_LEVEL(30))
   {
      log_printf(LOG_TRACE "dsp_mmap: ret=%d\n", ret);
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- dsp_munmap() */
dsp_error_t dsp_munmap(sUI _virtAddr, sUI _numBytes) {
   dsp_error_t ret;

#ifdef USE_FAST_MMAP
   if(!b_dsp_mmap_force_real)
   {
      sUI mapIdx;

      for(mapIdx=0; mapIdx < NUM_FAST_MMAP_ENTRIES; mapIdx++)
      {
         if( (_virtAddr >= fast_mmap[mapIdx].virt_addr) &&
             (_virtAddr <  (fast_mmap[mapIdx].virt_addr + fast_mmap[mapIdx].size))
             )
         {
            if(LOG_DEBUG_LEVEL(30))
            {
               log_printf(LOG_TRACE "**dsp_munmap: virtAddr=0x%08x numBytes=0x%08x (%u)\n",
                          _virtAddr,
                          _numBytes,
                          _numBytes
                          );
            }

            return DSP_ERR_OK;
         }
      }

      log_printf(LOG_WARN "dsp_munmap: fallback to munmap()\n");
   }
#endif /* USE_FAST_MMAP */

   if(LOG_DEBUG_LEVEL(30))
   {
      log_printf(LOG_TRACE "dsp_munmap: virtAddr=0x%08x numBytes=0x%08x (%u)\n",
                 _virtAddr,
                 _numBytes,
                 _numBytes
                 );
   }

   {
      sUI virtAddrAlign = DSP_PAGESIZE_ALIGN(_virtAddr);
      sUI numBytesRound = DSP_PAGESIZE_ROUND(_numBytes);

      if(LOG_DEBUG_LEVEL(30))      
      {
         log_printf(LOG_TRACE "dsp_munmap: virtAddrAlign=0x%08x numBytesRound=0x%08x (%u)\n",
                    virtAddrAlign,
                    numBytesRound,
                    numBytesRound
                    );
      }

      ret = osal_munmap((void*)virtAddrAlign, numBytesRound);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_reg_write_32() */
dsp_error_t dsp_reg_write_32(sUI _physAddr, sUI _value) {
   dsp_error_t ret;
   sUI virtAddr;

   ret = dsp_mmap(_physAddr, 4, &virtAddr);

   if(DSP_ERR_OK == ret)
   {
      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_reg_write_32: physAddr=0x%08x oldVal=0x%08x newVal=0x%08x\n",
                    _physAddr,
                    *((volatile sU32*)virtAddr),
                    _value
                    );
      }

      *((volatile sU32*)virtAddr) = _value;

      /* Succeeded (well, almost) */
      ret = dsp_munmap(virtAddr, 4);
   }
   else
   {
      log_printf(LOG_ERROR "dsp_reg_write_32: dsp_mmap() failed (ret=0x%08x).\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_reg_read_32() */
dsp_error_t dsp_reg_read_32(sUI _physAddr, sUI *_retValue) {
   dsp_error_t ret;
   sUI virtAddr;

   if(NULL != _retValue)
   {
      ret = dsp_mmap(_physAddr, 4, &virtAddr);
      
      if(DSP_ERR_OK == ret)
      {
         *_retValue = *((volatile sU32*)virtAddr);

         if(LOG_DEBUG_LEVEL(30))
         {
            log_printf(LOG_TRACE "dsp_reg_read_32: physAddr=0x%08x retVal=0x%08x\n",
                       _physAddr, *((sUI*)virtAddr)
                       );
         }
         
         /* Succeeded (well, almost) */
         ret = dsp_munmap(virtAddr, 4);
      }
      else
      {
         log_printf(LOG_ERROR "dsp_reg_read_32: dsp_mmap() failed (ret=0x%08x).\n", ret);

         ret = DSP_ERR_MEM;
      }
   }
   else
   {
      log_printf(LOG_ERROR "dsp_reg_read_32: _retValue is NULL!!\n");

      ret = DSP_ERR_MEM;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_reg_bit_set() */
dsp_error_t dsp_reg_bit_set(sUI _physAddr, sUI _bitNr) {
   dsp_error_t ret;
   sUI virtAddr;

   ret = dsp_mmap(_physAddr, 4, &virtAddr);

   if(DSP_ERR_OK == ret)
   {
      sU32 val = *((volatile sU32*)virtAddr);

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_reg_bit_set: physAddr=0x%08x bitNr=%u oldVal=0x%08x\n",
                    _physAddr, _bitNr, val
                    );
      }

      val |= (1 << _bitNr);

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_reg_bit_set: physAddr=0x%08x bitNr=%u newVal=0x%08x\n",
                    _physAddr, _bitNr, val
                    );
      }

      *((volatile sU32*)virtAddr) = val;

      /* Succeeded (well, almost) */
      ret = dsp_munmap(virtAddr, 4);
   }
   else
   {
      log_printf(LOG_ERROR "dsp_reg_bit_set: dsp_mmap() failed (ret=0x%08x).\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_reg_bit_clear() */
dsp_error_t dsp_reg_bit_clear(sUI _physAddr, sUI _bitNr) {
   dsp_error_t ret;
   sUI virtAddr;

   ret = dsp_mmap(_physAddr, 4, &virtAddr);

   if(DSP_ERR_OK == ret)
   {
      sU32 val = *((volatile sU32*)virtAddr);

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_reg_bit_clear: physAddr=0x%08x bitNr=%u oldVal=0x%08x\n",
                    _physAddr, _bitNr, val
                    );
      }

      val &= ~(1 << _bitNr);

      *((volatile sU32*)virtAddr) = val;

      if(LOG_DEBUG_LEVEL(30))
      {
         log_printf(LOG_TRACE "dsp_reg_bit_clear: physAddr=0x%08x bitNr=%u newVal=0x%08x\n",
                    _physAddr, _bitNr, val
                    );
      }

      /* Succeeded (well, almost) */
      ret = dsp_munmap(virtAddr, 4);
   }
   else
   {
      log_printf(LOG_ERROR "dsp_reg_bit_clear: dsp_mmap() failed (ret=0x%08x).\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_copy_to() */
dsp_error_t dsp_copy_to(sUI _physAddr, void *_srcBuf, sUI _numBytes) {
   dsp_error_t ret;
   sUI virtAddr;

   if(NULL != _srcBuf)
   {
      ret = dsp_mmap(_physAddr, _numBytes, &virtAddr);
      
      if(DSP_ERR_OK == ret)
      {
         memcpy((void*)virtAddr, _srcBuf, (size_t)_numBytes);

#if 0
         sUI i;
         sU8 *d = (sU8*)virtAddr;
         sU8 *s = (sU8*)_srcBuf;

         for(i=0; i<_numBytes; i++)
         {
            sU8 sv = s[i];
            d[i] = sv;
         }
#endif
         
         /* Succeeded (well, almost) */
         ret = dsp_munmap(virtAddr, _numBytes);
      }
      else
      {
         log_printf(LOG_ERROR "dsp_reg_read_32: dsp_mmap() failed (ret=0x%08x).\n", ret);
      }
   }
   else
   {
      log_printf(LOG_ERROR "dsp_reg_read_32: _retValue is NULL!!\n");

      ret = DSP_ERR_MEM;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_memset() */
dsp_error_t dsp_memset(sUI _physAddr, sU8 _c, sUI _numBytes) {
   dsp_error_t ret;
   sUI virtAddr;

   ret = dsp_mmap(_physAddr, _numBytes, &virtAddr);
 
   if(DSP_ERR_OK == ret)
   {
      memset((void*)virtAddr, _c, (size_t)_numBytes);
      
      /* Succeeded (well, almost) */
      ret = dsp_munmap(virtAddr, _numBytes);
   }
   else
   {
      log_printf(LOG_ERROR "dsp_memset: dsp_mmap() failed (ret=0x%08x).\n", ret);
   }
   
   return ret;
}
