/* ----
 * ---- file   : demo_calc_fastcall.c
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
 * ---- changed: 19Sep2013, 02Oct2013, 03Oct2013, 20Oct2013
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "demo_calc_fastcall.h"


/* --------------------------------------------------------------------------- exec() */
static sU32 loc_exec(dsp_component_cmd_t _cmd,
                     sU32  _arg1, sU32  _arg2,
                     sU32 *_ret1, sU32 *_ret2
                     ) {
   sU32 ret;

   switch(_cmd)
   {
      default:
         /* Failed: illegal command id */
         ret = DEMO_CALC_FASTCALL_ERR_ILLCMD;
         break;

      case DEMO_CALC_FASTCALL_INITIATE:
         syscalls.mlb_debug_usr(0, 0x900d1111);

         syscalls.mlb_fastcall_initiate();

         /* Succeeded */
         ret = 0;
         break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- exec_fc() */
static sU32 loc_exec_fc(dsp_component_cmd_t _cmd,
                        sU32  _arg1, sU32  _arg2,
                        sU32 *_ret1, sU32 *_ret2
                        ) {
   sU32 ret;

   switch(_cmd)
   {
      default:
         /* Failed: illegal command id */
         ret = DEMO_CALC_FASTCALL_ERR_ILLCMD;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CALC_FASTCALL_FC_CMD_ADD):
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) + DSP_ARG_S32(_arg2));

         /* Succeeded */
         ret = DEMO_CALC_FASTCALL_ERR_OK;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CALC_FASTCALL_FC_CMD_SUB):
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) - DSP_ARG_S32(_arg2));

         /* Succeeded */
         ret = DEMO_CALC_FASTCALL_ERR_OK;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CALC_FASTCALL_FC_CMD_MUL):
         *_ret1 = (sU32) (DSP_ARG_S32(_arg1) * DSP_ARG_S32(_arg2));

         /* Succeeded */
         ret = DEMO_CALC_FASTCALL_ERR_OK;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CALC_FASTCALL_FC_CMD_DIV):
         if(0 != _arg2)
         {
            *_ret1 = (sU32) (DSP_ARG_S32(_arg1) / DSP_ARG_S32(_arg2));

            /* Succeeded */
            ret = DEMO_CALC_FASTCALL_ERR_OK;
         }
         else
         {
            /* Failed: trying to divide by zero */
            ret = DEMO_CALC_FASTCALL_ERR_DIVBYZERO;
         }
         break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- component_demo_calc_fastcall */
#pragma DATA_SECTION(component_demo_calc_fastcall,  ".sec_com");
dsp_component_t component_demo_calc_fastcall = {

   /* fxns: */
   {
      NULL,         /* init */
      &loc_exec,
      &loc_exec_fc,
      NULL          /* exit */
   },

   COMPONENT_NAME_DEMO_CALC_FASTCALL
};


DSP_COMPONENT_MAIN
