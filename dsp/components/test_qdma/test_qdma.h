/* ----
 * ---- file   : test_qdma.h
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

#ifndef __COMPONENT_TEST_QDMA_H__
#define __COMPONENT_TEST_QDMA_H__


#define COMPONENT_NAME_TEST_QDMA  "test_qdma"


#define TEST_QDMA_MAX_LINKS  (8)


typedef struct {
   sU32 dst_phys_addr;
   sU32 src_phys_addr;
   sU32 num_bytes;
} test_qdma_cmd_copy1d_args_t;


typedef struct {
   sU32 dst_phys_addr;
   sS32 dst_pitch;
   sU32 src_phys_addr;
   sS32 src_pitch;
   sU32 num_bytes_x;
   sU32 num_y;
} test_qdma_cmd_copy2d_args_t;


typedef struct {
   sU32 num_links;
   test_qdma_cmd_copy1d_args_t transfers[TEST_QDMA_MAX_LINKS];
} test_qdma_cmd_link1d_args_t;


typedef struct {
   sU32 num_links;
   test_qdma_cmd_copy2d_args_t transfers[TEST_QDMA_MAX_LINKS];
} test_qdma_cmd_link2d_args_t;


enum {
   TEST_QDMA_CMD_COPY1D    = 1,
   TEST_QDMA_CMD_COPY2D    = 2,
   TEST_QDMA_CMD_LINK1D    = 3,
   TEST_QDMA_CMD_LINK2D    = 4,

   NUM_TEST_QDMA_COMMANDS
};


enum {
   TEST_QDMA_ERR_OK        = 0,
   TEST_QDMA_ERR_ILLCMD    = 1,

   NUM_TEST_QDMA_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_test_qdma;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_TEST_QDMA_H__ */
