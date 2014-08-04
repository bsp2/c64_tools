/* ----
 * ---- file   : minim.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : Minimalistic malloc'ator. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 18Oct2013, 19Oct2013, 21Oct2013, 22Oct2013, 23Oct2013, 01Nov2013, 02Nov2013
 * ----          08Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_MINI_MALLOC_H__
#define __C64_TOOLS_KMOD_MINI_MALLOC_H__


typedef struct {
   sU32  alloc_count;
   sU32  free_count;  /* only set for superblocks, 0 otherwise */
   void *owner;       /* only set for superblocks */
} minim_block_t;


typedef struct {
   sUI            num_blocks; /* [DSP_FSHM_MAX_DYNAMIC_ALLOCATIONS]; */
   sUI            block_size;
   sUI            block_size_shift;
   minim_block_t *blocks;
   sU32           phys_addr_base;
} minim_t;


/*
 * Initialize minimalistic malloc'ator.
 *
 *  "blocks" must have been allocated by the caller.
 *  "blockSizeShift" determine the size of each block (e.g. 6 => 1<<6 = 64 bytes)
 */
S_EXTERN void minim_init (minim_t *_mm,
                          minim_block_t *_blocks,
                          sUI _numBlocks,
                          sUI _blockSizeShift,
                          sU32 _physAddrBase
                          );


/*
 * Allocate memory
 *
 *  (note) _numBytes is rounded up to block_size
 *
 */
S_EXTERN dsp_mem_region_t minim_alloc (minim_t *_mm, void *_cl, sU32 _numBytes);


/*
 * Free memory (by address)
 *
 */
S_EXTERN long minim_free (minim_t *_mm, void *_cl, dsp_mem_region_t _mem);


/*
 * Free memory (by owner)
 *
 */
S_EXTERN void minim_free_by_client (minim_t *_mm, void *_cl);


/* 
 * Debug print allocator info
 *
 */
S_EXTERN void minim_debug (minim_t *_mm, sUI _sequenceId, const char *_msgPrefix);


#endif /* __C64_TOOLS_KMOD_MINI_MALLOC_H__ */
