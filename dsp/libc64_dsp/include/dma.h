/* ----
 * ---- file   : dma.h
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
 * ---- changed: 23Oct2013, 24Nov2013
 * ----
 * ----
 */

#ifndef __LIBC64_DSP_DMA_H__
#define __LIBC64_DSP_DMA_H__

#include "../../../include/cplusplus_begin.h"


#define NUM_QDMAS  (8u)


/*
 * Initialize QDMA(s).
 *
 *  Must be called before any transfers can be started
 *
 */
S_EXTERN void qdma_init (void);


/*
 * Wait until last QDMA transfer is complete
 *
 */
void qdma_wait (sUI _qdmaNr);


/*
 * Copy one dimensional array (like memcpy)
 *
 */
void qdma_copy1d (sUI _qdmaNr, void *_dst, const void *_src, sU32 _numBytes);


/*
 * Copy two dimensional array
 *
 */
void qdma_copy2d (sUI _qdmaNr, 
                  void *_dst, sS32 _dstPitch, 
                  const void *_src, sS32 _srcPitch, 
                  sU32 _numBytesX, sU32 _numY
                  );


/*
 * Setup one dimensional, linked transfer
 *
 */
void qdma_link1d(sUI _qdmaNr,
                 sUI _paramNr, sUI _linkToParamNr, 
                 void *_dst, const void *_src, sU32 _numBytes
                 );


/*
 * Setup two dimensional, linked transfer
 *
 */
void qdma_link2d (sUI _qdmaNr,
                  sUI _paramNr, sUI _linkToParamNr,
                  void *_dst, sS32 _dstPitch, 
                  const void *_src, sS32 _srcPitch, 
                  sU32 _numBytesX, sU32 _numY
                  );


#include "../../../include/cplusplus_end.h"

#endif /* __LIBC64_DSP_DMA_H__ */
