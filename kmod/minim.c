/* ----
 * ---- file   : minim.c
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>

#include "../include/types.h"
#include "../include/dsp_common.h"

#include "kmod.h"
#include "minim.h"


/*--------------------------------------------------------------------------- minim_init() */
void minim_init(minim_t       *_mm,
                minim_block_t *_blocks,
                sUI            _numBlocks,
                sUI            _blockSizeShift,
                sU32           _physAddrBase
                ) {

   _mm->blocks           = _blocks;
   _mm->num_blocks       = _numBlocks;
   _mm->block_size       = (1u << _blockSizeShift);
   _mm->block_size_shift = _blockSizeShift;
   _mm->phys_addr_base   = _physAddrBase;

   memset(_blocks, 0, (sizeof(minim_block_t) * _mm->num_blocks));

   /* First superblock */
   _blocks[0].free_count = _numBlocks;
}


/*--------------------------------------------------------------------------- minim_alloc() */
dsp_mem_region_t minim_alloc(minim_t *_mm, void *_cl, sU32 _numBytes) {
   sU32 numBlocks;
   dsp_mem_region_t ret;

   /* Round up to block_size */
   numBlocks = ((_numBytes + (_mm->block_size - 1u)) >> _mm->block_size_shift);

   _numBytes = (numBlocks << _mm->block_size_shift);


   /* Find bestmatching fragment/block range */
   if(numBlocks > 0)
   {
      minim_block_t *frBestMatch = NULL;
      sUI idxBestMatch = 0;
      sUI i;
      minim_block_t *fr = _mm->blocks;
      minim_block_t *endOfBlocks = &_mm->blocks[_mm->num_blocks];

      i = 0;

      while(fr < endOfBlocks)
      {
         if(fr->free_count > 0)
         {
            if(fr->free_count >= numBlocks)
            {
               if(NULL != frBestMatch)
               {
                  if(fr->free_count < frBestMatch->free_count)
                  {
                     frBestMatch = fr;
                     idxBestMatch = i;
                  }
               }
               else
               {
                  frBestMatch = fr;
                  idxBestMatch = i;
               }
            }
            else
            {
               /* Fragment/block range is too small for allocation request */
            }

            i  += fr->free_count;
            fr += fr->free_count;
         }
         else
         {
            /* Skip allocated fragment/block range */
            i  += fr->alloc_count;
            fr += fr->alloc_count;
         }
      }

      if(NULL != frBestMatch)
      {
         fr = frBestMatch;

         fr->alloc_count = numBlocks;

#if 1
         for(i=1; i<numBlocks; i++)
         {
            /* Mark block as used */
            fr[i].free_count = 0;
         }
#endif

         if(fr->alloc_count < fr->free_count)
         {
            /* Create new superblock */
            fr[fr->alloc_count].free_count = (fr->free_count - fr->alloc_count);
         }

         /* Succeeded */
         fr->free_count = 0;
         fr->owner      = _cl;

         ret.virt_addr = 0;  /* calc'd by libc64 */
         ret.phys_addr =
            _mm->phys_addr_base + (idxBestMatch << _mm->block_size_shift);

         ret.size = _numBytes;
      }
      else
      {
         /* Failed: out of memory */

         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::minim::minim_alloc: failed to find free block range, numBytes=%u\n", _numBytes);
         }

         ret.phys_addr = 0;
         ret.virt_addr = 0;
         ret.size      = 0;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_minim_free_by_idx() */
static long loc_minim_free_by_idx(minim_t *_mm, void *_cl, sUI blockIdx) {
   long ret;
   minim_block_t *fr = &_mm->blocks[blockIdx];

   if(fr->alloc_count > 0)
   {
      if(fr->owner == _cl)
      {
         fr->owner       = NULL;
         fr->free_count  = fr->alloc_count;

         /* Try to join right hand side super block */
         if( (blockIdx + fr->alloc_count) < _mm->num_blocks )
         {
            minim_block_t *rfr = &fr[fr->alloc_count];

            if(0 == rfr->alloc_count)
            {
               /* Join RHS SB */
               fr->free_count += rfr->free_count;
               rfr->free_count = 0;
            }
         }

         /* Try to join left hand side super block */
         if(blockIdx > 0)
         {
            minim_block_t *lfr = fr - 1;

            do
            {
               if((lfr->free_count > 0) || (lfr->alloc_count > 0))
               {
                  break;
               }
               else
               {
                  lfr--;
               }
            }
            while(lfr >= _mm->blocks);
               
            if(lfr->free_count > 0)
            {
               /* Join LHS SB */
               lfr->free_count += fr->free_count;
               fr->free_count   = 0;
            }
         }

         /* Succeeded */
         fr->alloc_count = 0;

         ret = 0;
      }
      else
      {
         /* Failed: super block not owned by client */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::minim::minim_free: super block %u not owned by client (cl=0x%p, mem.phys_addr=0x%08x)\n",
                   blockIdx,
                   _cl,
                   _mm->phys_addr_base + (blockIdx << _mm->block_size_shift)
                   );
         }

         ret = -EACCES;
      }
   }
   else
   {
      /* Failed: not a super block */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::minim::minim_free: %u is not a super block (mem.phys_addr=0x%08x)\n",
                blockIdx,
                _mm->phys_addr_base + (blockIdx << _mm->block_size_shift)
                );
      }

      ret = -EFAULT;
   }

   return ret;
}


