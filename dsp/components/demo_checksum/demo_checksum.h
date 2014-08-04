/* ----
 * ---- file   : demo_checksum.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel.
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
 * ---- changed: 18Sep2013, 020ct2013, 01Nov2013, 10Nov2013, 11Nov2013, 09Jan2014
 * ----
 * ----
 */

#ifndef __COMPONENT_DEMO_CHECKSUM_H__
#define __COMPONENT_DEMO_CHECKSUM_H__


#define COMPONENT_NAME_DEMO_CHECKSUM  "demo_checksum"


typedef struct {
   sU32 phys_addr_src;
   sU32 num_dwords_per_iteration;
   sU32 num_iterations;
   sU32 phys_addr_dst;
   
} demo_checksum_cmd_add32_dma_args_t;


typedef struct {
   sU32 phys_addr_src;
   sU32 num_bytes_per_block;      // e.g. 64
   sU32 adv_bytes_per_block;      // e.g. 8192
   sU32 num_blocks_per_iteration; // (num_bytes_per_block*num_blocks_per_iteration) must fit into L1D
   sU32 num_iterations;           // e.g. 512
   sU32 phys_addr_dst;            // increased by 4*num_blocks_per_iteration 
   sU32 b_add_to_dst;
   
} demo_checksum_cmd_addskip32_dma_args_t;


enum {
   /* Simply adds all 32bit dwords in the memory area specified by arg1 and arg2
       and return the checksum in ret1.
    */
   DEMO_CHECKSUM_CMD_ADD             = 1,

   /* Write <arg1> number of random dwords to addr <arg2> and return the checksum in ret1 */
   DEMO_CHECKSUM_CMD_RAND_WRITE      = 2,

   /* Read <arg1> number of random dwords from addr <arg2> and return the checksum in ret1 */
   DEMO_CHECKSUM_CMD_RAND_READ       = 3,

   /* Calc. 32bit average for given address range */
   DEMO_CHECKSUM_CMD_AVG32           = 4,

   /* Calc. 32bit average for given address range and write results to given addr. */
   DEMO_CHECKSUM_CMD_ADD32_DMA       = 5,

   /* Calc. 32bit average for given address range and write results to given addr. */
   DEMO_CHECKSUM_CMD_ADDSKIP32_DMA   = 6,

   /* Write <arg1> number of random dwords to addr <arg2> and return the checksum in ret1
    *
    *   (note) do NOT writeback caches 
    */
   DEMO_CHECKSUM_CMD_RAND_WRITE_NOWB = 7,

   /* Read <arg1> number of random dwords from addr <arg2> and return the checksum in ret1 
    *
    *   (note) do NOT invalidate caches
    */
   DEMO_CHECKSUM_CMD_RAND_READ_NOINV = 8,

   /* Initiate fastcall sequence */
   DEMO_CHECKSUM_CMD_FC_INITIATE     = 9,

   NUM_DEMO_CHECKSUM_COMMANDS
};


enum {
   DEMO_CHECKSUM_FC_CMD_RAND_WRITE_NOWB = DSP_COMPONENT_FC_CMD_DECLARE(1),
   DEMO_CHECKSUM_FC_CMD_RAND_READ_NOINV = DSP_COMPONENT_FC_CMD_DECLARE(2),

   NUM_DEMO_CHECKSUM_FC_CMDS
};


enum {
   DEMO_CHECKSUM_ERR_OK      = 0,
   DEMO_CHECKSUM_ERR_ILLCMD  = 1,

   NUM_DEMO_CHECKSUM_ERROR_CODES
};


#ifndef MLB_COMPONENT_GPP

S_EXTERN dsp_component_t component_demo_checksum;

#endif /* MLB_COMPONENT_GPP */


#endif /* __COMPONENT_DEMO_CHECKSUM_H__ */
