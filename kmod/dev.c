/* ----
 * ---- file   : dev.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          (c) 2013 by Gražvydas Ignotas <notasas@gmail.com> (loc_hugetlb_virt_to_phys())
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 11Sep2013, 12Sep2013, 14Sep2013, 16Sep2013, 17Sep2013, 18Sep2013, 19Sep2013
 * ----          22Sep2013, 23Sep2013, 02Oct2013, 03Oct2013, 04Oct2013, 05OCt2013, 15Oct2013
 * ----          16Oct2013, 18Oct2013, 19Oct2013, 21Oct2013, 22Oct2013, 23Oct2013, 01Nov2013
 * ----          02Nov2013, 08Dec2013, 13Dec2013, 15Dec2013, 14Jan2014, 18Jan2014
 * ----
 * ----
 */

/* If not defined, a 32mb memory hole must be reserved @0x86000000 */
#define USE_CMA_FOR_POOLS defined


#define __NO_VERSION__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/hugetlb.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <linux/clk.h>

#include <linux/../../arch/arm/include/asm/cacheflush.h>

#include "../include/types.h"
#include "../include/msg.h"
#include "../include/dsp_common.h"
#include "../include/dsp_config.h"
#include "../include/c64_dev.h"
#include "../dsp/libc64_dsp/include/com.h"
#include "../dsp/core/components.h"

#include "../include/arch/dm3730/dm3730.h"

#include "kmod.h"
#include "reg.h"
#include "mlb.h"
#include "dsp_c64.h"
#include "minim.h"
#include "dev.h"


/*--------------------------------------------------------------------------- c64_dma_*() */

/* (note) these are copied from cmemk.c, which most likely copied it from Linux arch/arm/mm/cache-v7.S
 *         however, this functions look slightly different
 *
 */


/*
 *	c64_v7_dma_inv_range(start,end)
 *
 *	Invalidate the data cache within the specified region; we will
 *	be performing a DMA operation in this region and we want to
 *	purge old data in the cache.
 *
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
asm("\n \
	.global c64_v7_dma_inv_range\n \
c64_v7_dma_inv_range:\n \
@	dcache_line_size r2, r3\n \
        mrc     p15, 0, r3, c0, c0, 1         @ read ctr\n \
        lsr     r3, r3, #16\n \
        and     r3, r3, #0xf                @ cache line size encoding\n \
        mov     r2, #4                        @ bytes per word\n \
        mov     r2, r2, lsl r3            @ actual cache line size\n \
\n \
	sub	r3, r2, #1\n \
	tst	r0, r3\n \
	bic	r0, r0, r3\n \
@ #ifdef CONFIG_ARM_ERRATA_764369\n \
@ 	ALT_SMP(W(dsb))\n \
@ 	ALT_UP(W(nop))\n \
@ #endif\n \
	mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line\n \
	tst	r1, r3\n \
	bic	r1, r1, r3\n \
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line\n \
1:\n \
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line\n \
	add	r0, r0, r2\n \
	cmp	r0, r1\n \
	blo	1b\n \
	dsb\n \
	mov	pc, lr\n \
");

/*
 *	c64_v7_dma_clean_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
asm("\n \
	.global c64_v7_dma_clean_range\n \
c64_v7_dma_clean_range:\n \
@	dcache_line_size r2, r3\n \
        mrc     p15, 0, r3, c0, c0, 1         @ read ctr\n \
        lsr     r3, r3, #16\n \
        and     r3, r3, #0xf                @ cache line size encoding\n \
        mov     r2, #4                        @ bytes per word\n \
        mov     r2, r2, lsl r3            @ actual cache line size\n \
\n \
	sub	r3, r2, #1\n \
	bic	r0, r0, r3\n \
@ #ifdef CONFIG_ARM_ERRATA_764369\n \
@ 	ALT_SMP(W(dsb))\n \
@ 	ALT_UP(W(nop))\n \
@ #endif\n \
1:\n \
	mcr	p15, 0, r0, c7, c10, 1		@ clean D / U line\n \
	add	r0, r0, r2\n \
	cmp	r0, r1\n \
	blo	1b\n \
	dsb\n \
	mov	pc, lr\n \
");

/*
 *	c64_v7_dma_flush_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
asm("\n \
	.global c64_v7_dma_flush_range\n \
c64_v7_dma_flush_range:\n \
@	dcache_line_size r2, r3\n \
        mrc     p15, 0, r3, c0, c0, 1         @ read ctr\n \
        lsr     r3, r3, #16\n \
        and     r3, r3, #0xf                @ cache line size encoding\n \
        mov     r2, #4                        @ bytes per word\n \
        mov     r2, r2, lsl r3            @ actual cache line size\n \
\n \
	sub	r3, r2, #1\n \
	bic	r0, r0, r3\n \
@ #ifdef CONFIG_ARM_ERRATA_764369\n \
@ 	ALT_SMP(W(dsb))\n \
@ 	ALT_UP(W(nop))\n \
@ #endif\n \
1:\n \
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line\n \
	add	r0, r0, r2\n \
	cmp	r0, r1\n \
	blo	1b\n \
	dsb\n \
	mov	pc, lr\n \
");

/*
 *	c64_dma_map_area(start, size, dir)
 *	- start	- kernel virtual start address
 *	- size	- size of region
 *	- dir	- DMA direction (1=clean (TO_DEVICE), 2=inv (FROM_DEVICE), flush otherwise (BIDIRECTIONAL))
 */
asm("\n \
	.global c64_v7_dma_map_area\n \
c64_v7_dma_map_area:\n \
	add	r1, r1, r0\n \
@        cmp     r2, #DMA_TO_DEVICE\n \
        cmp     r2, #1\n \
        beq     c64_v7_dma_clean_range\n \
        bcs     c64_v7_dma_inv_range\n \
        b       c64_v7_dma_flush_range\n \
");


void c64_v7_dma_map_area(void *start, sUI size, sUI dir);



/*--------------------------------------------------------------------------- module vars */
static dev_t          c64_dev_nr;
static struct cdev *  c64_cdev;
static struct class * c64_class;
static struct device *c64_device;

DEFINE_MUTEX(mtx_clients);
struct mutex mtx_clients;

static struct clk *iva2_clk;

static sBool b_dev_enabled;


/*--------------------------------------------------------------------------- client typedefs */
typedef enum {
   STATE_UNUSED        = 0,  /* client slot is free for allocation (open). */
   STATE_IDLE          = 1,  /* client is connected but there's no activity ATM. */
   STATE_REQ_QUEUED    = 2,  /* (todo) message is queued and will be sent to DSP ASAP. */
   STATE_REPLY_PENDING = 3,  /* waiting for reply from DSP. */
   STATE_REPLY_AVAIL   = 4,  /* reply from DSP has been recv'd but not picked up by the client, yet. */
} dev_client_state_t;


typedef struct {
   void *           kvirt_addr;
   dsp_mem_region_t dsp;
   struct dma_attrs dma_attrs;
} client_shm_t;


typedef struct {
   atomic_t          at_state;
   struct file *     filep;
   dsp_msg_t         dsp_msg;
   client_shm_t      shm[NUM_DSP_CACHE_TYPES];
   dsp_mem_region_t  l1sram_fshm;
   dsp_mem_region_t  l2sram_fshm;
   dsp_mem_region_t  logbuf;
   wait_queue_head_t wq_read;
   sBool             b_overlay_section_allocflags[MLB_COM_NUM_SECTION_INDICES];
   dsp_mem_region_t  overlay_section_mem[MLB_COM_NUM_SECTION_INDICES];
   sU32              dsp_errno;

} dev_client_t;


/*
 * (note) also see gen_link_area.tks script which generate the overlay_sections array from config.tci
 */
#include "../dsp/linker_scripts/overlay_sections.c"

static sBool b_overlay_section_allocflags[MLB_COM_NUM_SECTION_INDICES];


/*--------------------------------------------------------------------------- L1SRAM fshm dynamic allocator */
minim_t       l1sram_fshm_mm;
minim_block_t l1sram_fshm_blocks[DSP_L1SRAM_MAX_DYNAMIC_ALLOCATIONS];


/*--------------------------------------------------------------------------- L2SRAM fshm dynamic allocator */
minim_t       l2sram_fshm_mm;
minim_block_t l2sram_fshm_blocks[DSP_L2SRAM_MAX_DYNAMIC_ALLOCATIONS];


/*--------------------------------------------------------------------------- client module vars */
static dev_client_t clients[C64_DEV_MAX_CLIENTS];

static sU32 client_powerstate_mask;

static volatile dev_client_t *current_client; /* != NULL while transmission in progress */


/*--------------------------------------------------------------------------- loc_init_clients() */
static void loc_init_clients(void) {
   /* Initialize clients array */
   sUI i;
   dev_client_t *cl = clients;
   
   for(i=0; i<C64_DEV_MAX_CLIENTS; i++)
   {
      atomic_t atInit = ATOMIC_INIT(STATE_UNUSED);
      
      memset(cl, 0, sizeof(dev_client_t));
      
      cl->at_state = atInit;
      
      /* Next client */
      cl++;
   }

   memset(b_overlay_section_allocflags, 0, sizeof(sBool) * MLB_COM_NUM_SECTION_INDICES);

   client_powerstate_mask = 0u;
}


