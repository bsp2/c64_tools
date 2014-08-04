/* ----
 * ---- file   : sramtest.h
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
 * ---- changed: 12Nov2013, 13Nov2013, 14Nov2013
 * ----
 * ----
 */

#ifndef __COMPONENT_SRAMTEST_H__
#define __COMPONENT_SRAMTEST_H__


#define COMPONENT_NAME_SRAMTEST  "sramtest"


enum {
   SRAMTEST_CMD_L1SRAM_R_SPEED            =  1,
   SRAMTEST_CMD_L1SRAM_W_SPEED            =  2,
   SRAMTEST_CMD_L1SRAM_RW_SPEED           =  3,
   SRAMTEST_CMD_L1SRAM_R2W2_SPEED         =  4,
   SRAMTEST_CMD_L1SRAM_R4W4_SPEED         =  5,
   SRAMTEST_CMD_L1SRAM_R6W6_SPEED         =  6,
   SRAMTEST_CMD_L1SRAM_R8W8_SPEED         =  7,
   SRAMTEST_CMD_L1SRAM_RWRAM_SPEED        =  8,
   SRAMTEST_CMD_L1SRAM_RWRAMDMA_SPEED     =  9,

   SRAMTEST_CMD_L2SRAM_R_SPEED            = 10,
   SRAMTEST_CMD_L2SRAM_RW_SPEED           = 11,
   SRAMTEST_CMD_L2SRAM_R1_W2_SPEED        = 12,
   SRAMTEST_CMD_L2SRAM_R1_W2RAMDMA_SPEED  = 13,

   NUM_SRAMTEST_COMMANDS
};


enum {
   SRAMTEST_ERR_OK        = 0,
   SRAMTEST_ERR_ILLCMD    = 1,

   NUM_SRAMTEST_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_sramtest;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_SRAMTEST_H__ */
