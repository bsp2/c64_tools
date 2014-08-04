/* ----
 * ---- file   : demo_calc_fastcall.h
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
 * ---- changed: 19Sep2013
 * ----
 * ----
 */

#ifndef __COMPONENT_DEMO_CALC_FASTCALL_H__
#define __COMPONENT_DEMO_CALC_FASTCALL_H__


#define COMPONENT_NAME_DEMO_CALC_FASTCALL  "demo_calc_fastcall"


enum {
   DEMO_CALC_FASTCALL_INITIATE = 1,

   NUM_DEMO_CALC_FASTCALL_COMMANDS
};


enum {
   DEMO_CALC_FASTCALL_FC_CMD_ADD = DSP_COMPONENT_FC_CMD_DECLARE(1),
   DEMO_CALC_FASTCALL_FC_CMD_SUB = DSP_COMPONENT_FC_CMD_DECLARE(2),
   DEMO_CALC_FASTCALL_FC_CMD_MUL = DSP_COMPONENT_FC_CMD_DECLARE(3),
   DEMO_CALC_FASTCALL_FC_CMD_DIV = DSP_COMPONENT_FC_CMD_DECLARE(4),

   NUM_DEMO_CALC_FASTCALL_FC_CMDS
};


enum {
   DEMO_CALC_FASTCALL_ERR_OK        = 0,
   DEMO_CALC_FASTCALL_ERR_ILLCMD    = 1,
   DEMO_CALC_FASTCALL_ERR_DIVBYZERO = 2,

   NUM_DEMO_CALC_FASTCALL_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_demo_calc_fastcall;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_DEMO_CALC_FASTCALL_H__ */