/*--------------------------------------------------------------------------- loc_find_client_by_file() */
static dev_client_t *loc_find_client_by_filep(struct file *_filep) {
   dev_client_t *ret;
   sUI i;

   ret = clients;

   for(i=0; i<C64_DEV_MAX_CLIENTS; i++)
   {
      if(ret->filep == _filep)
      {
         if(STATE_UNUSED != atomic_read(&ret->at_state))
         {
            /* Succeeded */
            break;
         }
      }

      /* Try next client struct */
      ret++;
   }

   if(C64_DEV_MAX_CLIENTS == i)
   {
      /* Failed: client not found */
      ret = NULL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_hugetlb_virt_to_phys() */
u32 loc_hugetlb_virt_to_phys(u32 virt_addr) {
   /* contributed by Notaz */

   pgd_t *pgd = pgd_offset(current->mm, virt_addr);
   u32 l1_entry = *(u32 *)pgd;
   u32 phys = 0;

   if ((l1_entry & 3) == 2) { /* section a.k.a. huge page */
      if (l1_entry & (1<<18)) /* supersection */
         phys = (l1_entry & 0xFF000000) | (virt_addr & 0x00FFFFFF);
      else
         phys = (l1_entry & 0xFFF00000) | (virt_addr & 0x000FFFFF);
   }

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev::loc_hugetlb_virt_to_phys: virt=0x%08x phys=0x%08x\n", virt_addr, phys);
   }

   return phys;
}


/*--------------------------------------------------------------------------- loc_shm_alloc() */
static long loc_shm_alloc(dev_client_t *_client, c64_dev_ioctl_shm_alloc_t *_req) {
   long ret;
   sUI numBytes;
   dma_addr_t dmaAddr;
   void *kVirtAddr;

   if( (_req->type >= 0) && (_req->type < NUM_CONTIG_DSP_CACHE_TYPES) )
   {
      if(NULL == _client->shm[_req->type].kvirt_addr)
      {
         /* Round up req.size to page size */
         numBytes = (((_req->mem.size + DSP_PAGESIZE - 1u) >> DSP_PAGESHIFT) << DSP_PAGESHIFT);

#ifdef USE_CMA_FOR_POOLS
         /* Allocate physically contiguous shared memory */

         if(C64_DEBUG_LEVEL(10))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: calling dma_alloc_attrs()\n");
         }

         if(DSP_CACHE_RW == _req->type)
         {
            struct dma_attrs *dmaAttrs = &_client->shm[_req->type].dma_attrs;

            init_dma_attrs(dmaAttrs);

            dma_set_attr(DMA_ATTR_WEAK_ORDERING,     dmaAttrs);
            dma_set_attr(DMA_ATTR_WRITE_COMBINE,     dmaAttrs);
#if 0
            dma_set_attr(DMA_ATTR_NO_KERNEL_MAPPING, dmaAttrs);  /* not implemented ? */
#endif

            dmaAddr = (dma_addr_t)NULL;

            kVirtAddr = dma_alloc_attrs(NULL,
                                        (size_t)numBytes,
                                        &dmaAddr,
                                        GFP_KERNEL,
                                        dmaAttrs
                                        );

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: dma_alloc_attrs returned dmaAddr=0x%08x kVirtAddr=0x%08x\n",
                      (sU32)dmaAddr,
                      (sU32)kVirtAddr
                      );
            }
         }
         else
         {
            kVirtAddr = dma_alloc_coherent(NULL,
                                           (size_t)numBytes,
                                           &dmaAddr,
                                           GFP_KERNEL
                                           );
         }

         if(C64_DEBUG_LEVEL(10))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: dma_alloc_coherent() returned kVirtAddr=0x%08x physAddr=0x%08x\n",
                   (sU32)kVirtAddr,
                   (sU32)dmaAddr
                   );
         }

#else
         /* ------ static memory configuration ------ */
         {
            sU32 off;
            sU32 szAvail;

            switch(_req->type)
            {
               default:
               case DSP_CACHE_NONE:
                  off = 0;
                  szAvail = DSP_STATICMEM_CACHE_NONE_SIZE;
                  break;

               case DSP_CACHE_RW:
                  off = DSP_STATICMEM_CACHE_NONE_SIZE;
                  szAvail = DSP_STATICMEM_CACHE_RW_SIZE;
                  break;

               case DSP_CACHE_R:
                  off = DSP_STATICMEM_CACHE_NONE_SIZE + DSP_STATICMEM_CACHE_RW_SIZE;
                  szAvail = DSP_STATICMEM_CACHE_R_SIZE;
                  break;

               case DSP_CACHE_W:
                  off = DSP_STATICMEM_CACHE_NONE_SIZE + DSP_STATICMEM_CACHE_RW_SIZE + DSP_STATICMEM_CACHE_R_SIZE;
                  szAvail = DSP_STATICMEM_CACHE_W_SIZE;
                  break;
            }

            if(numBytes <= szAvail)
            {
               /* Assume that pool/shared mem area starts directly after DSP code area */
               dmaAddr = (dma_addr_t) (dsp_config.ram.phys_addr + dsp_config.ram.size + off);

               if(C64_DEBUG_LEVEL(10))
               {
                  printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: type=%d dmaAddr=0x%08x calling ioremap\n",
                         _req->type,
                         (sU32)dmaAddr
                         );
               }
               
               if(DSP_CACHE_NONE == _req->type)
               {
                  kVirtAddr = ioremap_nocache((sU32)dmaAddr, numBytes);

                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: ioremap_nocache returned kVirtAddr=0x%08x\n",
                            (sU32)kVirtAddr
                            );
                  }
               }
               else
               {
                  kVirtAddr = ioremap((sU32)dmaAddr, numBytes);

                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: ioremap returned kVirtAddr=0x%08x\n",
                            (sU32)kVirtAddr
                            );
                  }
               }
            }
            else
            {
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::ioctl[SHM_ALLOC]: requested shm size (%u) exceeds available #bytes (%u)\n",
                         numBytes,
                         szAvail
                         );
               }

               dmaAddr   = (dma_addr_t) NULL;
               kVirtAddr = NULL;
            }

         }

         if(C64_DEBUG_LEVEL(10))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: ioremap*() returned kVirtAddr=0x%08x physAddr=0x%08x\n",
                   (sU32)kVirtAddr,
                   (sU32)dmaAddr
                   );
         }
#endif /* USE_CMA_FOR_POOLS */
   
   
         if(NULL != kVirtAddr)
         {
            if(C64_DEBUG_LEVEL(5))
            {
               printk(KERN_DEBUG "c64::dev::ioctl[SHM_ALLOC]: allocated %u kBytes (%u pages).\n",
                      (_req->mem.size >> 10u),
                      (_req->mem.size >> 12u)
                      );
            }

            _client->shm[_req->type].dsp.size       = (sU32) _req->mem.size;
            _client->shm[_req->type].dsp.phys_addr  = (sU32) dmaAddr;
            _client->shm[_req->type].dsp.virt_addr  = (sU32) 0; /* will be set in fops__mmap() */
            _client->shm[_req->type].kvirt_addr     = kVirtAddr;

            /* Succeeded */
            ret = 0;
         }
         else
         {
            /* Failed: dma_alloc_coherent() */
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl[SHM_ALLOC]: failed to allocate %u kBytes (%u pages).\n",
                      (_req->mem.size >> 10u),
                      (_req->mem.size >> 12u)
                      );
            }

            ret = -ENOMEM;
         }
      }
      else
      {
         /* Failed: memory already allocated */
         ret = -ENOMEM;
      }
   }
   else
   {
      if(DSP_CACHE_HUGETLB == _req->type)
      {
         /* (todo) do huge page allocation in c64.ko instead of relying on userspace provided addresses */
         sU32 physAddr = loc_hugetlb_virt_to_phys(_req->mem.virt_addr);

         if(0 != physAddr)
         {
            _client->shm[DSP_CACHE_HUGETLB].dsp.size       = _req->mem.size;
            _client->shm[DSP_CACHE_HUGETLB].dsp.phys_addr  = physAddr;
            _client->shm[DSP_CACHE_HUGETLB].dsp.virt_addr  = _req->mem.virt_addr;  /* set in userspace */
            _client->shm[DSP_CACHE_HUGETLB].kvirt_addr     = 0u;

            ret = 0;
         }
         else
         {
            /* Failed: virt_addr does not map to a hugetlb page */

            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl[SHM_ALLOC]: user provided virt_addr=0x%08x does not map to a hugetlb page.\n",
                      _req->mem.virt_addr
                      );
            }

            ret = -EFAULT;
         }
      }
      else
      {
         /* Failed: invalid shared memory type */
         ret = -EINVAL;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_shm_free() */
static long loc_shm_free(dev_client_t *_client, dsp_cache_t _type) {
   long ret;

   if( (_type >= 0) && (_type < NUM_CONTIG_DSP_CACHE_TYPES) )
   {
      if(NULL != _client->shm[_type].kvirt_addr)
      {
         struct dma_attrs *dmaAttrs;

         if(DSP_CACHE_RW == _type)
         {
            dmaAttrs = &_client->shm[_type].dma_attrs;
         }
         else
         {
            dmaAttrs = NULL;
         }

         if(C64_DEBUG_LEVEL(10))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[SHM_FREE]: calling dma_free_coherent()\n");
         }

#ifdef USE_CMA_FOR_POOLS

         /* Free dynamically allocated memory */
         dma_free_attrs(NULL,
                        _client->shm[_type].dsp.size,
                        (void*)_client->shm[_type].kvirt_addr,
                        (dma_addr_t)_client->shm[_type].dsp.phys_addr,
                        dmaAttrs
                        );

#else
         /* Unmap statically allocated memory */
         iounmap(_client->shm[_type].kvirt_addr);
#endif

         if(C64_DEBUG_LEVEL(5))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[SHM_FREE]: freed %u kBytes (%u pages).\n",
                   (_client->shm[_type].dsp.size >> 10u),
                   (_client->shm[_type].dsp.size >> 12u)
                   );
         }

         _client->shm[_type].kvirt_addr     = NULL;
         _client->shm[_type].dsp.size       = 0;
         _client->shm[_type].dsp.virt_addr  = 0;
         _client->shm[_type].dsp.phys_addr  = 0;
                        
         /* Succeeded */
         ret = 0;
      }
      else
      {
         /* Nothing's allocated ATM */
         ret = 0;
      }
   }
   else
   {
      if(DSP_CACHE_HUGETLB == _type)
      {
         /* Nothing else to do here, actual free() is called from userspace (munmap()) */
         _client->shm[_type].kvirt_addr     = NULL;
         _client->shm[_type].dsp.size       = 0;
         _client->shm[_type].dsp.virt_addr  = 0;
         _client->shm[_type].dsp.phys_addr  = 0;

         ret = 0;
      }
      else
      {
         /* Failed: invalid shared memory type */
         ret = -EINVAL;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- FOPS module vars */
static int     fops__open     (struct inode *_inode, struct file *_filep);
static int     fops__release  (struct inode *_inode, struct file *_filep);
static ssize_t fops__read     (struct file *_filep, char __user *_buf, size_t _count, loff_t *_pos);
static ssize_t fops__write    (struct file *_filep, const char __user *_buf, size_t _count, loff_t *_pos);
static long    fops__unlocked_ioctl (struct file *_filep, unsigned int _cmd, unsigned long _arg);
static int     fops__mmap     (struct file *_filep, struct vm_area_struct *_vma);
static unsigned int fops__poll (struct file *_filep, struct poll_table_struct *_pts);

#define FOP(a) .a = fops__##a

static struct file_operations fops = {
   .owner = THIS_MODULE,

   FOP(open),
   FOP(release),
   FOP(read),
   FOP(write),
   FOP(unlocked_ioctl),
   FOP(mmap),
   FOP(poll)
};

/* (note) may temporarily exceed C64_DEV_MAX_CLIENTS */
static atomic_t at_num_open = ATOMIC_INIT(0);


/*--------------------------------------------------------------------------- fops__open */
int fops__open(struct inode *_inode, struct file *_filep) {
   int ret;

   if(b_dev_enabled)
   {
      mutex_lock(&mtx_clients);
      
      if(C64_DEV_MINOR == iminor(_inode))
      {
         if(atomic_inc_and_test(&at_num_open) < C64_DEV_MAX_CLIENTS)
         {
            dev_client_t *cl = loc_find_client_by_filep(_filep);
            
            if(NULL == cl)
            {
               /* Find first unused client */
               sUI i;
               cl = clients;
               
               for(i=0; i<C64_DEV_MAX_CLIENTS; i++)
               {
                  if(STATE_UNUSED == atomic_read(&cl->at_state))
                  {
                     /* Found unused client struct */
                     init_waitqueue_head(&cl->wq_read); /* (todo) do we have to free this and if yes, how? */
                     cl->filep = _filep;
                     atomic_set(&cl->at_state, STATE_IDLE);
                     break;
                  }
                  
                  /* Try next client struct */
                  cl++;
               }
               
               if(C64_DEV_MAX_CLIENTS != i)
               {
                  /* Succeeded */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_DEBUG "c64::dev::open: client connected (filep=0x%p).\n", _filep);
                  }
                  
                  ret = 0;
               }
               else
               {
                  /* Failed: all clients in use */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::open: all clients in use.\n");
                  }
                  
                  ret = -EMFILE;
               }
            }
            else
            {
               /* Failed: client not found (internal error) */
               printk(KERN_ERR "c64::dev::open: internal error: client (filep=0x%p) already exists.\n", _filep);
               
               ret = -EIO;
            }
         }
         else
         {
            /* Failed: maximum number of file handles exceeded */
            atomic_dec(&at_num_open);
            
            ret = -EMFILE;
         }
      }
      else
      {
         /* Failed: wrong minor device number */
         ret = -ENODEV;
         
      }

      mutex_unlock(&mtx_clients);
   }
   else
   {
      /* Failed: kernel module is still initializing */
      ret = -EAGAIN;
   }

   return ret;
}


