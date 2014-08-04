/* ----
 * ---- file   : syscalls.h
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
 * ---- changed: 02Oct2013, 03Oct2013, 04Oct2013, 21Oct2013, 22Oct2013, 23Oct2013, 24Nov2013
 * ----
 * ----
 */

#ifndef __MLB_SYSCALLS_H__
#define __MLB_SYSCALLS_H__


#define puts please_use_syscalls_puts
#define printf please_use_syscalls_printf
#define vprintf please_use_syscalls_vprintf


#define QDMA_0     (0u)
#define QDMA_1     (1u)
#define QDMA_2     (2u)
#define QDMA_3     (3u)
#define QDMA_4     (4u)
#define QDMA_5     (5u)
#define QDMA_6     (6u)
#define QDMA_7     (7u)


typedef struct {

   /*--------------------------------------------------------------------------*/
   /*  c64_tools / mailbox syscalls                                            */
   /*--------------------------------------------------------------------------*/       

   /* #000
    * Should be called when things terribly go wrong and the system / DSP
    *  becomes unusable.
    *
    *  Use with extreme caution.
    *
    */
   void (*mlb_panic) (void);


   /* #001
    * Write value to debug location
    *
    *   idx: 0..3
    */
   void (*mlb_debug_usr) (sU32 _idx, sU32 _val);


   /* #002
    * This function can be called by DSP components in order to initiate a fastcall sequence.
    *
    */
   void (*mlb_fastcall_initiate) (void);


   /* #003
    * Register additional (overlay) component.
    *
    *  (note) Usually each overlay section is associated with exactly one component.
    *          By using the mlb_component_register() syscall it is possible
    *          to have more than one component per overlay.
    *
    *  (note) The _secIdx argument value can be determined by looking at the address of
    *          the main overlay component and looking that up in the overlay_sections table
    *          (see dsp/linker_scripts/overlay_sections.c)
    *
    *  (note) When an overlay section is unloaded, all components it has registered will 
    *          automatically be unregistered.
    *         
    *
    */
   sBool (*mlb_component_register) (dsp_component_t *_component, sU16 _sectionIdx);


   /* #004
    * Find component by name
    */
   dsp_component_t *(*mlb_component_find_by_name) (const char *_name, sBool _bIncRef);


   /* #005
    * Query API version number
    */
   sU32 (*mlb_version) (void);


   void (*__mlb_reservd06) (void);
   void (*__mlb_reservd07) (void);
   void (*__mlb_reservd08) (void);
   void (*__mlb_reservd09) (void);
   void (*__mlb_reservd10) (void);
   void (*__mlb_reservd11) (void);
   void (*__mlb_reservd12) (void);
   void (*__mlb_reservd13) (void);
   void (*__mlb_reservd14) (void);
   void (*__mlb_reservd15) (void);


   /*--------------------------------------------------------------------------*/
   /*  cache utility fuctions                                                  */
   /*--------------------------------------------------------------------------*/       

   /* #016
    * Invalidate L1P/L1D/L2 caches for given range.
    *
    *   blockPtr - Start address of range to be invalidated
    *    byteCnt - Number of bytes to be invalidated
    *      bWait - If true(1), wait until the operation is completed
    *
    *  (note) this uses BCACHE_inv()
    *
    */
   void (*cache_inv) (void *_blockPtr, sUI _byteCnt, sBool _bWait);


   /* #017
    * Write back the range of memory in all caches (L1P/L1D/L2).
    *
    *   blockPtr - Start address of range to be invalidated
    *    byteCnt - Number of bytes to be invalidated
    *      bWait - If true(1), wait until the operation is completed
    *
    *  (note) this uses BCACHE_wb()
    *
    */
   void (*cache_wb) (void *_blockPtr, sUI _byteCnt, sBool _bWait);

   
   /* #018
    * Write back and invalidate the range of memory in all caches (L1P/L1D/L2).
    *
    *   blockPtr - Start address of range to be invalidated
    *    byteCnt - Number of bytes to be invalidated
    *      bWait - If true(1), wait until the operation is completed
    *
    *  (note) this uses BCACHE_wbInv()
    *
    */
   void (*cache_wbInv) (void *_blockPtr, sUI _byteCnt, sBool _bWait);


   /* #019
    * Invalidate the L1P cache completely.
    *
    *  (note) this uses BCACHE_invL1pAll()
    *
    */
   void (*cache_invL1pAll) (void);


   /* #020
    * Perform a global writeback (L1P/L1D/L2D)
    *
    *  (note) this uses BCACHE_wbAll()
    *
    */
   void (*cache_wbAll) (void);


   /* #021
    * Perform a global writeback and invalidate (L1P/L1D/L2D)
    *
    *  (note) this uses BCACHE_wbInvAll()
    *
    */
   void (*cache_wbInvAll) (void);


   /* #022
    * Wait for previously issued cache operation to complete.
    *
    *  (note) this uses BCACHE_wait()
    *
    */
   void (*cache_wait) (void);


   void (*__cache_reserved23) (void);


   /*--------------------------------------------------------------------------*/
   /*  mem utility fuctions                                                    */
   /*--------------------------------------------------------------------------*/       

   /* (note) it is quite possible to call these functions directly but this will
    *         increase the size of the overlay image.
    */

   /* #024 */
   void  (*bzero)  (void *_addr, size_t _numBytes);


   /* #025 */
   void *(*memset) (void *_addr, int _char, size_t _numBytes);


   /* #026 */
   void *(*memcpy) (void *_destAddr, const void *_srcAddr, size_t _numBytes);


   void (*__mem_reserved27) (void);
   void (*__mem_reserved28) (void);
   void (*__mem_reserved29) (void);
   void (*__mem_reserved30) (void);
   void (*__mem_reserved31) (void);


   /*--------------------------------------------------------------------------*/
   /*  string utility fuctions                                                 */
   /*--------------------------------------------------------------------------*/       

   /* (note) it is quite possible to call some of these functions directly but this will
    *         increase the size of the overlay image.
    */

   /* #032 */
   int   (*strncmp)   (const char *_str1, const char *_str2, size_t _maxChars);


   /* #033 */
   char *(*strncpy)   (char *_destAddr, const char *_srcAddr, size_t _maxChars);


   /* #034 */
   char *(*strstr)    (const char *_haystack, const char *_needle);


   /* #035 */
   char *(*strchr)    (const char *_str, int _char);


   /* #036 */
   char *(*strrchr)   (const char *_str, int _char);


   /* #037 */
   int   (*snprintf)  (char *_str, size_t _maxChars, const char *_format, ...);


   /* #038 */
   int   (*vsnprintf) (char *_str, size_t _maxChars, const char *_format, va_list _ap);


   /* #039 */
   int   (*puts)      (const char *_str);


   /* #040 */
   int   (*printf)    (const char *_fmt, ...);


   /* #041 */
   int   (*vprintf)   (const char *_fmt, va_list _l);

   void (*__string_reserved42) (void);
   void (*__string_reserved43) (void);
   void (*__string_reserved44) (void);
   void (*__string_reserved45) (void);
   void (*__string_reserved46) (void);
   void (*__string_reserved47) (void);


   /*--------------------------------------------------------------------------*/
   /*  QDMA utility fuctions                                                   */
   /*--------------------------------------------------------------------------*/       

   /* #048
    * Initialize QDMA. Must be called before any transfers can be done.
    *
    */
   void (*qdma_init)   (void);

   /* #049 
    * Wait until QDMA transfer is complete. 
    *
    */
   void (*qdma_wait)   (sUI _qdmaNr);

   /* #050
    * Start one dimensional QDMA transfer.
    *
    *  (note) _numBytes must not exceed 65535.
    *
    *  (note) use qdma_copy2d() for longer transfers.
    *
    */
   void (*qdma_copy1d) (sUI _qdmaNr, void *_dst, const void *_src, sU32 _numBytes);

   /* #051
    * Start two dimensional QDMA transfer.
    *
    *  (note) _numBytesX must not exceed 65535.
    *
    *  (note) _numY must not exceed 65535.
    *
    *  (note) _dstPitch and _srcPitch must be in the range -32768..32767.
    *
    */
   void (*qdma_copy2d) (sUI _qdmaNr, 
                        void *_dst, sS32 _dstPitch,
                        const void *_src, sS32 _srcPitch,
                        sU32 _numBytesX, sU32 _numY
                        );

   /* #052
    * Setup one dimensional, linked QDMA transfer resp. PaRAM entry
    *
    *  (note) also see qdma_copy1d()
    */
   void (*qdma_link1d) (sUI _qdmaNr,
                        sUI _paramNr, sUI _linkToParamNr, 
                        void *_dst, const void *_src, sU32 _numBytes
                        );

   /* #053
    * Setup two dimensional, linked QDMA transfer resp. PaRAM entry
    *
    *  (note) also see qdma_copy2d()
    */
   void (*qdma_link2d) (sUI _qdmaNr, 
                        sUI _paramNr, sUI _linkToParamNr,
                        void *_dst, sS32 _dstPitch,
                        const void *_src, sS32 _srcPitch,
                        sU32 _numBytesX, sU32 _numY
                        );
   

} mlb_syscalls_t;


S_EXTERN mlb_syscalls_t syscalls;


#endif /* __MLB_SYSCALLS_H__ */
