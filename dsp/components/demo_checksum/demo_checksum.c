/* ----
 * ---- file   : demo_checksum.c
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
 * ---- changed: 18Sep2013, 19Sep2013, 02Oct2013, 03Oct2013, 20Oct2013, 01Nov2013, 10Nov2013
 * ----          11Nov2013, 09Jan2014
 * ----
 * ----
 */

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>


#include "demo_checksum.h"


/* --------------------------------------------------------------------------- loc_rand() */
static sU32 seed = 0x44894489u;

static sU32 loc_rand_get(void) {
   seed *= 16807u;

   return seed;
}


/* --------------------------------------------------------------------------- loc_add() */
static sU32 loc_add(const sU32 *_addr, const sU32 _numBytes) {
   sUI ret = 0;
   sUI i;

   /* Invalidate DSP caches for the memory area */
   syscalls.cache_inv((void*)_addr, _numBytes, S_TRUE);

   for(i=0; i<(_numBytes >> 2); i++)
   {
      ret += _addr[i];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_rand_write() */
static sU32 loc_rand_write(sU32 *_addr, const sU32 _numDWords) {
   sUI i;
   sU32 ret = 0;

   for(i=0; i<_numDWords; i++)
   {
      sU32 c = loc_rand_get();
      _addr[i] = c;

      ret = (ret ^ c);
   }

   syscalls.cache_wb(_addr, _numDWords * sizeof(sU32), S_TRUE /*wait*/);

   return ret;
}


/* --------------------------------------------------------------------------- loc_rand_write_nowb() */
static sU32 loc_rand_write_nowb(sU32 *_addr, const sU32 _numDWords) {
   sUI i;
   sU32 ret = 0;

   for(i=0; i<_numDWords; i++)
   {
      sU32 c = loc_rand_get();
      _addr[i] = c;

      ret = (ret ^ c);
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_rand_read() */
static sU32 loc_rand_read(sU32 *_addr, const sU32 _numDWords) {
   sUI i;
   sU32 ret = 0;

   syscalls.cache_inv(_addr, _numDWords * sizeof(sU32), S_TRUE /*wait*/);

   for(i=0; i<_numDWords; i++)
   {
      sU32 c = _addr[i];

      ret = (ret ^ c);
   }

   syscalls.mlb_debug_usr(0, ret);
   ////syscalls->mlb_debug_usr(0, 0x87654321u);

   return ret;
}


/* --------------------------------------------------------------------------- loc_rand_read_noinv() */
static sU32 loc_rand_read_noinv(sU32 *_addr, const sU32 _numDWords) {
   sUI i;
   sU32 ret = 0;

   for(i=0; i<_numDWords; i++)
   {
      sU32 c = _addr[i];

      ret = (ret ^ c);
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_avg32() */
static sU32 loc_avg32(const sU32 *_addr, const sU32 _numDWords) {
   sUI ret = 0;
   sUI i;

   /* Invalidate DSP caches for the memory area */
   syscalls.cache_inv((void*)_addr, (_numDWords << 2), S_TRUE);
   
   for(i=0; i<_numDWords; i++)
   {
      ret = (ret >> 1) + (_addr[i] >> 1);
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_add32_dma() */
static sU32 loc_add32_dma(demo_checksum_cmd_add32_dma_args_t *_args) {
   sUI ret = 0;
   sUI i;
#define ADD32_SCRATCH_SIZE       (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP / 2)
#define ADD32_SCRATCH_NUMDWORDS  (ADD32_SCRATCH_SIZE >> 2)

#define ADD32_SCRATCH1_ADDR  (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u)
#define ADD32_SCRATCH2_ADDR  (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + ADD32_SCRATCH_SIZE)

   if(_args->num_dwords_per_iteration <= ADD32_SCRATCH_NUMDWORDS)
   {
      sU32 *workSrc     = (void*)ADD32_SCRATCH1_ADDR;
      sU32 *workSrcNext = (void*)ADD32_SCRATCH2_ADDR;
      sU8 *srcAddr = (sU8*)_args->phys_addr_src;
      sU32 *dst = (sU32*)_args->phys_addr_dst;

      sU32 w4 = (_args->num_dwords_per_iteration << 2);

      sUI iter;

      syscalls.qdma_init();

      /* Stream first working set into SRAM */
      syscalls.qdma_copy2d(QDMA_0,
                           workSrc,
                           w4,
                           srcAddr,
                           0,
                           w4,
                           1
                           );
      
      for(iter=0; iter<_args->num_iterations; iter++)
      {
         syscalls.qdma_wait(QDMA_0);

         /* Stream next working set into SRAM */
         if(iter != (_args->num_iterations - 1))
         {
            syscalls.qdma_copy2d(QDMA_0,
                                 workSrcNext,
                                 w4,
                                 srcAddr + w4,
                                 0,
                                 w4,
                                 1
                                 );
         }

         /* Calc 32bit average */
         {
#if 0
            sU32 avg = 0;

            for(i=0; i<_args->num_dwords_per_iteration; i++)
            {
               avg = (avg >> 1) + (workSrc[i] >> 1);
            }

            /* Store result in destination buffer */
            dst[iter] = avg;
#else
            sU32 add = 0;

            for(i=0; i<_args->num_dwords_per_iteration; i++)
            {
               add += workSrc[i];
            }

            /* Store result in destination buffer */
            dst[iter] = add;
#endif
         }

         /* Swap ping-pong buffers */
         {
            sU32 *t = workSrc;

            workSrc = workSrcNext;
            workSrcNext = t;
         }

         srcAddr += w4;
      }

      /* Write back caches so GPP can see the result */
      syscalls.cache_wbInvAll();
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_addskip32_dma() */
static sU32 loc_addskip32_dma(demo_checksum_cmd_addskip32_dma_args_t *_args) {
   sUI ret = 0;
#define AS32_SCRATCH_SIZE       (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP / 2)

#define AS32_SCRATCH1_ADDR  (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u)
#define AS32_SCRATCH2_ADDR  (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + AS32_SCRATCH_SIZE)

#if 0
   sU32 phys_addr_src;
   sU32 num_bytes_per_block;      // e.g. 64
   sU32 adv_bytes_per_block;      // e.g. 8192
   sU32 num_blocks_per_iteration; // (num_bytes_per_block*num_blocks_per_iteration) must fit into L1D
   sU32 num_iterations;           // e.g. 512
   sU32 phys_addr_dst;            // increased by 4*num_blocks_per_iteration 
#endif

   if((_args->num_bytes_per_block * _args->num_blocks_per_iteration) <= AS32_SCRATCH_SIZE)
   {
      sU32 *workSrc     = (void*)AS32_SCRATCH1_ADDR;
      sU32 *workSrcNext = (void*)AS32_SCRATCH2_ADDR;
      sU8 *srcAddr = (sU8*)_args->phys_addr_src;
      sU32 *dst = (sU32*)_args->phys_addr_dst;

      sU32 advBytesPerIter  = (_args->num_blocks_per_iteration * _args->adv_bytes_per_block);
      //sU32 numDWordsPerIter = (_args->num_blocks_per_iteration * _args->num_bytes_per_block) >> 2;

      sUI iter;
      sU32 dstIdx=0;

      syscalls.qdma_init();

      /* Stream first working set into SRAM */
      syscalls.qdma_copy2d(QDMA_0,
                           workSrc,
                           _args->num_bytes_per_block,
                           srcAddr,
                           _args->adv_bytes_per_block,
                           _args->num_bytes_per_block,
                           _args->num_blocks_per_iteration
                           );
      
      for(iter=0; iter<_args->num_iterations; iter++)
      {
         syscalls.qdma_wait(QDMA_0);

         /* Stream next working set into SRAM */
         if(iter != (_args->num_iterations - 1))
         {
            syscalls.qdma_copy2d(QDMA_0,
                                 workSrcNext,
                                 _args->num_bytes_per_block,
                                 srcAddr + advBytesPerIter,
                                 _args->adv_bytes_per_block,
                                 _args->num_bytes_per_block,
                                 _args->num_blocks_per_iteration
                                 );
         }

         /* Calc checksum */
         {
            sU32 numDWordsPerBlock = (_args->num_bytes_per_block) >> 2;
            sU32 numBlocksPerIter  = (_args->num_blocks_per_iteration);
            sUI y;
            sU32 si = 0;

            if(0 != _args->b_add_to_dst)
            {
               for(y=0; y<numBlocksPerIter; y++)
               {
                  sUI x;
                  sU32 add = 0;
                  
                  for(x=0; x<numDWordsPerBlock; x++)
                  {
                     add += workSrc[si++];
                  }
                  
                  /* Add result to destination buffer */
                  dst[dstIdx++] += add;
               }
            }
            else
            {
               for(y=0; y<numBlocksPerIter; y++)
               {
                  sUI x;
                  sU32 add = 0;
                  
                  for(x=0; x<numDWordsPerBlock; x++)
                  {
                     add += workSrc[si++];
                  }
                  
                  /* Store result in destination buffer (overwrite) */
                  dst[dstIdx++] = add;
               }
            }

         }

         /* Swap ping-pong buffers */
         {
            sU32 *t = workSrc;

            workSrc = workSrcNext;
            workSrcNext = t;
         }

         srcAddr += advBytesPerIter;
      }

      /* Write back caches so GPP can see the result */
      syscalls.cache_wbInvAll();
   }

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
         ret = DEMO_CHECKSUM_ERR_ILLCMD;
         break;

      case DEMO_CHECKSUM_CMD_ADD:
         *_ret1 = loc_add((sU32*)_arg1,   /* phys_addr start */
                                 _arg2    /* num bytes */
                          );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_RAND_WRITE:
         *_ret1 = loc_rand_write((sU32*)_arg1,   /* phys_addr start */
                                        _arg2    /* num dwords */
                                 );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_RAND_READ:
         *_ret1 = loc_rand_read((sU32*)_arg1,   /* phys_addr start */
                                       _arg2    /* num dwords */
                                );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_AVG32:
         *_ret1 = loc_avg32((sU32*)_arg1,   /* phys_addr start */
                                   _arg2    /* num dwords */
                            );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_ADD32_DMA:
         *_ret1 = loc_add32_dma((demo_checksum_cmd_add32_dma_args_t *)_arg1);

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_ADDSKIP32_DMA:
         *_ret1 = loc_addskip32_dma((demo_checksum_cmd_addskip32_dma_args_t *)_arg1);

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_RAND_WRITE_NOWB:
         *_ret1 = loc_rand_write_nowb((sU32*)_arg1,   /* phys_addr start */
                                             _arg2    /* num dwords */
                                      );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_RAND_READ_NOINV:
         *_ret1 = loc_rand_read_noinv((sU32*)_arg1,   /* phys_addr start */
                                             _arg2    /* num dwords */
                                      );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DEMO_CHECKSUM_CMD_FC_INITIATE:

         syscalls.mlb_fastcall_initiate();

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
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
         ret = DEMO_CHECKSUM_ERR_ILLCMD;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CHECKSUM_FC_CMD_RAND_WRITE_NOWB):
         *_ret1 = loc_rand_write_nowb((sU32*)_arg1,   /* phys_addr start */
                                             _arg2    /* num dwords */
                                      );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;

      case DSP_COMPONENT_FC_CMD_DISPATCH(DEMO_CHECKSUM_FC_CMD_RAND_READ_NOINV):
         *_ret1 = loc_rand_read_noinv((sU32*)_arg1,   /* phys_addr start */
                                             _arg2    /* num dwords */
                                      );

         /* Succeeded */
         ret = DEMO_CHECKSUM_ERR_OK;
         break;
   }

   return ret;
}


/* --------------------------------------------------------------------------- component_checksum_calc */
#pragma DATA_SECTION(component_demo_checksum,  ".sec_com");
dsp_component_t component_demo_checksum = {

   /* fxns: */
   {
      NULL,       /* init */
      &loc_exec,
      &loc_exec_fc,
      NULL        /* exit */
   },

   COMPONENT_NAME_DEMO_CHECKSUM
};


DSP_COMPONENT_MAIN