/*--------------------------------------------------------------------------- fops__release */
int fops__release(struct inode *_inode, struct file *_filep) {
   int ret;
   dev_client_t *cl;
   sU32 clientIdx;

   mutex_lock(&mtx_clients);

   atomic_dec(&at_num_open);

   cl = loc_find_client_by_filep(_filep);

   if(NULL != cl)
   {
      atomic_set(&cl->at_state, STATE_UNUSED);

      clientIdx = (sU32) (cl - clients);
      client_powerstate_mask &= ~(1u << clientIdx);

      if(cl == current_client)
      {

         /* Wait for / terminate last fastcall (if any) */
         mlb_fastcall_end();

         current_client = NULL;
      }

      /* Free shared memory in case client forgot to */
      {
         dsp_cache_t type;

         for(type = 0; type < NUM_DSP_CACHE_TYPES; type++)
         {
            if(0/*NULL*/ != cl->shm[type].dsp.phys_addr)
            {
               loc_shm_free(cl, type);
            }
         }
      }

      /* Free fast shared memory (L1DSRAM) in case client forgot to */
      minim_free_by_client(&l1sram_fshm_mm, cl);

      /* Free fast shared memory (L2SRAM) in case client forgot to */
      minim_free_by_client(&l2sram_fshm_mm, cl);

      cl->filep = NULL;

      cl->l1sram_fshm.phys_addr = 0;
      cl->l1sram_fshm.virt_addr = 0;
      cl->l1sram_fshm.size      = 0;

      cl->l2sram_fshm.phys_addr = 0;
      cl->l2sram_fshm.virt_addr = 0;
      cl->l2sram_fshm.size      = 0;

      cl->logbuf.phys_addr = 0;
      cl->logbuf.virt_addr = 0;
      cl->logbuf.size      = 0;

      /* (note) components registered by this client keep on being registered 
       *         and the areas keep being allocated, unless the client itself has 
       *         unregistered the components and freed the areas before exiting.
       */
      /* (note) this can be seen as a feature: an app. can simply install a component, then exit.
       *         OTOH, it can be seen as a bug since, if an app. crashes and accidentally
       *         leaves the areas allocated / components registered, these areas will be unavailable
       *         to other apps. A re-run of "go64.sh" (restart the DSP) will fix this, though.
       */
      /* (note) ideally, we would track what components and areas the client has registered / allocated,
       *         and clean up after the client at this point. Maybe this will be added later if this
       *         turns out to be a real issue. Maybe it can be fixed on the GPP side (e.g. atexit / signal handlers).
       */

      /* (note) ok, compromise: added an "emergency_unload" cmd. Uses polling, does not much error checking etc but better
       *         than nothing. Nevertheless, it is strongly recommended to unregister the overlay and its components
       *         using the libc64 API since calling mlb_send_to_dsp() here can cause a deadlock
       *         (w/ mtx_clients locked in write, currently removed) resp. a race condition w/ other writes.
       */

      {
         sUI secIdx;

         for(secIdx=0; secIdx< MLB_COM_NUM_SECTION_INDICES; secIdx++)
         {
            if(cl->b_overlay_section_allocflags[secIdx])
            {
               dsp_msg_t msg;
               sUI timeout = 0;

               DSP_MSG_INIT_CORE(&msg, CORE, CMD_COM_EMERGENCY_UNLOAD, secIdx, 0);

               printk(KERN_WARNING "c64::dev::release: client did not unregister overlay section #%u, calling CMD_COM_EMERGENCY_UNLOAD\n", secIdx);

               /* Power on DSP in case it's off */
               dsp_resume();

               while(!mlb_is_ready_to_send() && (timeout < 0x20000000u))
               {
                  timeout++;
               }

               if(mlb_is_ready_to_send())
               {
                  /* (note) the "cl" handle is not actually used by send(), resp. it is only used
                   *         to determine whether a testmsg shall be send (cl==NULL) or an RPC
                   */
                  mlb_send_to_dsp(cl, &msg);

                  printk(KERN_WARNING "c64::dev::release:  -> ok, message sent.\n");

                  /* Mark overlay section as unused */
                  b_overlay_section_allocflags[secIdx] = S_FALSE;
               }
               else
               {
                  printk(KERN_ERR "c64::dev::release  -> timeout (not ready to send). section %u is lost now.\n", secIdx);
               }

            }
         }
      }

      /* Reset client state */
      memset(cl->b_overlay_section_allocflags, 0, sizeof(sBool) * MLB_COM_NUM_SECTION_INDICES);

      memset(cl->overlay_section_mem, 0, sizeof(dsp_mem_region_t) * MLB_COM_NUM_SECTION_INDICES);

      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_DEBUG "c64::dev::release: client disconnected (filep=0x%p).\n", _filep);
      }

      if(0 == client_powerstate_mask)
      {
         sU32 timeout;

         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_DEBUG "c64::dev::release: powering down DSP.\n");
         }

         /* sync with DSP */
         timeout = 0;
         while(!mlb_is_ready_to_send() && (timeout < 0x20000000u))
         {
            timeout++;
         }
         
         if(mlb_is_ready_to_send())
         {
            if(c64_pwroff)
            {
               dsp_poweroff();
            }
            else if(c64_pwrlog)
            {
               printk(KERN_INFO "c64::dev::release: skipping dsp_poweroff() since 'pwroff' param is set to 0.\n");
            }
         }
         else
         {
            printk(KERN_WARNING "c64::dev::release: not powering down DSP because it is still busy (?!).\n");
         }
      }
      else
      {
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_DEBUG "c64::dev::ioctl[RELEASE]: NOT powering down DSP, client_powerstate_mask=0x%08x clIdx=%u\n",
                   client_powerstate_mask, clientIdx
                   );
         }
      }
   }
   else
   {
      /* Failed: client not found (internal error) */
      printk(KERN_ERR "c64::dev::release: internal error: client (filep=0x%p) not found.\n", _filep);
   }

   /* Succeeded */
   ret = 0;

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- fops__read */
ssize_t fops__read(struct file *_filep, char __user *_buf, size_t _count, loff_t *_pos) {
   ssize_t ret;

   (void)_pos;

   if(C64_DEBUG_LEVEL(30))
   {
      printk(KERN_DEBUG "c64::dev::read: ENTER  count=%u\n", _count);
   }

   if(sizeof(dsp_msg_t) == _count)
   {
      dev_client_t *cl = loc_find_client_by_filep(_filep);

      if(NULL != cl)
      {
         dev_client_state_t cState;

         if(wait_event_interruptible(cl->wq_read, 1))
         {
            /* interrupted by signal */
            return -ERESTARTSYS;
         }

         cState = atomic_read(&cl->at_state);

         if(STATE_REPLY_AVAIL == cState)
         {
            if(0 == copy_to_user(_buf, &cl->dsp_msg, sizeof(dsp_msg_t)))
            {
               /* Succeeded */
               if(C64_DEBUG_LEVEL(15))
               {
                  printk(KERN_DEBUG "c64::dev:read: DSP reply picked up by application.\n");
               }

               atomic_set(&cl->at_state, STATE_IDLE);

               ret = sizeof(dsp_msg_t);
            }
            else
            {
               /* Failed: copy_to_user() */
               printk(KERN_ERR "c64::dev::read: copy_to_user() failed.\n");

               return -EFAULT;
            }
         }
         else
         {
            if(STATE_REPLY_PENDING == cState)
            {
               /* Failed: no reply available, yet. */
               if(C64_DEBUG_LEVEL(30))
               {
                  printk(KERN_ERR "c64::dev::read: reply still pending, cState=%d.\n", cState);
               }

               ret = -EAGAIN;
            }
            else
            {
               /* Failed: (warning) nothing to read. */
               if(C64_DEBUG_LEVEL(15))
               {
                  printk(KERN_ERR "c64::dev::read: nothing to read (no pending reply).\n");
               }

               ret = 0;
            }
         }
      }
      else
      {
         /* Failed: client not found */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::dev::read: client (filep=0x%p) not found.\n", _filep);
         }

         ret = -EBADF;
      }
   }
   else if(4 == _count)
   {
      /* Read debug value */
      sU32 val = mlb_debug_read();

      if(0 == copy_to_user(_buf, &val, 4))
      {
         /* Succeeded */
         ret = 4;
      }
      else
      {
         /* Failed: copy_to_user() */
         return -EFAULT;
      }
   }
   else
   {
      /* Failed: invalid request size */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::read: invalid size %d.\n", _count);
      }

      ret = -EINVAL;
   }

   if(C64_DEBUG_LEVEL(30))
   {
      printk(KERN_DEBUG "c64::dev::read: LEAVE  ret=%d\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- fops__write */
ssize_t fops__write(struct file *_filep, const char __user *_buf, size_t _count, loff_t *_pos) {
   ssize_t ret;

   mutex_lock(&mtx_clients);

   if(sizeof(dsp_msg_t) == _count)
   {
      if(NULL == current_client)
      {
         dev_client_t *cl = loc_find_client_by_filep(_filep);
         
         if(NULL != cl)
         {
            if(0 == copy_from_user(&cl->dsp_msg, _buf, sizeof(dsp_msg_t)))
            {
               current_client = cl;
               
               atomic_set(&cl->at_state, STATE_REPLY_PENDING);

               ret = (ssize_t) mlb_send_to_dsp(cl, &cl->dsp_msg);
               
               if(0 == ret)
               {
                  /* Succeeded */
                  ret = sizeof(dsp_msg_t);
               }
               else
               {
                  /* Failed: mlb_send_to_dsp() */
                  if(C64_DEBUG_LEVEL(15))
                  {
                     printk(KERN_ERR "c64::dev::write: mlb_send_to_dsp() failed (still busy).\n");
                  }
                  
                  current_client = NULL;

                  atomic_set(&cl->at_state, STATE_IDLE);
                  
                  ret = -EAGAIN;
               }
            }
            else
            {
               /* Failed: copy_from_user() */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::write: copy_from_user() failed.\n");
               }
               
               ret = -EFAULT;
            }
         }
         else
         {
            /* Failed: client not found */
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::write: client (filep=0x%p) not found.\n", _filep);
            }
            
            ret = -EBADF;
         }
      }
      else
      {
         /* Failed: transmission still in progress (current_client != NULL) */
         if(C64_DEBUG_LEVEL(30))
         {
            printk(KERN_ERR "c64::dev::write: (current_client != NULL) (still busy).\n");
         }
         
         ret = -EAGAIN;
      }
   }
   else if(4 == _count)
   {
      /* Send test message to DSP, bypassing client handler */
      sU32 val;

      if(0 == copy_from_user(&val, _buf, 4))
      {
         current_client = NULL;

         ret = (ssize_t) mlb_send_to_dsp(NULL, (const dsp_msg_t*) &val);

         if(0 == ret)
         {
            ret = 4;
         }
         else
         {
            /* Failed: mlb_send_to_dsp() */
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::write: mlb_send_to_dsp() failed (testmsg).\n");
            }
         }
      }
      else
      {
         /* Failed: copy_from_user() */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::dev::write: copy_from_user() (4) failed.\n");
         }
         
         ret = -EFAULT;
      }
   }
   else
   {
      /* Failed: invalid request size */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::write: invalid size %d.\n", _count);
      }
      
      ret = -EINVAL;
   }

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- loc_ioctl_cache */
static long loc_ioctl_cache(dev_client_t *cl, unsigned long _arg) {
   long ret = 0;
   c64_dev_ioctl_cache_t req;
   dsp_cache_t type;
   sBool bDone = S_FALSE;

   mutex_lock(&mtx_clients);

   if(0 == copy_from_user(&req, (const void __user *)_arg, sizeof(c64_dev_ioctl_cache_t)))
   {
      if(C64_CACHE_AC_WBINVALL == req.action)
      {
         flush_cache_all();
         
         /* Succeeded */
         ret = 0;
         bDone = S_TRUE;
      }
      else
      {
         for(type=0; type < NUM_CONTIG_DSP_CACHE_TYPES; type++)
         {
            if(0/*NULL*/ != cl->shm[type].dsp.virt_addr)
            {
               if((req.phys_addr >= cl->shm[type].dsp.phys_addr)                            &&
                  (req.phys_addr <  (cl->shm[type].dsp.phys_addr + cl->shm[type].dsp.size)) && /* b.c. of possible 32bit overflow */
                  ((req.phys_addr + req.size) <= (cl->shm[type].dsp.phys_addr + cl->shm[type].dsp.size))
                  )
               {
                  switch(req.action)
                  {
                     default:
                        /* Failed: unsupported action */
                        if(C64_DEBUG_LEVEL(1))
                        {
                           printk(KERN_ERR "c64::dev::ioctl[CACHE]: unsupported action (%u).\n", req.action);
                        }
                           
                        ret = -EINVAL;
                        break;
                           
                     case C64_CACHE_AC_WB:

                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) (cl->shm[type].dsp.virt_addr + (req.phys_addr - cl->shm[type].dsp.phys_addr));
                           c64_v7_dma_map_area(startAddr, req.size, 1 /*DMA_TO_DEVICE*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                     
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE]: writeback (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.phys_addr,
                                  req.size
                                  );
                        }

                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                           
                     case C64_CACHE_AC_INV:
                        /* This is loosely borrowed from TI's CMEM (thanks!) */

                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) (cl->shm[type].dsp.virt_addr + (req.phys_addr - cl->shm[type].dsp.phys_addr));
                           c64_v7_dma_map_area(startAddr, req.size, 2 /*DMA_FROM_DEVICE*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                  
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE]: invalidate (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.phys_addr,
                                  req.size
                                  );
                        }
                     
                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                           
                     case C64_CACHE_AC_WBINV:
                        /* This is loosely borrowed from TI's CMEM (thanks!) */
                     
                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) (cl->shm[type].dsp.virt_addr + (req.phys_addr - cl->shm[type].dsp.phys_addr));
                           c64_v7_dma_map_area(startAddr, req.size, 0 /*DMA_BIDIRECTIONAL*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                        
                 
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE]: writeback (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.phys_addr,
                                  req.size
                                  );
                        }
                        
                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                  }
               
               }
               else
               {
                  /* Requested address not within current 'type' address range */
               }
            }
            else
            {
               /* Current 'type' address range not allocated */
            }

         } /* iterate types */

      } /* else CACHE_AC_WBINVALL */

      if(!bDone)
      {
         if(0 == ret)
         {
#if 0
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl[CACHE]: pls call C64_IOCTL_SHM_ALLOC and mmap() first.\n");
            }
#endif
            /* Failed: requested address range is outside of CMA block area */
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl: CACHE: requested phys_addr=0x%08x, size=0x%08x outside of CMA area.\n",
                      req.phys_addr,
                      req.size
                      );
            }
            
            ret = -EFAULT;
         }
      }
   }
   else
   {
      /* Failed: copy_from_user() */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::ioctl[CACHE]: copy_from_user() failed.\n");
      }
      
      ret = -EFAULT;
   }

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- loc_ioctl_cache_virt */
static long loc_ioctl_cache_virt(dev_client_t *cl, unsigned long _arg) {
   long ret = 0;
   c64_dev_ioctl_cache_virt_t req;
   dsp_cache_t type;
   sBool bDone = S_FALSE;

   mutex_lock(&mtx_clients);

   if(0 == copy_from_user(&req, (const void __user *)_arg, sizeof(c64_dev_ioctl_cache_virt_t)))
   {
      if(C64_CACHE_AC_WBINVALL == req.action)
      {
         flush_cache_all();
         
         /* Succeeded */
         ret = 0;
         bDone = S_TRUE;
      }
      else
      {
         for(type=0; type < NUM_DSP_CACHE_TYPES; type++)
         {
            if(0/*NULL*/ != cl->shm[type].dsp.virt_addr)
            {
               if((req.virt_addr >= cl->shm[type].dsp.virt_addr)                            &&
                  (req.virt_addr <  (cl->shm[type].dsp.virt_addr + cl->shm[type].dsp.size)) && /* b.c. of possible 32bit overflow */
                  ((req.virt_addr + req.size) <= (cl->shm[type].dsp.virt_addr + cl->shm[type].dsp.size))
                  )
               {
                  switch(req.action)
                  {
                     default:
                        /* Failed: unsupported action */
                        if(C64_DEBUG_LEVEL(1))
                        {
                           printk(KERN_ERR "c64::dev::ioctl[CACHE_VIRT]: unsupported action (%u).\n", req.action);
                        }
                           
                        ret = -EINVAL;
                        break;
                           
                     case C64_CACHE_AC_WB:

                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) req.virt_addr;

                           c64_v7_dma_map_area(startAddr, req.size, 1 /*DMA_TO_DEVICE*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                     
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE_VIRT]: cache writeback (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.virt_addr,
                                  req.size
                                  );
                        }

                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                           
                     case C64_CACHE_AC_INV:
                        /* This is loosely borrowed from TI's CMEM (thanks!) */

                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) req.virt_addr;

                           c64_v7_dma_map_area(startAddr, req.size, 2 /*DMA_FROM_DEVICE*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                  
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE_VIRT]: invalidate (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.virt_addr,
                                  req.size
                                  );
                        }
                     
                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                           
                     case C64_CACHE_AC_WBINV:
                        /* This is loosely borrowed from TI's CMEM (thanks!) */
                     
                        if(0 != cl->shm[type].dsp.virt_addr)
                        {
                           sU8 *startAddr = (sU8*) req.virt_addr;

                           c64_v7_dma_map_area(startAddr, req.size, 0 /*DMA_BIDIRECTIONAL*/);
                        }
                        else
                        {
                           /* Not mapped to userspace */
                           flush_cache_all();
                        }
                        
                 
                        if(C64_DEBUG_LEVEL(20))
                        {
                           printk(KERN_DEBUG "c64::dev::ioctl[CACHE_VIRT]: writeback (start=0x%08x size=0x%08x) succeeded.\n",
                                  req.virt_addr,
                                  req.size
                                  );
                        }
                        
                        /* Succeeded */
                        ret = 0;
                        bDone = S_TRUE;
                        break;
                  }
               
               }
               else
               {
                  /* Requested address not within current 'type' address range */
               }
            }
            else
            {
               /* Current 'type' address range not allocated */
            }

         } /* iterate types */

      } /* else CACHE_AC_WBINVALL */

      if(!bDone)
      {
         if(0 == ret)
         {
#if 0
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl[CACHE_VIRT]: pls call C64_IOCTL_SHM_ALLOC and mmap() first.\n");
            }
#endif
            /* Failed: requested address range is outside of CMA block area */
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_ERR "c64::dev::ioctl[CACHE_VIRT]: requested phys_addr=0x%08x, size=0x%08x outside of known areas.\n",
                      req.virt_addr,
                      req.size
                      );
            }
            
            ret = -EFAULT;
         }
      }
   }
   else
   {
      /* Failed: copy_from_user() */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::ioctl[CACHE_VIRT]: copy_from_user() failed.\n");
      }
      
      ret = -EFAULT;
   }

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- loc_ioctl_hugetlb_v2p() */
static long loc_ioctl_hugetlb_v2p(dev_client_t *cl, unsigned long _arg) {
   long ret = 0;
   sU32 req;

   mutex_lock(&mtx_clients);

   if(0 == copy_from_user(&req, (const void __user *)_arg, sizeof(sU32)))
   {
      req = loc_hugetlb_virt_to_phys(req);

      if(0 == copy_to_user((void __user *)_arg, &req, sizeof(sU32)))
      {
         /* Succeded */
      }
      else
      {
         /* Failed: copy_to_user() */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::dev::ioctl[HUGETLB_V2P]: copy_to_user() failed.\n");
         }

         ret = -EFAULT;
      }
   }
   else
   {
      /* Failed: copy_from_user() */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::ioctl[HUGETLB_V2P]: copy_from_user() failed.\n");
      }
      
      ret = -EFAULT;
   }

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- loc_ioctl_overlay_alloc() */
static long loc_ioctl_overlay_alloc(dev_client_t *cl, unsigned long _arg) {
   long ret;
   c64_dev_ioctl_overlay_alloc_t buf;

   mutex_lock(&mtx_clients);

   ret = 0;

   if(0 == copy_from_user(&buf, (const void __user *)_arg, sizeof(c64_dev_ioctl_overlay_alloc_t)))
   {
      if( (C64_OVERLAY_AREA2 == buf.overlay_area) || (C64_OVERLAY_AREA3 == buf.overlay_area) )
      {
         if(C64_OVERLAY_AREA2 == buf.overlay_area)
         {
            sUI secIdx = 0;

            while(secIdx < MLB_COM_SECTIONIDX_AREA3)
            {
               if(!b_overlay_section_allocflags[secIdx])
               {
                  /* Found unused overlay section */
                  break;
               }

               /* Try next area2 section */
               secIdx++;
            }

            if(MLB_COM_SECTIONIDX_AREA3 != secIdx)
            {
                   b_overlay_section_allocflags[secIdx] = S_TRUE;
               cl->b_overlay_section_allocflags[secIdx] = S_TRUE;

               cl->overlay_section_mem[secIdx] = overlay_sections[secIdx];

               /* ok, area2 section <secIdx> is now reserved for this client */
               buf.ret.section_idx = secIdx;
               buf.ret.mem         = overlay_sections[secIdx];

               if(C64_DEBUG_LEVEL(20))
               {
                  printk(KERN_DEBUG "c64::dev::ioctl: reserved area2 section %u @0x%08x,sz=0x%08x for client=0x%p\n",
                         buf.ret.section_idx,
                         buf.ret.mem.phys_addr,
                         buf.ret.mem.size,
                         cl->filep
                         );
               }
            }
            else
            {
               /* Failed: alloc area2 sections in use */
               ret = -ENOMEM;
            }
         }
         else
         {
            if(!b_overlay_section_allocflags[MLB_COM_SECTIONIDX_AREA3])
            {
                   b_overlay_section_allocflags[MLB_COM_SECTIONIDX_AREA3] = S_TRUE;
               cl->b_overlay_section_allocflags[MLB_COM_SECTIONIDX_AREA3] = S_TRUE;

               cl->overlay_section_mem[MLB_COM_SECTIONIDX_AREA3] = overlay_sections[MLB_COM_SECTIONIDX_AREA3];

               /* ok, area3 section is now reserved for this client */
               buf.ret.section_idx = MLB_COM_SECTIONIDX_AREA3;
               buf.ret.mem         = overlay_sections[MLB_COM_SECTIONIDX_AREA3];

               if(C64_DEBUG_LEVEL(20))
               {
                  printk(KERN_DEBUG "c64::dev::ioctl: reserved area3 section @0x%08x,sz=0x%08x for client filep=0x%p\n",
                         buf.ret.mem.phys_addr,
                         buf.ret.mem.size,
                         cl->filep
                         );
               }
            }
            else
            {
               /* Failed: area3 already in use */
               ret = -ENOMEM;
            }
         }

         if(0 == ret)
         {
            if(0 == copy_to_user((void __user *)_arg, &buf, sizeof(c64_dev_ioctl_overlay_alloc_t)))
            {
               /* Succeded */
            }
            else
            {
               /* Failed: copy_to_user() */
               ret = -EFAULT;
            }
         }
      }
      else
      {
         /* Failed: requested area is unkown */
         ret = -EINVAL;
      }
   }
   else
   {
      /* Failed: copy_from_user() */
      ret = -EFAULT;
   }
   
   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- loc_ioctl_overlay_free() */
static long loc_ioctl_overlay_free(dev_client_t *cl, unsigned long _arg) {
   long ret;
   sU32 secIdx = (sU32)_arg;

   mutex_lock(&mtx_clients);

   if(secIdx < MLB_COM_NUM_SECTION_INDICES)
   {
      if(cl->b_overlay_section_allocflags[secIdx])
      {
             b_overlay_section_allocflags[secIdx] = S_FALSE;
         cl->b_overlay_section_allocflags[secIdx] = S_FALSE;

         memset(&cl->overlay_section_mem[secIdx], 0, sizeof(dsp_mem_region_t));

         /* OK, overlay section is now free for other clients */
         ret = 0;
      }
      else
      {
         /* Failed: overlay section was not allocated by this client */
         ret = -EPERM;
      }
   }
   else
   {
      /* Failed: section index out of bounds */
      ret = -EINVAL;
   }

   mutex_unlock(&mtx_clients);

   return ret;
}


/*--------------------------------------------------------------------------- fops__unlocked_ioctl() */
long fops__unlocked_ioctl(struct file *_filep, unsigned int _cmd, unsigned long _arg) {
   long ret;
   dev_client_t *cl;

   cl = loc_find_client_by_filep(_filep);

   if(NULL != cl)
   {
      switch(_cmd)
      {
         default:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: illegal cmd=%u\n", _cmd);
            }

            ret = -EINVAL;
            break;
            
         case C64_IOCTL_SHM_ALLOC:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=SHM_ALLOC\n");
            }

            {
               c64_dev_ioctl_shm_alloc_t req;

               if(0 == copy_from_user(&req, (const void __user *)_arg, sizeof(c64_dev_ioctl_shm_alloc_t)))
               {
                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::ioctl: SHM_ALLOC req.type=%d\n", req.type);
                  }

                  ret = loc_shm_alloc(cl, &req);

                  if(0 == ret)
                  {
                     /* Copy shm_alloc struct */
                     req.mem = cl->shm[req.type].dsp;

                     if(0 == copy_to_user((void __user *)_arg, &req, sizeof(c64_dev_ioctl_shm_alloc_t)))
                     {
                        /* Succeeded */
                     }
                     else
                     {
                        /* Failed: copy_to_user() */
                        loc_shm_free(cl, req.type);
                        
                        ret = -EFAULT;
                     }
                  }
               }
               else
               {
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl: SHM_ALLOC: copy_from_user() failed.\n");
                  }
               
                  ret = -EFAULT;
               }
            }

            break;
            
         case C64_IOCTL_SHM_FREE:
            {
               dsp_cache_t type = (dsp_cache_t)_arg;

               if(C64_DEBUG_LEVEL(10))
               {
                  printk(KERN_DEBUG "c64::dev::ioctl: cmd=SHM_FREE type=%d\n", type);
               }

               ret = loc_shm_free(cl, type);
            }

            break;

         case C64_IOCTL_TEST:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=TEST\n");
            }

