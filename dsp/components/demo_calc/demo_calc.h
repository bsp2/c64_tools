/* ----
 * ---- file   : demo_calc.h
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
 * ---- changed: 14Sep2013, 02Oct2013
 * ----
 * ----
 */

#ifndef __COMPONENT_DEMO_CALC_H__
#define __COMPONENT_DEMO_CALC_H__


#define COMPONENT_NAME_DEMO_CALC  "demo_calc"


enum {
   DEMO_CALC_CMD_ADD = 1,
   DEMO_CALC_CMD_SUB = 2,
   DEMO_CALC_CMD_MUL = 3,
   DEMO_CALC_CMD_DIV = 4,

   NUM_DEMO_CALC_COMMANDS
};


enum {
   DEMO_CALC_ERR_OK        = 0,
   DEMO_CALC_ERR_ILLCMD    = 1,
   DEMO_CALC_ERR_DIVBYZERO = 2,

   NUM_DEMO_CALC_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_demo_calc;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_DEMO_CALC_H__ */
