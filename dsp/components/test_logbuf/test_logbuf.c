/* ----
 * ---- file   : test_logbuf.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel.
 * ----          Distributed under terms of the MIT LICENSE (MIT).
 * ----
 * ---- Permission is hereby granted, free of charge, to any person obtaining a copy
 * ---- of this software and associated documentation files (the "Software"), to deal
 * ---- in the Software without restriction, including without limitation the rights
 * ---- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * ---- copies of the Software, and to permit persons to whom the Software is
 * ---- furnished to do so, subject to the following conditions:
 * ----
 * ---- The above copyright notice and this permission notice shall be included in
 * ---- all copies or substantial portions of the Software.
 * ----
 * ---- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * ---- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * ---- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * ---- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * ---- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * ---- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * ---- THE SOFTWARE.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 21Oct2013, 22Oct2013
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "test_logbuf.h"


int cnt_i = 42;
float cnt_f = 3.1415f;


/* --------------------------------------------------------------------------- loc_mylog_printf() */
static int loc_mylog_printf(const char *_fmt, ...) {
   int ret;
   va_list l;

   va_start(l, _fmt);

   ret = syscalls.vprintf(_fmt, l);

   va_end(l);

   return ret;
}


/* --------------------------------------------------------------------------- init */
static sBool loc_init(void) {

   //syscalls.mlb_debug_usr(3, *(volatile sS32*) 0x8620189c); //cnt_i
   //syscalls.mlb_debug_usr(3, *(volatile sS32*) &cnt_i); //cnt_i
   syscalls.mlb_debug_usr(3, (sU32) &cnt_i); // --> wrong address (in AREA1 instead AREA2)

#if 0
   /* (note) test linker error when using e.g. printf instead of syscalls.printf (works) */
   printf("hello, world.\n");
#endif

#if 0
/*  
 *   (note) just a test (works), DSP code should not use malloc/free, at least not
 *           across multiple RPCs
 */
   void *addr = malloc(42);
   syscalls.mlb_debug_usr(3, (sU32)addr);
   free(addr);
#endif

   return S_TRUE;
}


/* --------------------------------------------------------------------------- exec */
static sU32 loc_exec(dsp_component_cmd_t _cmd,
                     sU32  _arg1, sU32  _arg2,
                     sU32 *_ret1, sU32 *_ret2
                     ) {
   sU32 ret;

   switch(_cmd)
   {
      default:
         /* Failed: illegal command id */
         ret = TEST_LOGBUF_ERR_ILLCMD;
         break;

      case TEST_LOGBUF_CMD_PUTS:
         (void) syscalls.puts("hello, DSP world. (TEST_LOGBUF_PUTS called)\n");

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_PRINTF:

         (void) syscalls.printf("cnt_i=%d cnt_f=%f (TEST_LOGBUF_PRINTF called)\n",
                                cnt_i,
                                cnt_f
                                );

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_VPRINTF:
         (void) loc_mylog_printf("mylog: cnt_i=%d cnt_f=%f (TEST_LOGBUF_VPRINTF called)\n",
                                 cnt_i,
                                 cnt_f
                                 );

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_INC_CNT_I:
         cnt_i += *(sS32*)&_arg1;

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_INC_CNT_F:
         cnt_f += *(sF32*)&_arg1;

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_PRINTF_SPLIT:
         syscalls.printf("this is the beginning");
         syscalls.printf(", and this is the end.\n");

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

      case TEST_LOGBUF_CMD_PRINTF_NO_LF:
         syscalls.printf("message w/o linefeed");

         /* Succeeded */
         *_ret1 = ret = TEST_LOGBUF_ERR_OK;
         break;

   }

   return ret;
}


/* --------------------------------------------------------------------------- component_test_logbuf */
#pragma DATA_SECTION(component_test_logbuf,  ".sec_com");
dsp_component_t component_test_logbuf = {

   /* fxns: */
   {
      &loc_init,  /* init() */
      &loc_exec,
      NULL,       /* exec fastcall RPC */
      NULL        /* exit() */
   },
   
   COMPONENT_NAME_TEST_LOGBUF,
};


DSP_COMPONENT_MAIN