/*--------------------------------------------------------------------------- minim_free() */
long minim_free(minim_t *_mm, void *_cl, dsp_mem_region_t _mem) {
   long ret;

   sUI blockIdx =
      (_mem.phys_addr - _mm->phys_addr_base) >> _mm->block_size_shift
      ;

   if(blockIdx < _mm->num_blocks)
   {
      ret = loc_minim_free_by_idx(_mm, _cl, blockIdx);
   }
   else
   {
      /* Failed: address out of bounds */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::minim::minim_free: invalid mem.phys_addr=0x%08x.\n",
                _mem.phys_addr
                );
      }

      ret = -EFAULT;
   }

   return ret;
}


/*--------------------------------------------------------------------------- minim_free_by_client() */
void minim_free_by_client(minim_t *_mm, void *_cl) {
   sBool bFreed = S_TRUE;

   minim_block_t *fr = _mm->blocks;
   minim_block_t *endOfBlocks = (&_mm->blocks[_mm->num_blocks]);

   while(bFreed)
   {
      bFreed = S_FALSE;

      while(fr < endOfBlocks)
      {
         if(fr->alloc_count > 0)
         {
            if(fr->owner == _cl)
            {
               loc_minim_free_by_idx(_mm, _cl, (sUI) (fr - _mm->blocks));

               bFreed = S_TRUE;
               break;
            }

            /* Not owned by client, skip to next super block */
            fr += fr->alloc_count;
         }
         else
         {
            /* Not allocated, skip to next super block */
            fr += fr->free_count;
         }
      }
   }
}


/*--------------------------------------------------------------------------- minim_debug() */
void minim_debug(minim_t *_mm, sUI _sequenceId, const char *_msgPrefix) {
   sUI i;
   minim_block_t *fr = _mm->blocks;
   sBool bAlloc = S_FALSE;

   printk(KERN_DEBUG "\n%s: 0x%08x  ------------------------------------------\n",
          _msgPrefix, _sequenceId
          );

   for(i=0; i< _mm->num_blocks; i++)
   {
      if(fr->alloc_count > 0)
      {
         bAlloc = S_TRUE;

         printk(KERN_DEBUG "%s: fr[%03u] allocated super block alloc_count=%03u free_count=%03u owner=0x%08x\n",
                _msgPrefix,
                i,
                fr->alloc_count,
                fr->free_count,
                (sU32)fr->owner
                );
      }
      else if(fr->free_count > 0)
      {
         bAlloc = S_FALSE;

         printk(KERN_DEBUG "%s: fr[%03u] free super block      alloc_count=%03u free_count=%03u owner=0x%08x\n",
                _msgPrefix,
                i,
                fr->alloc_count,
                fr->free_count,
                (sU32)fr->owner
                );
      }
      else
      { 
         printk(KERN_DEBUG "%s: fr[%03u] %s           alloc_count=%03u free_count=%03u owner=0x%08x\n",
                _msgPrefix,
                i,
                (bAlloc ? "<allocated>" : "<free>     "),
                fr->alloc_count,
                fr->free_count,
                (sU32)fr->owner
                );
      }

      /* Next block */
      fr++;
   }
}
