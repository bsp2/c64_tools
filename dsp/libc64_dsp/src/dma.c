/* ----
 * ---- file   : dma.c
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
 * ---- info   : This is part of the "c64_tools" package.
 * ----
 * ---- changed: 23Oct2013, 24Oct2013, 25Oct2013, 24Nov2013, 11Dec2013
 * ----
 * ----
 */

#include "../../../include/types.h"
#include "../../../include/arch/dm3730/dm3730.h"
#include "../include/dma.h"


#define EVENT_QUEUE_NR  (1)
//#define EVENT_QUEUE_NR  (0)

#define EVENT_QUEUE0_TC  (0)
#define EVENT_QUEUE1_TC  (1)


/* --------------------------------------------------------------------------- types */
typedef struct {
   /* QDMA event queue nr (0 or 1 on DM3730) */
   sUI   event_queue_nr;

   /* QDMA PaRAM nr (0..127) */
   sUI   param_nr;

   /* QDMA TCC (transfer completion code) (0..31) */
   sUI   tcc_nr;

   /* S_TRUE while QDMA transfer is in progress */
   sBool b_running;
} qdma_t;


/* --------------------------------------------------------------------------- vars */
static qdma_t qdma[NUM_QDMAS];


/* --------------------------------------------------------------------------- qdma_init() */
void qdma_init(void) {

   /* QDMA#0 configuration */
   qdma[0].event_queue_nr = EVENT_QUEUE_NR;
   qdma[0].param_nr       = 0u;
   qdma[0].tcc_nr         = 0u;

   /* QDMA#1 configuration */
   qdma[1].event_queue_nr = EVENT_QUEUE_NR;
   qdma[1].param_nr       = 1u;
   qdma[1].tcc_nr         = 1u;

   /* QDMA#2 configuration */
   qdma[2].event_queue_nr = EVENT_QUEUE_NR;
   qdma[2].param_nr       = 2u;
   qdma[2].tcc_nr         = 2u;

   /* QDMA#3 configuration */
   qdma[3].event_queue_nr = EVENT_QUEUE_NR;
   qdma[3].param_nr       = 3u;
   qdma[3].tcc_nr         = 3u;

   /* QDMA#4 configuration */
   qdma[4].event_queue_nr = EVENT_QUEUE_NR;
   qdma[4].param_nr       = 4u;
   qdma[4].tcc_nr         = 4u;

   /* QDMA#5 configuration */
   qdma[5].event_queue_nr = EVENT_QUEUE_NR;
   qdma[5].param_nr       = 5u;
   qdma[5].tcc_nr         = 5u;

   /* QDMA#6 configuration */
   qdma[6].event_queue_nr = EVENT_QUEUE_NR;
   qdma[6].param_nr       = 6u;
   qdma[6].tcc_nr         = 6u;

   /* QDMA#7 configuration */
   qdma[7].event_queue_nr = EVENT_QUEUE_NR;
   qdma[7].param_nr       = 7u;
   qdma[7].tcc_nr         = 7u;

   {
      sUI qdmaNr;

      for(qdmaNr=0; qdmaNr < NUM_QDMAS; qdmaNr++)
      {
         /* Configure QDMA PaRAM mapping. Writing to PaRAM CCNT (7) starts the transfer */
         DSP_REG(IVA2_TPCC_QCHMAP0 + (4u * qdmaNr)) = (qdma[qdmaNr].param_nr << 5u) | (7u << 2u);
         
         /* Configure QDMA event queue */
         DSP_REG(IVA2_TPCC_QDMAQNUM) = (qdma[qdmaNr].event_queue_nr << (4u * qdmaNr));
         
         /* Clear any pending interrupts for QDMA TCC */
         DSP_REG(IVA2_TPCC_ICR) = (1u << qdma[qdmaNr].tcc_nr);
         
         /* Enable QDMA event */
         DSP_REG(IVA2_TPCC_QEESR) = (1u << qdmaNr);

         qdma[qdmaNr].b_running = S_FALSE;
      }
   }

   /* Configure queue to transfer controller mapping */
   DSP_REG(IVA2_TPCC_QUETCMAP) =
      (EVENT_QUEUE1_TC << 4) |
      (EVENT_QUEUE0_TC << 0) ;

   /* Configure queue priorities */
   DSP_REG(IVA2_TPCC_QUEPRI) =
      (0 << 0u) |  /* queue 0 priority */
      (0 << 4u) ;  /* queue 1 priority */

   /* Configure TPTC read rate */
#if 1
   DSP_REG(IVA2_TPTC0_RDRATE) = 0u;  /* Issue reads as fast as possible */
   DSP_REG(IVA2_TPTC1_RDRATE) = 0u;  /* Issue reads as fast as possible */
#endif

#if 0
   DSP_REG(IVA2_TPTC0_RDRATE) = 2u;  /* 8 cycles between reads */
   DSP_REG(IVA2_TPTC1_RDRATE) = 2u;  /* 8 cycles between reads */
#endif

#if 0
   DSP_REG(IVA2_TPTC0_RDRATE) = 3u;  /* 16 cycles between reads */
   DSP_REG(IVA2_TPTC1_RDRATE) = 3u;  /* 16 cycles between reads */
#endif
}


