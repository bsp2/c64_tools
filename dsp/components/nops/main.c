/* ----
 * ---- file   : main.c
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
 * ---- changed: 25Oct2013, 14Nov2013
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "nops.h"


S_EXTERN void nops(void);


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
         ret = NOPS_ERR_ILLCMD;
         break;

      case NOPS_CMD_NOPS:
      {
         nops();
        
         *_ret1 = NOPS_ERR_OK;
      }
      break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- component_nops */
#pragma DATA_SECTION(component_nops,  ".sec_com");
dsp_component_t component_nops = {

   /* fxns: */
   {
      NULL,       /* init() */
      &loc_exec,
      NULL,       /* exec fastcall RPC */
      NULL        /* exit() */
   },
   
   COMPONENT_NAME_NOPS,
};


DSP_COMPONENT_MAIN
