/* ----
 * ---- file   : demo_calc.c
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
 * ---- changed: 14Sep2013, 18Sep2013, 19Sep2013, 02Oct2013, 03Oct2013, 20Oct2013
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "demo_calc.h"


/* --------------------------------------------------------------------------- init */
static sBool loc_init(void) {
   sBool ret;

   /* Intentionally left blank */

   /* Succeeded */
   ret = S_TRUE;

   return ret;
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
         ret = DEMO_CALC_ERR_ILLCMD;
         break;

      case DEMO_CALC_CMD_ADD:
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) + DSP_ARG_S32(_arg2));

         ////syscalls.mlb_debug_usr(0, 42);

         /* Succeeded */
         ret = DEMO_CALC_ERR_OK;
         break;

      case DEMO_CALC_CMD_SUB:
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) - DSP_ARG_S32(_arg2));

         /* Succeeded */
         ret = DEMO_CALC_ERR_OK;
         break;

      case DEMO_CALC_CMD_MUL:
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) * DSP_ARG_S32(_arg2));

         /* Succeeded */
         ret = DEMO_CALC_ERR_OK;
         break;

      case DEMO_CALC_CMD_DIV:
         if(0 != _arg2)
         {
            *_ret1 = (sU32) (DSP_ARG_S32(_arg1) / DSP_ARG_S32(_arg2));

            /* Succeeded */
            ret = DEMO_CALC_ERR_OK;
         }
         else
         {
            /* Failed: trying to divide by zero */
            ret = DEMO_CALC_ERR_DIVBYZERO;
         }
         break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- exit */
static void loc_exit(void) {

   /* Intentionally left blank */
}


/* --------------------------------------------------------------------------- component_demo_calc */
#pragma DATA_SECTION(component_demo_calc,  ".sec_com");
dsp_component_t component_demo_calc = {

   /* fxns: */
   {
      &loc_init,
      &loc_exec,
      NULL,       /* exec fastcall RPC */
      &loc_exit
   },
   
   COMPONENT_NAME_DEMO_CALC,
};


DSP_COMPONENT_MAIN
