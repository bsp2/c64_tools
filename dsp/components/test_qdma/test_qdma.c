/* ----
 * ---- file   : test_qdma.c
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
 * ---- changed: 23Oct2013, 11Dec2013
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "test_qdma.h"


/* --------------------------------------------------------------------------- loc_copy1d() */
static void loc_copy1d(test_qdma_cmd_copy1d_args_t *_args) {

   syscalls.qdma_init();

   syscalls.qdma_copy1d(QDMA_0,
                        (void*)_args->dst_phys_addr,
                        (void*)_args->src_phys_addr,
                        _args->num_bytes
                        );

   syscalls.qdma_wait(QDMA_0);
}


/* --------------------------------------------------------------------------- loc_copy2d() */
static void loc_copy2d(test_qdma_cmd_copy2d_args_t *_args) {

   syscalls.qdma_init();

   syscalls.qdma_copy2d(QDMA_0,
                        (void*)_args->dst_phys_addr,
                        _args->dst_pitch,
                        (void*)_args->src_phys_addr,
                        _args->src_pitch,
                        _args->num_bytes_x,
                        _args->num_y
                        );

   syscalls.qdma_wait(QDMA_0);
}


/* --------------------------------------------------------------------------- loc_link1d() */
static void loc_link1d(test_qdma_cmd_link1d_args_t *_args) {

   syscalls.qdma_init();

   {
      sUI i;
      test_qdma_cmd_copy1d_args_t *xfer;
            
      /* Setup linked transfers / PaRAM entries */
      for(i=(_args->num_links - 1); i>0; i--)
      {
         xfer = &_args->transfers[i];

         syscalls.qdma_link1d(QDMA_1,
                              (8 + i - 1),
                              (1 == i) ? 0xFFFFu : (8 + i - 2), /* last transfer is linked to none (0xFFFF) */
                              (void*)xfer->dst_phys_addr,
                              (void*)xfer->src_phys_addr,
                              xfer->num_bytes
                              );
      }

      /* Setup first transfer and start DMA by writing to the
       *  PaRAM entry that is mapped to QDMA0 
       */
      xfer = &_args->transfers[0];

      syscalls.qdma_link1d(QDMA_1,
                           QDMA_1,
                           (8 + (_args->num_links - 1) - 1),
                           (void*)xfer->dst_phys_addr,
                           (void*)xfer->src_phys_addr,
                           xfer->num_bytes
                           );
   }

   syscalls.qdma_wait(QDMA_1);
}


/* --------------------------------------------------------------------------- loc_link2d() */
static void loc_link2d(test_qdma_cmd_link2d_args_t *_args) {

   syscalls.qdma_init();

   {
      sUI i;
      test_qdma_cmd_copy2d_args_t *xfer;
            
      /* Setup linked transfers / PaRAM entries */
      for(i=(_args->num_links - 1); i>0; i--)
      {
         xfer = &_args->transfers[i];

         syscalls.qdma_link2d(QDMA_1,
                              (8 + i - 1),
                              (1 == i) ? 0xFFFFu : (8 + i - 2), /* last transfer is linked to none (0xFFFF) */
                              (void*)xfer->dst_phys_addr,
                              xfer->dst_pitch,
                              (void*)xfer->src_phys_addr,
                              xfer->src_pitch,
                              xfer->num_bytes_x,
                              xfer->num_y
                              );
      }

      /* Setup first transfer and start DMA by writing to the
       *  PaRAM entry that is mapped to QDMA0 
       */
      xfer = &_args->transfers[0];

      syscalls.qdma_link2d(QDMA_1,
                           QDMA_1,
                           (8 + (_args->num_links - 1) - 1),
                           (void*)xfer->dst_phys_addr,
                           xfer->dst_pitch,
                           (void*)xfer->src_phys_addr,
                           xfer->src_pitch,
                           xfer->num_bytes_x,
                           xfer->num_y
                           );
   }

   syscalls.qdma_wait(QDMA_1);
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
         ret = TEST_QDMA_ERR_ILLCMD;
         break;

      case TEST_QDMA_CMD_COPY1D:

         loc_copy1d((test_qdma_cmd_copy1d_args_t*)_arg1);

         /* Succeeded */
         *_ret1 = ret = TEST_QDMA_ERR_OK;
         break;

      case TEST_QDMA_CMD_COPY2D:

         loc_copy2d((test_qdma_cmd_copy2d_args_t*)_arg1);

         /* Succeeded */
         *_ret1 = ret = TEST_QDMA_ERR_OK;
         break;

      case TEST_QDMA_CMD_LINK1D:

         loc_link1d((test_qdma_cmd_link1d_args_t*)_arg1);

         /* Succeeded */
         *_ret1 = ret = TEST_QDMA_ERR_OK;
         break;

      case TEST_QDMA_CMD_LINK2D:

         loc_link2d((test_qdma_cmd_link2d_args_t*)_arg1);

         /* Succeeded */
         *_ret1 = ret = TEST_QDMA_ERR_OK;
         break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- component_test_qdma */
#pragma DATA_SECTION(component_test_qdma,  ".sec_com");
dsp_component_t component_test_qdma = {

   /* fxns: */
   {
      NULL,       /* init() */
      &loc_exec,
      NULL,       /* exec fastcall RPC */
      NULL        /* exit() */
   },
   
   COMPONENT_NAME_TEST_QDMA,
};


DSP_COMPONENT_MAIN
