/* ----
 * ---- file   : sramtest.c
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

#define SYSCALLS_C defined
#include <libc64_dsp/include/inc_overlay.h>

#include "sramtest.h"


/* --------------------------------------------------------------------------- loc_l1sram_r_speed() */
static sU32 loc_l1sram_r_speed(sU32 _numIter) {
#define L1SRAM_R_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_R_SPEED_NUMDWORDS; i++)
      {
         ret += s[i];
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_w_speed() */
static sU32 loc_l1sram_w_speed(sU32 _numIter) {
#define L1SRAM_W_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;
      sUI k = d[0];

      for(i=0; i<L1SRAM_W_SPEED_NUMDWORDS; i++)
      {
         d[i] = k++;
      }

   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_rw_speed() */
static sU32 loc_l1sram_rw_speed(sU32 _numIter) {
#define L1SRAM_RW_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 3u)
///#define L1SRAM_RW_SPEED_NUMDWORDS  (IVA2_L1DCSRAM_SIZE >> 3u)

   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + (12u*1024u));
   ///volatile sU32 *s = (sU32*) (IVA2_L1DCSRAM_BASE_DSP + 0u);
   ///volatile sU32 *d = (sU32*) (IVA2_L1DCSRAM_BASE_DSP + (8u*1024u));


   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_RW_SPEED_NUMDWORDS; i++)
      {
         sU32 c = s[i];
         ret += c;
         d[i] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_r2w2_speed() */
static sU32 loc_l1sram_r2w2_speed(sU32 _numIter) {
#define L1SRAM_R2W2_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 3u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + (12u*1024u));

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_R2W2_SPEED_NUMDWORDS; i+=2)
      {
         ret += s[i + 0];
         ret += s[i + 1];

         d[i + 0] = ret;
         d[i + 1] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_r4w4_speed() */
static sU32 loc_l1sram_r4w4_speed(sU32 _numIter) {
#define L1SRAM_R4W4_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 3u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + (12u*1024u));

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_R4W4_SPEED_NUMDWORDS; i+=4)
      {
         ret += s[i + 0];
         ret += s[i + 1];
         ret += s[i + 2];
         ret += s[i + 3];

         d[i + 0] = ret;
         d[i + 1] = ret;
         d[i + 2] = ret;
         d[i + 3] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_r6w6_speed() */
static sU32 loc_l1sram_r6w6_speed(sU32 _numIter) {
#define L1SRAM_R6W6_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 3u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + (12u*1024u));

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_R6W6_SPEED_NUMDWORDS; i+=6)
      {
         ret += s[i + 0];
         ret += s[i + 1];
         ret += s[i + 2];
         ret += s[i + 3];
         ret += s[i + 4];
         ret += s[i + 5];

         d[i + 0] = ret;
         d[i + 1] = ret;
         d[i + 2] = ret;
         d[i + 3] = ret;
         d[i + 4] = ret;
         d[i + 5] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_r8w8_speed() */
static sU32 loc_l1sram_r8w8_speed(sU32 _numIter) {
#define L1SRAM_R8W8_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 3u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + (12u*1024u));

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L1SRAM_R8W8_SPEED_NUMDWORDS; i+=8)
      {
         ret += s[i + 0];
         ret += s[i + 1];
         ret += s[i + 2];
         ret += s[i + 3];
         ret += s[i + 4];
         ret += s[i + 5];
         ret += s[i + 6];
         ret += s[i + 7];

         d[i + 0] = ret;
         d[i + 1] = ret;
         d[i + 2] = ret;
         d[i + 3] = ret;
         d[i + 4] = ret;
         d[i + 5] = ret;
         d[i + 6] = ret;
         d[i + 7] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_rwram_speed() */
static sU32 loc_l1sram_rwram_speed(sU32 _numIter, sU32 _dst) {
#define L1SRAM_RWRAM_SPEED_NUMDWORDS  (DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   sU32 *d = (sU32*) _dst;

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;
      sUI k = d[0];

      for(i=0; i<L1SRAM_RWRAM_SPEED_NUMDWORDS; i++)
      {
         d[i] = s[i] + (k++);
      }

   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l1sram_rwramdma_speed() */
static sU32 loc_l1sram_rwramdma_speed(sU32 _numIter, sU32 _dst) {
#define L1SRAM_RWRAMDMA_SPEED_NUMDWORDS  ((DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 2u) / 4)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *srcCur  = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *srcNext = srcCur  + L1SRAM_RWRAMDMA_SPEED_NUMDWORDS;
   volatile sU32 *dstCur  = srcNext + L1SRAM_RWRAMDMA_SPEED_NUMDWORDS;
   volatile sU32 *dstNext = dstCur  + L1SRAM_RWRAMDMA_SPEED_NUMDWORDS;
   sU32 *dRAM = (sU32*) _dst;

   syscalls.qdma_init();

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;
      sUI k = srcNext[0];

      for(i=0; i<L1SRAM_RWRAMDMA_SPEED_NUMDWORDS; i++)
      {
         dstCur[i] = srcCur[i] + (k++);
      }

      syscalls.qdma_wait(0);

      syscalls.qdma_copy2d(QDMA_0,
                           (void*)dstCur,
                           0,
                           dRAM,
                           0,
                           (L1SRAM_RWRAMDMA_SPEED_NUMDWORDS << 2),
                           1
                           );

      {
         volatile sU32 *vt;

         vt      = srcCur;
         srcCur  = srcNext;
         srcNext = vt;
         
         vt      = dstCur;
         dstCur  = dstNext;
         dstNext = vt;
      }

   }

   syscalls.qdma_wait(0);

   return ret;
}


/* --------------------------------------------------------------------------- loc_l2sram_r_speed() */
static sU32 loc_l2sram_r_speed(sU32 _numIter) {
#define L2SRAM_R_SPEED_NUMDWORDS  (IVA2_L2SRAM_SIZE >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (IVA2_L2SRAM_BASE_DSP + 0u);

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L2SRAM_R_SPEED_NUMDWORDS; i++)
      {
         ret += s[i];
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l2sram_rw_speed() */
static sU32 loc_l2sram_rw_speed(sU32 _numIter) {
#define L2SRAM_RW_SPEED_NUMDWORDS  (IVA2_L2SRAM_SIZE >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (IVA2_L2SRAM_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (IVA2_L2SRAM_BASE_DSP + (48u*1024u));

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L2SRAM_RW_SPEED_NUMDWORDS; i++)
      {
         sU32 c = s[i];
         ret += c;
         d[i] = ret;
      }

      s[0] = s[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l2sram_r1_w2_speed() */
static sU32 loc_l2sram_r1_w2_speed(sU32 _numIter) {
#define L2SRAM_R1_W2_SPEED_NUMDWORDS  (IVA2_L1DSRAM_SIZE >> 2u)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *s = (sU32*) (IVA2_L1DSRAM_BASE_DSP + 0u);
   volatile sU32 *d = (sU32*) (IVA2_L2SRAM_BASE_DSP  + 0u);

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;

      for(i=0; i<L2SRAM_R1_W2_SPEED_NUMDWORDS; i++)
      {
         sU32 c = s[i];
         ret += c;
         d[i] = ret;
      }

      s[0] = d[_numIter&8191];
   }

   return ret;
}


/* --------------------------------------------------------------------------- loc_l2sram_r1_w2ramdma_speed() */
static sU32 loc_l2sram_r1_w2ramdma_speed(sU32 _numIter, sU32 _dst) {
#define L2SRAM_R1_W2RAMDMA_SPEED_NUMDWORDS  ((DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP >> 2u) / 2)
   sUI iter;
   sU32 ret = 0;
   volatile sU32 *srcCur  = (sU32*) (DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP + 0u);
   volatile sU32 *srcNext = srcCur  + L2SRAM_R1_W2RAMDMA_SPEED_NUMDWORDS;
   volatile sU32 *dstCur  = (sU32*) (IVA2_L2SRAM_BASE_DSP + 0u);
   volatile sU32 *dstNext = dstCur  + L2SRAM_R1_W2RAMDMA_SPEED_NUMDWORDS;
   sU32 *dRAM = (sU32*) _dst;

   syscalls.qdma_init();

   for(iter=0; iter<_numIter; iter++)
   {
      sUI i;
      sUI k = srcNext[0];

      for(i=0; i<L2SRAM_R1_W2RAMDMA_SPEED_NUMDWORDS; i++)
      {
         dstCur[i] = srcCur[i] + (k++);
      }

      syscalls.qdma_wait(0);

      syscalls.qdma_copy2d(QDMA_0,
                           (void*)dstCur,
                           0,
                           dRAM,
                           0,
                           (L2SRAM_R1_W2RAMDMA_SPEED_NUMDWORDS << 2),
                           1
                           );

      {
         volatile sU32 *vt;

         vt      = srcCur;
         srcCur  = srcNext;
         srcNext = vt;
         
         vt      = dstCur;
         dstCur  = dstNext;
         dstNext = vt;
      }

   }

   syscalls.qdma_wait(0);

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
         ret = SRAMTEST_ERR_ILLCMD;
         break;

      case SRAMTEST_CMD_L1SRAM_R_SPEED:
      {
         *_ret1 = loc_l1sram_r_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_W_SPEED:
      {
         *_ret1 = loc_l1sram_w_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_RW_SPEED:
      {
         *_ret1 = loc_l1sram_rw_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_R2W2_SPEED:
      {
         *_ret1 = loc_l1sram_r2w2_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_R4W4_SPEED:
      {
         *_ret1 = loc_l1sram_r4w4_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_R6W6_SPEED:
      {
         *_ret1 = loc_l1sram_r6w6_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_R8W8_SPEED:
      {
         *_ret1 = loc_l1sram_r8w8_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_RWRAM_SPEED:
      {
         *_ret1 = loc_l1sram_rwram_speed(_arg1 /*numIter*/, _arg2 /* dstAddr */);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L1SRAM_RWRAMDMA_SPEED:
      {
         *_ret1 = loc_l1sram_rwramdma_speed(_arg1 /*numIter*/, _arg2 /* dstAddr */);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L2SRAM_R_SPEED:
      {
         *_ret1 = loc_l2sram_r_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L2SRAM_RW_SPEED:
      {
         *_ret1 = loc_l2sram_rw_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L2SRAM_R1_W2_SPEED:
      {
         *_ret1 = loc_l2sram_r1_w2_speed(_arg1 /*numIter*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

      case SRAMTEST_CMD_L2SRAM_R1_W2RAMDMA_SPEED:
      {
         *_ret1 = loc_l2sram_r1_w2ramdma_speed(_arg1 /*numIter*/, _arg2 /*dstAddr*/);
        
         ret = SRAMTEST_ERR_OK;
      }
      break;

   }

   return ret;
}


/* --------------------------------------------------------------------------- component_sramtest */
#pragma DATA_SECTION(component_sramtest,  ".sec_com");
dsp_component_t component_sramtest = {

   /* fxns: */
   {
      NULL,       /* init() */
      &loc_exec,
      NULL,       /* exec fastcall RPC */
      NULL        /* exit() */
   },
   
   COMPONENT_NAME_SRAMTEST,
};


DSP_COMPONENT_MAIN