#if 1
            /* Try to write to L1DSRAM (works) */
            printk(KERN_DEBUG "c64::dev::ioctl: write L1DSRAM:\n");
            reg32_write(0x5cf04000u, 0x12345678u);

            printk(KERN_DEBUG "c64::dev::ioctl: read L1DSRAM:\n");
            printk(KERN_DEBUG "  ==> val=0x%08x\n", reg32_read(0x5cf04000u));
#endif

            ret = 0;
            break;

         case C64_IOCTL_CACHE:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=CACHE\n");
            }

            ret = loc_ioctl_cache(cl, _arg);

            break;

         case C64_IOCTL_DEBUG_USR:
         {
            c64_dev_ioctl_debug_usr_t req;

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=DEBUG_USR\n");
            }

            if(0 == copy_from_user(&req, (const void __user *)_arg, sizeof(c64_dev_ioctl_debug_usr_t)))
            {
               sUI idx = req.idx;

               if(idx < 4u)
               {
                  req.val = mlb_debug_usr_read(idx);

                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::ioctl[DEBUG_USR]: read val[%u]=0x%08x\n",
                            idx,
                            req.val
                            );

                  }

                  if(0 == copy_to_user((void __user *)_arg, &req, sizeof(c64_dev_ioctl_debug_usr_t)))
                  {
                     /* Succeeded */
                     ret = 0;
                  }
                  else
                  {
                     /* Failed: copy_to_user() */
                     printk(KERN_ERR "c64::dev::ioctl[DEBUG_USR]: copy_to_user() failed.\n");
                     
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: debug index out of range (0..3) */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl[DEBUG_USR]: invalid value idx (%u)\n", idx);
                  }

                  ret = -EINVAL;
               }
            }
            else
            {
               /* Failed: copy_from_user() */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::ioctl[DEBUG_USR]: copy_from_user() failed.\n");
               }
               
               ret = -EFAULT;
            }
         }
         break;

         case C64_IOCTL_PWR_ON_RST:

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=PWR_ON_RST\n");
            }

            mutex_lock(&mtx_clients);

            if(1 == atomic_read(&at_num_open))
            {
               ret = dsp_poweron_and_reset();
            }
            else
            {
               /* Failed: more than one client connected */
               ret = -EBUSY;
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_START:

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=START\n");
            }

            mutex_lock(&mtx_clients);

            if(1 == atomic_read(&at_num_open))
            {
               sU32 entryPoint;

               entryPoint = (sU32)_arg;

               printk(KERN_DEBUG "c64::dev::ioctl[START]: starting DSP @0x%08x\n", entryPoint);

               ret = dsp_start(entryPoint);
            }
            else
            {
               /* Failed: more than one client connected */
               ret = -EBUSY;
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_PWR_OFF:

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=PWR_OFF\n");
            }

            mutex_lock(&mtx_clients);

            {
               sU32 clientIdx = (sU32) (cl - clients);

               client_powerstate_mask &= ~(1u << clientIdx);

               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_DEBUG "c64::dev::ioctl: client_powerstate_mask=0x%08x\n", client_powerstate_mask);
               }
            }

            if(0 == client_powerstate_mask)
            {
               if(c64_pwroff)
               {
                  ret = dsp_poweroff();
               }
               else
               {
                  if(c64_pwrlog)
                  {
                     printk(KERN_INFO "c64::dev::ioctl: skipping dsp_poweroff() since 'pwroff' param is set to 0.\n");
                  }

                  ret = 0;
               }
            }
            else
            {
               /* Failed: at least one client is still using the DSP */
               ret = -EBUSY;
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_RESUME:

            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=RESUME\n");
            }

            mutex_lock(&mtx_clients);

            {
               sU32 clientIdx = (sU32) (cl - clients);

               client_powerstate_mask |= (1u << clientIdx);
            }

            ret = dsp_resume();

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_OVERLAY_ALLOC:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=OVERLAY_ALLOC\n");
            }

            ret = loc_ioctl_overlay_alloc(cl, _arg /* c64_dev_ioctl_overlay_alloc_t* */);

            break;

         case C64_IOCTL_OVERLAY_FREE:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=OVERLAY_FREE\n");
            }

            ret = loc_ioctl_overlay_free(cl, (sU32)_arg /* MLB_COM_SECTIONIDX_xxx */);

            break;

         case C64_IOCTL_OVERLAY_REF:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=OVERLAY_REF\n");
            }
            
            {
               sU32 secIdx = _arg;

               /* (note) called when an overlay was not loaded by an app but still used by it.
                *         (refcount increased)
                */

               if(secIdx < MLB_COM_NUM_SECTION_INDICES)
               {
                  cl->b_overlay_section_allocflags[secIdx] = S_TRUE;

                  /* Succeeded */
                  ret = 0;
               }
               else
               {
                  /* Failed: section index out of bounds */
                  ret = -EINVAL;
               }
            }
            break;

         case C64_IOCTL_L1SRAM_ALLOC:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L1SRAM_ALLOC\n");
            }

            mutex_lock(&mtx_clients);

            {
               dsp_mem_region_t mem;

               if(0 == copy_from_user(&mem, (const void __user *)_arg, sizeof(dsp_mem_region_t)))
               {
                  mem = minim_alloc(&l1sram_fshm_mm, cl, mem.size);

                  if(0 == copy_to_user((void __user *)_arg, &mem, sizeof(dsp_mem_region_t)))
                  {
                     /* Succeeded */
                     ret = 0;
                  }
                  else
                  {
                     /* Failed: copy_to_user() */
                     printk(KERN_ERR "c64::dev::ioctl[L1SRAM_ALLOC]: copy_to_user() failed.\n");
                     
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: copy_from_user() */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl[L1SRAM_ALLOC]: copy_from_user() failed.\n");
                  }
                  
                  ret = -EFAULT;
               }
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_L1SRAM_FREE:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L1SRAM_FREE\n");
            }

            mutex_lock(&mtx_clients);

            {
               dsp_mem_region_t mem;

               if(0 == copy_from_user(&mem, (const void __user *)_arg, sizeof(dsp_mem_region_t)))
               {
                  ret = minim_free(&l1sram_fshm_mm, cl, mem);
               }
               else
               {
                  /* Failed: copy_from_user() */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl[L1SRAM_FREE]: copy_from_user() failed.\n");
                  }
                  
                  ret = -EFAULT;
               }
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_L1SRAM_DEBUG:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L1SRAM_DEBUG\n");
            }

            mutex_lock(&mtx_clients);

            minim_debug(&l1sram_fshm_mm, (sUI)_arg, "c64::dev::ioctl[L1SRAM_DEBUG]");

            mutex_unlock(&mtx_clients);

            /* Succeeded */
            ret = 0;

            break;


         case C64_IOCTL_CACHE_VIRT:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=CACHE_VIRT\n");
            }

            ret = loc_ioctl_cache_virt(cl, _arg);

            break;


         case C64_IOCTL_HUGETLB_V2P:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=HUGETLB_V2P\n");
            }

            ret = loc_ioctl_hugetlb_v2p(cl, _arg);

            break;


         case C64_IOCTL_L2SRAM_ALLOC:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L2SRAM_ALLOC\n");
            }

            mutex_lock(&mtx_clients);

            {
               dsp_mem_region_t mem;

               if(0 == copy_from_user(&mem, (const void __user *)_arg, sizeof(dsp_mem_region_t)))
               {
                  mem = minim_alloc(&l2sram_fshm_mm, cl, mem.size);

                  if(0 == copy_to_user((void __user *)_arg, &mem, sizeof(dsp_mem_region_t)))
                  {
                     /* Succeeded */
                     ret = 0;
                  }
                  else
                  {
                     /* Failed: copy_to_user() */
                     printk(KERN_ERR "c64::dev::ioctl[L2SRAM_ALLOC]: copy_to_user() failed.\n");
                     
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: copy_from_user() */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl[L2SRAM_ALLOC]: copy_from_user() failed.\n");
                  }
                  
                  ret = -EFAULT;
               }
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_L2SRAM_FREE:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L2SRAM_FREE\n");
            }

            mutex_lock(&mtx_clients);

            {
               dsp_mem_region_t mem;

               if(0 == copy_from_user(&mem, (const void __user *)_arg, sizeof(dsp_mem_region_t)))
               {
                  ret = minim_free(&l2sram_fshm_mm, cl, mem);
               }
               else
               {
                  /* Failed: copy_from_user() */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::ioctl[L2SRAM_FREE]: copy_from_user() failed.\n");
                  }
                  
                  ret = -EFAULT;
               }
            }

            mutex_unlock(&mtx_clients);

            break;

         case C64_IOCTL_L2SRAM_DEBUG:
            if(C64_DEBUG_LEVEL(10))
            {
               printk(KERN_DEBUG "c64::dev::ioctl: cmd=L2SRAM_DEBUG\n");
            }

            mutex_lock(&mtx_clients);

            minim_debug(&l2sram_fshm_mm, (sUI)_arg, "c64::dev::ioctl[L2SRAM_DEBUG]");

            mutex_unlock(&mtx_clients);

            /* Succeeded */
            ret = 0;

            break;


      } /* switch(_cmd) */
   }
   else
   {
      /* Failed: client not found ?!?? */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::ioctl: client (filep=0x%p) not found.\n", _filep);
      }

      ret = -EBADF;
   }

   if(C64_DEBUG_LEVEL(10))
   {
      printk(KERN_ERR "c64::dev::ioctl: LEAVE ret=%d\n", (sSI)ret);
   }
      
   return ret;
}


