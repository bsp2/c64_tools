/* ----
 * ---- file   : test_logbuf.h
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

#ifndef __COMPONENT_TEST_LOGBUF_H__
#define __COMPONENT_TEST_LOGBUF_H__


#define COMPONENT_NAME_TEST_LOGBUF  "test_logbuf"


enum {
   TEST_LOGBUF_CMD_PUTS         = 1,
   TEST_LOGBUF_CMD_PRINTF       = 2,
   TEST_LOGBUF_CMD_VPRINTF      = 3,
   TEST_LOGBUF_CMD_INC_CNT_I    = 4,  /* arg1=increment (int) */
   TEST_LOGBUF_CMD_INC_CNT_F    = 5,  /* arg2=increment (float) */
   TEST_LOGBUF_CMD_PRINTF_SPLIT = 6,  /* two printfs, one line */
   TEST_LOGBUF_CMD_PRINTF_NO_LF = 7,  /* no linefeed */

   NUM_TEST_LOGBUF_COMMANDS
};


enum {
   TEST_LOGBUF_ERR_OK        = 0,
   TEST_LOGBUF_ERR_ILLCMD    = 1,

   NUM_TEST_LOGBUF_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_test_logbuf;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_TEST_LOGBUF_H__ */