/* --------------------------------------------------------------------------- qdma0_wait() */
void qdma_wait(sUI _qdmaNr) {

   /* Wait for QDMA */
   if(qdma[_qdmaNr].b_running)
   {
      while(!(DSP_REG(IVA2_TPCC_IPR) & (1u << qdma[_qdmaNr].tcc_nr)))
      {
         /* ..wait for interrupt.. */
      }

      /* Clear interrupt */
      DSP_REG(IVA2_TPCC_ICR) = (1u << qdma[_qdmaNr].tcc_nr);

      qdma[_qdmaNr].b_running = S_FALSE;
   }
}


/* --------------------------------------------------------------------------- qdma_copy1d() */
void qdma_copy1d(sUI _qdmaNr, void *_dst, const void *_src, sU32 _numBytes) {

   volatile sU32 *param = (volatile sU32*) (IVA2_TPCC_PARAM_BASE + (32u * qdma[_qdmaNr].param_nr));

   param[IVA2_TPCC_PARAM_OPT] = 
      (1u << 20u)                      |  /* TCINTEN: enable transfer complete interrupt */
      ((qdma[_qdmaNr].tcc_nr) << 12u)  |  /*     TCC: transfer complete code selection */
      (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
      (1u << 3u)                       |  /*  STATIC: Count and address not updated, no linking */
      (0u << 2u)                       |  /* SYNCDIM: A-Sync, transfer ACNT elements */
      (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
      (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
      0u
      ;

   param[IVA2_TPCC_PARAM_SRC] = (sU32) _src;

   param[IVA2_TPCC_PARAM_ABCNT] =
      (1u << 16u)           |  /* BCNT */
      (_numBytes & 65535u)  |  /* ACNT */
      0u
      ;

   param[IVA2_TPCC_PARAM_DST] = (sU32) _dst;

   param[IVA2_TPCC_PARAM_BIDX] = 0u;  /* no 2D access dest/src increments */

   param[IVA2_TPCC_PARAM_LNK] = 
      (0u << 16u)      |  /* BCNTRLD: no bcnt reload */
      (0xFFFFu << 0u)  |  /*    LINK: link to NULL entry */
      0u
      ;

   param[IVA2_TPCC_PARAM_CIDX] = 0u;  /* no 3D access dest/src increments */

   param[IVA2_TPCC_PARAM_CCNT] = 1u;  /* let's go! */

   qdma[_qdmaNr].b_running = S_TRUE;
}


/* --------------------------------------------------------------------------- qdma_copy2d() */
void qdma_copy2d(sUI _qdmaNr, 
                 void *_dst, sS32 _dstPitch, 
                 const void *_src, sS32 _srcPitch, 
                 sU32 _numBytesX, sU32 _numY
                 ) {

   volatile sU32 *param = (volatile sU32*) (IVA2_TPCC_PARAM_BASE + (32u * qdma[_qdmaNr].param_nr));

   param[IVA2_TPCC_PARAM_OPT] = 
      (1u << 20u)                      |  /* TCINTEN: enable transfer complete interrupt */
      ((qdma[_qdmaNr].tcc_nr) << 12u)  |  /*     TCC: transfer complete code selection */
      (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
      (1u << 3u)                       |  /*  STATIC: Count and address not updated, no linking */
      (1u << 2u)                       |  /* SYNCDIM: AB-Sync, transfer BCNT*ACNT elements */
      (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
      (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
      0u
      ;

   param[IVA2_TPCC_PARAM_SRC] = (sU32) _src;

   param[IVA2_TPCC_PARAM_ABCNT] =
      (_numY << 16u)        |  /* BCNT */
      (_numBytesX & 65535u) |  /* ACNT */
      0u
      ;

   param[IVA2_TPCC_PARAM_DST] = (sU32) _dst;

   param[IVA2_TPCC_PARAM_BIDX] = 
      ((_dstPitch & 65535) << 16) |  /* 2D access dest/src increments */
      ((_srcPitch & 65535) <<  0) ;

   param[IVA2_TPCC_PARAM_LNK] = 
      (0u << 16u)      |  /* BCNTRLD: no bcnt reload */
      (0xFFFFu << 0u)  |  /*    LINK: link to NULL entry */
      0u
      ;

   param[IVA2_TPCC_PARAM_CIDX] = 0u;  /* no 3D access dest/src increments */

   param[IVA2_TPCC_PARAM_CCNT] = 1u;  /* let's go! */

   qdma[_qdmaNr].b_running = S_TRUE;
}


/* --------------------------------------------------------------------------- qdma_link1d() */
void qdma_link1d(sUI _qdmaNr,
                 sUI _paramNr, sUI _linkToParamNr,
                 void *_dst, const void *_src, sU32 _numBytes
                 ) {

   volatile sU32 *param = (volatile sU32*) (IVA2_TPCC_PARAM_BASE + (32u * _paramNr));

   if(_linkToParamNr <= 127u)
   {
      /* Links to next PaRAM entry */
      param[IVA2_TPCC_PARAM_OPT] = 
         0u                               |  /* TCINTEN: disable transfer complete interrupt */
         0u                               |  /*     TCC: transfer complete code selection */
         (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
         0u                               |  /*  STATIC: Link to _linkToParamNr */
         (0u << 2u)                       |  /* SYNCDIM: A-Sync, transfer ACNT elements */
         (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
         (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
         0u
         ;
   }
   else
   {
      /* Final PaRAM entry */
      param[IVA2_TPCC_PARAM_OPT] = 
         (1u << 20u)                      |  /* TCINTEN: enable transfer complete interrupt */
         ((qdma[_qdmaNr].tcc_nr) << 12u)  |  /*     TCC: transfer complete code selection */
         (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
         (1u << 3u)                       |  /*  STATIC: No linking */
         (0u << 2u)                       |  /* SYNCDIM: A-Sync, transfer ACNT elements */
         (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
         (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
         0u
         ;
   }


   param[IVA2_TPCC_PARAM_SRC] = (sU32) _src;

   param[IVA2_TPCC_PARAM_ABCNT] =
      (1u << 16u)           |  /* BCNT */
      (_numBytes & 65535u)  |  /* ACNT */
      0u
      ;

   param[IVA2_TPCC_PARAM_DST] = (sU32) _dst;

   param[IVA2_TPCC_PARAM_BIDX] = 0u;  /* no 2D access dest/src increments */

   param[IVA2_TPCC_PARAM_LNK] = 
      (0u << 16u)      |  /* BCNTRLD: no bcnt reload */
      (_linkToParamNr <= 127u) ? (32u * _linkToParamNr) : (0xFFFFu << 0u)  |  /*    LINK: link to PaRAM (or NULL) entry */
      0u
      ;

   param[IVA2_TPCC_PARAM_CIDX] = 0u;  /* no 3D access dest/src increments */

   param[IVA2_TPCC_PARAM_CCNT] = 1u;  /* let's go! */

   /* Setting up a PaRAM entry < 8 starts the transfer */
   if(_paramNr == _qdmaNr)
   {
      qdma[_qdmaNr].b_running = S_TRUE;
   }
}


/* --------------------------------------------------------------------------- qdma_copy2d() */
void qdma_link2d(sUI _qdmaNr,
                 sUI _paramNr, sUI _linkToParamNr,
                 void *_dst, sS32 _dstPitch,
                 const void *_src, sS32 _srcPitch,
                 sU32 _numBytesX, sU32 _numY
                 ) {

   volatile sU32 *param = (volatile sU32*) (IVA2_TPCC_PARAM_BASE + (32u * _paramNr));

   if(_linkToParamNr <= 127u)
   {
      /* Links to next PaRAM entry */
      param[IVA2_TPCC_PARAM_OPT] = 
         0u                               |  /* TCINTEN: disable transfer complete interrupt */
         0u                               |  /*     TCC: transfer complete code selection */
         (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
         0u                               |  /*  STATIC: Link to _linkToParamNr */
         (1u << 2u)                       |  /* SYNCDIM: AB-Sync, transfer BCNT*ACNT elements */
         (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
         (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
         0u
         ;
   }
   else
   {
      /* Final PaRAM entry */
      param[IVA2_TPCC_PARAM_OPT] = 
         (1u << 20u)                      |  /* TCINTEN: enable transfer complete interrupt */
         ((qdma[_qdmaNr].tcc_nr) << 12u)  |  /*     TCC: transfer complete code selection */
         (0u << 8u)                       |  /*    FWID: FIFO width is 8-bit */
         (1u << 3u)                       |  /*  STATIC: No linking */
         (1u << 2u)                       |  /* SYNCDIM: AB-Sync, transfer BCNT*ACNT elements */
         (0u << 1u)                       |  /*     DAM: INCR dst addressing mode */
         (0u << 0u)                       |  /*     SAM: INCR src addressing mode */
         0u
         ;
   }


   param[IVA2_TPCC_PARAM_SRC] = (sU32) _src;

   param[IVA2_TPCC_PARAM_ABCNT] =
      (_numY << 16u)        |  /* BCNT */
      (_numBytesX & 65535u) |  /* ACNT */
      0u
      ;

   param[IVA2_TPCC_PARAM_DST] = (sU32) _dst;

   param[IVA2_TPCC_PARAM_BIDX] = 
      ((_dstPitch & 65535) << 16) |  /* 2D access dest/src increments */
      ((_srcPitch & 65535) <<  0) ;

   param[IVA2_TPCC_PARAM_LNK] = 
      (0u << 16u)      |  /* BCNTRLD: no bcnt reload */
      (_linkToParamNr <= 127u) ? (32u * _linkToParamNr) : (0xFFFFu << 0u)  |  /*    LINK: link to PaRAM (or NULL) entry */
      0u
      ;

   param[IVA2_TPCC_PARAM_CIDX] = 0u;  /* no 3D access dest/src increments */

   param[IVA2_TPCC_PARAM_CCNT] = 1u;  /* let's go! */

   /* Setting up a PaRAM entry < 8 starts the transfer */
   if(_paramNr == _qdmaNr)
   {
      qdma[_qdmaNr].b_running = S_TRUE;
   }
}