/*--------------------------------------------------------------------------- fops__mmap() */
static int fops__mmap(struct file *_filep, struct vm_area_struct *_vma) {
   /*
    * (note) I had a peek at how TI's CMEM kernel module (GPL) implements this
    *         (saved me some time, thanks guys'n'gals!)
    *
    * This <http://www.cs.binghamton.edu/~ghose/CS529/sharedmemmap.htm> also has some nice explanations.
    *
    */
   dev_client_t *cl;
   size_t reqSize;
   int ret = 0;
   sBool bMapped = S_FALSE;

   cl = loc_find_client_by_filep(_filep);

   if(NULL != cl)
   {
      sU32 reqPhysAddr;
      dsp_cache_t type;

#if 0
      {
         /* test code: */
         unsigned long virt;

         virt = do_mmap(NULL, 0, /* file and addr */
                        2*1024*1024,
                        (PROT_READ | PROT_WRITE),
                        (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB),
                        0 /* offset */
                        );

         printk("xxx c64: mmap: virt=0x%08x MAP_HUGETLB=0x%08x\n", virt, MAP_HUGETLB);

         if(virt > 0)
         {
            /* xxx why is the VM_HUGETLB flag not set ??
             *
             *  (=> according to notaz, the MAP_HUGETLB flag is handled only by
             *       the linux userland syscall interface.)
             */
            
            u32 phys = loc_hugetlb_virt_to_phys((u32)virt);

            printk("xxx c64: mmap: phys=0x%08x\n", phys);

            do_munmap(current->mm, virt, 2*1024*1024);
         }
      }
#endif
      
      reqPhysAddr = (sU32) (_vma->vm_pgoff << PAGE_SHIFT);
      
      for(type=0; type < NUM_CONTIG_DSP_CACHE_TYPES; type++)
      {
         if(reqPhysAddr == cl->shm[type].dsp.phys_addr)
         {
            if(0/*NULL*/ == cl->shm[type].dsp.virt_addr)
            {
               reqSize = (_vma->vm_end - _vma->vm_start);
            
               if(reqSize <= cl->shm[type].dsp.size)
               {
                  /* (note) map CACHED memory. applications must use dsp_cache_*() functions to maintain coherency */

                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::mmap: shm OLD vm_page_prot=0x%08x\n", _vma->vm_page_prot);
                  }

                  /* (note) for pgprot*() see arch/arm/include/asm/pgtable.h */

                  switch(type)
                  {
                     default:
                     case DSP_CACHE_NONE:
                        if(C64_DEBUG_LEVEL(10))
                        {
                           printk(KERN_DEBUG "c64::dev::mmap: shm DSP_CACHE_NONE\n");
                        }

                        _vma->vm_page_prot = pgprot_noncached(_vma->vm_page_prot);
                        break;

                     case DSP_CACHE_RW:
                        if(C64_DEBUG_LEVEL(10))
                        {
                           printk(KERN_DEBUG "c64::dev::mmap: shm DSP_CACHE_RW\n");
                        }
                        break;

                     case DSP_CACHE_R:
                        if(C64_DEBUG_LEVEL(10))
                        {
                           printk(KERN_DEBUG "c64::dev::mmap: shm DSP_CACHE_R\n");
                        }

                        _vma->vm_page_prot = pgprot_writethrough(_vma->vm_page_prot);
                        break;

                     case DSP_CACHE_W:
                        if(C64_DEBUG_LEVEL(10))
                        {
                           printk(KERN_DEBUG "c64::dev::mmap: shm DSP_CACHE_W\n");
                        }

                        _vma->vm_page_prot = pgprot_writecombine(_vma->vm_page_prot);
                        break;
                  }

                  if(C64_DEBUG_LEVEL(10))
                  {
                     printk(KERN_DEBUG "c64::dev::mmap: shm NEW vm_page_prot=0x%08x\n", _vma->vm_page_prot);
                  }
              
                  _vma->vm_flags |= (VM_RESERVED | VM_IO);

                  /* Remap kernel memory to userspace */
                  ret = remap_pfn_range(_vma,
                                        _vma->vm_start,
                                        _vma->vm_pgoff,
                                        cl->shm[type].dsp.size,
                                        _vma->vm_page_prot
                                        );

                  if(0 == ret)
                  {
                     cl->shm[type].dsp.virt_addr = (sU32)_vma->vm_start;
                  
                     if(C64_DEBUG_LEVEL(5))
                     {
                        printk(KERN_DEBUG "c64::dev::map: OK, mapped CMA block to user virt_addr=0x%08x.\n",
                               cl->shm[type].dsp.virt_addr
                               );
                     }

                     /* Succeeded */
                     bMapped = S_TRUE;
                  }
                  else
                  {
                     /* Failed: remap_pfn_range() */
                     if(C64_DEBUG_LEVEL(1))
                     {
                        printk(KERN_ERR "c64::dev::map: failed to map CMA block @0x%08x to user virtual address.\n",
                               (sU32)reqPhysAddr
                               );
                     }
                  
                     ret = -EFAULT;
                  }

               }
               else
               {
                  /* Failed: size exceeds CMA block size */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::mmap: req. size (%u) exceeds CMA block size (%u).\n",
                            reqSize,
                            cl->shm[type].dsp.size
                            );
                  }
               
                  ret = -EINVAL;
               }
            }
            else
            {
               /* Failed: CMA block is already mapped to user virtual address */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::mmap: CMA block already mapped to user virt_addr=0x%08x.\n",
                         cl->shm[type].dsp.virt_addr
                         );
               }
            
               ret = -EEXIST;
            }

            break;
         }
         else
         {
            /* reqAddr does not match shm area */
         }
      } /* iterate shm types */

      if( (0 == ret) && !bMapped)
      {
         if(DSP_L1SRAM_FSHM_BASE_GPP == reqPhysAddr)
         {
            if(0/*NULL*/ == cl->l1sram_fshm.virt_addr)
            {
               reqSize = (_vma->vm_end - _vma->vm_start);
            
               if(reqSize <= DSP_L1SRAM_FSHM_SIZE)
               {
                  /* Map uncached memory */
                  _vma->vm_page_prot = pgprot_stronglyordered(_vma->vm_page_prot);
               
                  _vma->vm_flags |= (VM_RESERVED | VM_IO);
               
                  /* Remap kernel memory to userspace */
                  ret = remap_pfn_range(_vma,
                                        _vma->vm_start,
                                        _vma->vm_pgoff,
                                        DSP_L1SRAM_FSHM_SIZE,
                                        _vma->vm_page_prot
                                        );
                  if(0 == ret)
                  {
                     cl->l1sram_fshm.phys_addr = (sU32)reqPhysAddr;
                     cl->l1sram_fshm.virt_addr = (sU32)_vma->vm_start;
                  
                     if(C64_DEBUG_LEVEL(5))
                     {
                        printk(KERN_DEBUG "c64::dev::map: OK, mapped L1SRAM FSHM area to user virt_addr=0x%08x.\n",
                               cl->l1sram_fshm.virt_addr
                               );
                     }

                     /* Succeeded */
                     bMapped = S_TRUE;
                  }
                  else
                  {
                     /* Failed: remap_pfn_range() */
                     if(C64_DEBUG_LEVEL(1))
                     {
                        printk(KERN_ERR "c64::dev::map: failed to map L1SRAM FSHM area @0x%08x to user virtual address.\n",
                               (sU32)reqPhysAddr
                               );
                     }
                  
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: size exceeds L1SRAM FSHM area size */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::mmap: req. size (%u) exceeds L1SRAM FSHM area size (%u).\n",
                            reqSize,
                            DSP_L1SRAM_FSHM_SIZE
                            );
                  }
               
                  ret = -EINVAL;
               }
            }
            else
            {
               /* Failed: L1SRAM FSHM area is already mapped to user virtual address */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::mmap: L1SRAM FSHM area already mapped to user virt_addr=0x%08x.\n",
                         cl->l1sram_fshm.virt_addr
                         );
               }
            
               ret = -EEXIST;
            }
         }
         else if(DSP_L2SRAM_FSHM_BASE_GPP == reqPhysAddr)
         {
            if(0/*NULL*/ == cl->l2sram_fshm.virt_addr)
            {
               reqSize = (_vma->vm_end - _vma->vm_start);
            
               if(reqSize <= DSP_L2SRAM_FSHM_SIZE)
               {
                  /* Map uncached memory */

                  _vma->vm_page_prot = pgprot_stronglyordered(_vma->vm_page_prot);
               
                  _vma->vm_flags |= (VM_RESERVED | VM_IO);
               
                  /* Remap kernel memory to userspace */
                  ret = remap_pfn_range(_vma,
                                        _vma->vm_start,
                                        _vma->vm_pgoff,
                                        DSP_L2SRAM_FSHM_SIZE,
                                        _vma->vm_page_prot
                                        );
                  if(0 == ret)
                  {
                     cl->l2sram_fshm.phys_addr = (sU32)reqPhysAddr;
                     cl->l2sram_fshm.virt_addr = (sU32)_vma->vm_start;
                  
                     if(C64_DEBUG_LEVEL(5))
                     {
                        printk(KERN_DEBUG "c64::dev::map: OK, mapped L2SRAM FSHM area to user virt_addr=0x%08x.\n",
                               cl->l2sram_fshm.virt_addr
                               );
                     }

                     /* Succeeded */
                     bMapped = S_TRUE;
                  }
                  else
                  {
                     /* Failed: remap_pfn_range() */
                     if(C64_DEBUG_LEVEL(1))
                     {
                        printk(KERN_ERR "c64::dev::map: failed to map L2SRAM FSHM area @0x%08x to user virtual address.\n",
                               (sU32)reqPhysAddr
                               );
                     }
                  
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: size exceeds L2SRAM FSHM area size */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::mmap: req. size (%u) exceeds L2SRAM FSHM area size (%u).\n",
                            reqSize,
                            DSP_L2SRAM_FSHM_SIZE
                            );
                  }
               
                  ret = -EINVAL;
               }
            }
            else
            {
               /* Failed: L2SRAM FSHM area is already mapped to user virtual address */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::mmap: L2SRAM FSHM area already mapped to user virt_addr=0x%08x.\n",
                         cl->l2sram_fshm.virt_addr
                         );
               }
            
               ret = -EEXIST;
            }
         }
         else if(DSP_LOGBUF_BASE_GPP == reqPhysAddr)
         {
            if(0/*NULL*/ == cl->logbuf.virt_addr)
            {
               reqSize = (_vma->vm_end - _vma->vm_start);
            
               if(reqSize <= DSP_LOGBUF_SIZE)
               {
                  /* Map uncached memory */

                  _vma->vm_page_prot = pgprot_noncached(_vma->vm_page_prot);
               
                  _vma->vm_flags |= (VM_RESERVED | VM_IO);
               
                  /* Remap kernel memory to userspace */
                  ret = remap_pfn_range(_vma,
                                        _vma->vm_start,
                                        _vma->vm_pgoff,
                                        DSP_LOGBUF_SIZE,
                                        _vma->vm_page_prot
                                        );
                  if(0 == ret)
                  {
                     cl->logbuf.phys_addr = (sU32)reqPhysAddr;
                     cl->logbuf.virt_addr = (sU32)_vma->vm_start;
                  
                     if(C64_DEBUG_LEVEL(5))
                     {
                        printk(KERN_DEBUG "c64::dev::map: OK, mapped logbuf area to user virt_addr=0x%08x.\n",
                               cl->logbuf.virt_addr
                               );
                     }

                     /* Succeeded */
                     bMapped = S_TRUE;
                  }
                  else
                  {
                     /* Failed: remap_pfn_range() */
                     if(C64_DEBUG_LEVEL(1))
                     {
                        printk(KERN_ERR "c64::dev::map: failed to map logbuf area @0x%08x to user virtual address.\n",
                               (sU32)reqPhysAddr
                               );
                     }
                  
                     ret = -EFAULT;
                  }
               }
               else
               {
                  /* Failed: size exceeds logbuf size */
                  if(C64_DEBUG_LEVEL(1))
                  {
                     printk(KERN_ERR "c64::dev::mmap: req. size (%u) exceeds logbuf size (%u).\n",
                            reqSize,
                            DSP_LOGBUF_SIZE
                            );
                  }
               
                  ret = -EINVAL;
               }
            }
            else
            {
               /* Failed: logbuf area is already mapped to user virtual address */
               if(C64_DEBUG_LEVEL(1))
               {
                  printk(KERN_ERR "c64::dev::mmap: logbuf area already mapped to user virt_addr=0x%08x.\n",
                         cl->logbuf.virt_addr
                         );
               }
            
               ret = -EEXIST;
            }
         }
         else
         {
            /* Try overlay code area */
            sUI secIdx;

            reqSize = (_vma->vm_end - _vma->vm_start);

            for(secIdx=0; secIdx < MLB_COM_NUM_SECTION_INDICES; secIdx++)
            {
               if(cl->b_overlay_section_allocflags[secIdx])
               {
                  if( (reqPhysAddr == cl->overlay_section_mem[secIdx].phys_addr) &&
                      (reqSize == cl->overlay_section_mem[secIdx].size)
                      )
                  {
                     /* Map write-through (read cached) memory */
                     _vma->vm_page_prot = pgprot_writethrough(_vma->vm_page_prot);
                  
                     _vma->vm_flags |= (VM_RESERVED | VM_IO);
                  
                     /* Remap kernel memory to userspace */
                     ret = remap_pfn_range(_vma,
                                           _vma->vm_start,
                                           _vma->vm_pgoff,
                                           reqSize,
                                           _vma->vm_page_prot
                                           );
                     if(0 == ret)
                     {
                        if(C64_DEBUG_LEVEL(5))
                        {
                           printk(KERN_DEBUG "c64::dev::map: OK, mapped DSP code area to user virt_addr=0x%08x.\n",
                                  (sU32)_vma->vm_start
                                  );
                        }

                        bMapped = S_TRUE;

                        /* Succeeded */
                     }
                     else
                     {
                        /* Failed: remap_pfn_range() */
                        if(C64_DEBUG_LEVEL(1))
                        {
                           printk(KERN_ERR "c64::dev::map: failed to map DSP code area @0x%08x to user virtual address.\n",
                                  (sU32)reqPhysAddr
                                  );
                        }
                     
                        ret = -EFAULT;
                     }

                     break;
                  }
               }
            }
         }
      }

      if( (0 == ret) && !bMapped)
      {
         /* Failed: requested physical addr does not equal CMA_SHM/FSHM/OVERLAY block start */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::dev::mmap: req. physaddr (0x%08x) does not equal SHM/FSHM/OVL block starts.\n",
                   (sU32)reqPhysAddr
                   );
         }
         
         ret = -EINVAL;
      }
   }
   else
   {
      /* Failed: client not found ?!?? */
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_ERR "c64::dev::mmap: client (filep=0x%p) not found.\n", _filep);
      }

      ret = -EBADF;
   }

   return ret;
}


/*--------------------------------------------------------------------------- fops__poll() */
unsigned int fops__poll (struct file *_filep, struct poll_table_struct *_pts) {
   unsigned int ret; /* mask */

   dev_client_t *cl = loc_find_client_by_filep(_filep);

   if(NULL != cl)
   {
      if(NULL != _pts)
      {
         poll_wait(_filep, &cl->wq_read, _pts);
      }

      if(STATE_REPLY_AVAIL == atomic_read(&cl->at_state))
      {
         ret = POLLIN | POLLRDNORM;
      }
      else
      {
         ret = 0;
      }

      /* (todo) */
      ret |= POLLOUT | POLLWRNORM;
   }
   else
   {
      ret = POLLERR;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dev_handle_reply() */
void dev_handle_reply(sU32 _ret1, sU32 _ret2) {

   /* (note) currently called directly from mlb IRQ handler */
   
   if(NULL != current_client)
   {
      /* Integrity check: state */
      dev_client_state_t cState = (dev_client_state_t) atomic_read(&current_client->at_state);

      if(C64_DEBUG_LEVEL(10))
      {
         printk(KERN_DEBUG "c64::dev_handle_reply:: recv'd msg. client=0x%08x dspCompId=0x%04x dspCmdId=0x%04x ret1=0x%08x ret2=0x%08x\n", 
                (sU32)current_client,
                current_client->dsp_msg.req.fields.dsp_component_id,
                current_client->dsp_msg.req.fields.dsp_cmd_id,
                _ret1,
                _ret2
                );
      }

      if(STATE_REPLY_PENDING == cState)
      {
         /* Copy return values to client message */
         current_client->dsp_msg.data[0].ret = _ret1;
         current_client->dsp_msg.data[1].ret = _ret2;

         current_client->dsp_errno = mlb_dsp_errno();
         
         atomic_set(&current_client->at_state, STATE_REPLY_AVAIL);

         wake_up_interruptible((wait_queue_head_t*)&current_client->wq_read);
         
         /* (note) another client may already be exchanging a message when
          *         this client picks up the DSP reply.
          */
         current_client = NULL;
      }
      else
      {
         /* (note) in order to prevent kernel log spam in case sth goes terribly wrong,
          *         let's just print the error message in debug mode.
          */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_ERR "c64::dev_handle_reply: unexpected client state %d.\n",
                   cState
                   );
         }
      }
   }
   else
   {
      /* received simple test message */

      if(C64_DEBUG_LEVEL(10))
      {
         printk(KERN_DEBUG "c64::dev_handle_reply:: recv'd test msg. ret1=0x%08x ret2=0x%08x\n", 
                _ret1,
                _ret2
                );
      }

      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_DEBUG "c64::dev_handle_reply: no current client.\n");
      }
   }
   
}


/*--------------------------------------------------------------------------- dev_iva2_clk_set_enable() */
void dev_iva2_clk_set_enable(sBool _bEnabled) {
   if(_bEnabled)
   {
      clk_enable(iva2_clk);
   }
   else
   {
      clk_disable(iva2_clk);
   }
}


/*--------------------------------------------------------------------------- dev_enable() */
void dev_enable(void) {
   b_dev_enabled = S_TRUE;
}


/*--------------------------------------------------------------------------- dev_init() */
sBool dev_init(void) {
   sBool ret;
   int r;

   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::dev_init: ENTER\n");
   }

   current_client = NULL;

   b_dev_enabled = S_FALSE;

   /* Register char device */
   {
#if 0
      c64_dev_nr = MKDEV(C64_DEV_MAJOR, C64_DEV_MINOR);
#endif

      r = alloc_chrdev_region(&c64_dev_nr, C64_DEV_MINOR, 1, C64_DEV_NAME);

      if(r >= 0)
      {
         c64_cdev = cdev_alloc();

         if(NULL != c64_cdev)
         {
            /* Setup device */
            c64_cdev->owner = THIS_MODULE;

            cdev_init(c64_cdev, &fops);
            
            c64_class = class_create(THIS_MODULE, "c64");

            if(! (IS_ERR(c64_class)))
            {
               c64_device = device_create(c64_class,
                                          NULL,       /* parent */
                                          c64_dev_nr,
                                          NULL,       /* drvdata */
                                          C64_DEV_NAME /*"%d",
                                                         0*/
                                          );

               if(NULL != c64_device)
               {
                  {
                     /* Initialize clients (zerofill + init atomics) */
                     loc_init_clients();

                     if(C64_DEBUG_LEVEL(20))
                     {
                        printk(KERN_DEBUG "c64::dev::init: calling minim_init() (L1SRAM)\n");
                     }

                     /* Initialize L1DSRAM FSHM allocator */
                     minim_init(&l1sram_fshm_mm,
                                l1sram_fshm_blocks,
                                DSP_L1SRAM_MAX_DYNAMIC_ALLOCATIONS,
                                DSP_L1SRAM_ALLOC_GRANULARITY_SHIFT,
                                (IVA2_L1DSRAM_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF + DSP_L1SRAM_SCRATCH_SIZE)
                                );

                     if(C64_DEBUG_LEVEL(20))
                     {
                        printk(KERN_DEBUG "c64::dev::init: calling minim_init() (L2SRAM)\n");
                     }

                     /* Initialize L2SRAM FSHM allocator */
                     minim_init(&l2sram_fshm_mm,
                                l2sram_fshm_blocks,
                                DSP_L2SRAM_MAX_DYNAMIC_ALLOCATIONS,
                                DSP_L2SRAM_ALLOC_GRANULARITY_SHIFT,
                                (IVA2_L2SRAM_BASE_GPP + DSP_L2SRAM_FSHM_BASE_OFF)
                                );

                     if(C64_DEBUG_LEVEL(20))
                     {
                        printk(KERN_DEBUG "c64::dev::init: minim_init()s done OK\n");
                     }

                     /* Set up mutex that protects the clients array */
                     mutex_init(&mtx_clients);
         
                     /* Add the device */
                     r = cdev_add(c64_cdev, c64_dev_nr, 1);
                     
                     if(0 == r)
                     {
                        if(C64_DEBUG_LEVEL(1))
                        {
                           printk(KERN_DEBUG "c64: added char device dev_nr=0x%08x name=\"%s\"\n",
                                  c64_dev_nr,
                                  C64_DEV_NAME
                                  );
                        }

                        /* Query IVA2 clock */
                        iva2_clk = clk_get(c64_device, "iva2_ck");

                        if(NULL != iva2_clk)
                        {
                           /* Succeeded */
                       
                           ret = S_TRUE;
                        }
                        else
                        {
                           /* Failed: clk_get() */
                           printk(KERN_ERR "c64::dev_init: clk_get(\"iva2_ck\") failed.\n");

                           ret = S_FALSE;

                           goto failed_clk_get;
                        }
                     }
                     else
                     {
                        /* Failed: cdev_add() */
                        printk(KERN_ERR "c64: cdev_add() failed (r=%d)\n", r);

                        ret = S_FALSE;

                        goto failed_cdev_add;
                     }
                  }
#if 0
                  else
                  {
                     /* Failed: device_bind_driver() */
                     printk(KERN_ERR "c64: device_bind_driver() failed (r=%d).\n", r);

                     ret = S_FALSE;

                     goto failed_device_bind_driver;
                  }
#endif
               }
               else
               {
                  /* Failed: device_create() */
                  printk(KERN_ERR "c64: device_create() failed.\n");
                  
                  ret = S_FALSE;

                  goto failed_device_create;
               }
            }
            else
            {
               /* Failed: class_create() */
               printk(KERN_ERR "c64: driver_register() failed.\n");
               
               ret = S_FALSE;

               goto failed_class_create;
            }
         }
         else
         {
            /* Failed: cdev_alloc()() */
            printk(KERN_ERR "c64: cdev_alloc() failed.\n");
            
            ret = S_FALSE;

            goto failed_cdev_alloc;
         }
      }
      else
      {
         /* Failed: alloc_chrdev_region() */
         printk(KERN_ERR "c64: alloc_chrdev_region() failed (r=%d).\n", r);

         ret = S_FALSE;

         goto failed_alloc_chrdev_region;
      }
   }

   goto ok;

  failed_clk_get:
   cdev_del(c64_cdev);

  failed_cdev_add:
   device_destroy(c64_class, c64_dev_nr);

  failed_device_create:
   class_destroy(c64_class);

  failed_class_create:
   cdev_del(c64_cdev);

  failed_cdev_alloc:
   unregister_chrdev_region(c64_dev_nr, 1);

  failed_alloc_chrdev_region:

  ok:
   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::dev_init: LEAVE ret=%d\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- dev_exit() */
void dev_exit(void) {

   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::dev_exit: ENTER\n");
   }

   /* Teardown clients array (all file handles should have been released by now ?) */
   {
      sUI i;
      dev_client_t *cl = clients;
      
      for(i=0; i<C64_DEV_MAX_CLIENTS; i++)
      {
         dsp_cache_t type;

         for(type=0; type < NUM_DSP_CACHE_TYPES; type++)
         {
            if(0/*NULL*/ != cl->shm[type].dsp.phys_addr)
            {
               loc_shm_free(cl, type);
            }
         }
        
         /* Next client */
         cl++;
      }
   }

   clk_put(iva2_clk);


#if 0
   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev_exit: calling device_release_driver()\n");
   }

   device_release_driver(c64_device);
#endif

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev_exit: calling device_destroy()\n");
   }

   device_destroy(c64_class, c64_dev_nr); /* (note) crashes when driver was registered (and driver field was set) */

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev_exit: calling class_destroy()\n");
   }

   class_destroy(c64_class);

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev_exit: calling cdev_del()\n");
   }

   cdev_del(c64_cdev);

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::dev_exit: calling unregister_chrdev_region()\n");
   }

   unregister_chrdev_region(c64_dev_nr, 1);

   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::dev_exit: LEAVE\n");
   }
}
