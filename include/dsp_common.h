/* ----
 * ---- file   : dsp_common.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Common DSP types and structs. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 11Sep2013, 13Sep2013, 14Sep2013, 15Sep2013, 17Sep2013
 * ----          19Sep2013, 20Sep2013, 03Oct2013, 15Oct2013, 18Oct2013, 01Nov2013, 02Nov2013
 * ----          08Dec2013, 15Dec2013, 18Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_DSP_COMMON_H__
#define __C64_TOOLS_DSP_COMMON_H__

#include "cplusplus_begin.h"


/*
 * (Shared) memory cache attribute types (see dsp_shm_init(), dsp_shm_get())
 */
typedef enum {
   DSP_CACHE_NONE    = 0,  /* read/writes are uncached on GPP side 
                            *  (note) no dsp_cache_inv() or dsp_cache_wb() calls needed
                            *  (note) slow writes + reads on GPP side
                            */

   DSP_CACHE_RW      = 1,  /* read/writes are cached on GPP side (writeback) 
                            *  (note) GPP must call dsp_cache_wb() before DSP can see data
                            *  (note) GPP must call dsp_cache_inv() before it can see data written by DSP
                            *  fast writes + reads
                            */

   DSP_CACHE_R       = 2,  /* reads are cached, writes are cached on GPP side (writethrough) 
                            *  (note) GPP must call dsp_cache_inv() before it can see data written by DSP
                            *  (note) fast writes + reads
                            */

   DSP_CACHE_W       = 3,  /* reads are uncached, writes are cached on GPP side (writecombine) 
                            *  (note) no dsp_cache_inv() or dsp_cache_wb() needed
                            *  (note) fast writes but slow GPP-side reads
                            */

   /* Special cache type that indicates that hugetlb pages shall be used.
    *
    *  (note) currently, hugetlb allocations are done via mmap() in userspace but they *should*
    *          be done in kernel space.
    *
    *  (note) before huge pages can be used, they must be reserved first.
    *          See here https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/vm/hugetlbpage.txt
    *  (note) HUGETLB memory uses 2 MByte pages on ARMv7 (instead of 4kb)
    *
    *  (note) on the Open Pandora, huge page memory can be reserved using
    *          /usr/pandora/scripts/op_hugetlb.sh <megabytes>
    */
   DSP_CACHE_HUGETLB = 4,  /* read/writes are cached on GPP side (writeback) 
                            *  (note) GPP must call dsp_cache_wb() before DSP can see data
                            *  (note) GPP must call dsp_cache_inv() before it can see data written by DSP
                            *  (very) fast writes + reads
                            */

   NUM_DSP_CACHE_TYPES

} dsp_cache_t;


#define NUM_CONTIG_DSP_CACHE_TYPES  (4)


/* DSP boot flags */
#define DSP_BOOTFLAG_REBOOT   (0u)           /* set by GPP before releasing DSP from reset */
#define DSP_BOOTFLAG_RESUME   (0xFFffFFffu)  /* set by GPP before releasing DSP from reset */
#define DSP_BOOTFLAG_RUNNING  (0x44894489u)  /* set by DSP when it's ready to process RPCs */


/* for IPC / from GPP to DSP */
#define DSP_IPC_BASE_GPP  (IVA2_L1DSRAM_BASE_GPP)
////(must use compiler generated array-access code or DSP will freeze) #define DSP_IPC_BASE_DSP  (IVA2_L1DSRAM_BASE_DSP)
#define DSP_IPC_SIZE  (64u)

// core IPC memory: (16 bytes)
#define DSP_IPC_MSG_SERIAL_OFF  ( 0u)
#define DSP_IPC_MSG_COMP_OFF    ( 4u)  // component id (lo) + cmd id (hi)
#define DSP_IPC_BOOTMODE_OFF    ( 4u)  // used to signal DSP that exec. resumes after suspend/powersave mode (FFFFFFFFu)
#define DSP_IPC_MSG_ARG1_OFF    ( 8u)
#define DSP_IPC_MSG_ARG2_OFF    (12u)
#define DSP_IPC_MSG_RET1_OFF    ( 8u)
#define DSP_IPC_MSG_RET2_OFF    (12u)

// component error number and internal debug memory:
#define DSP_IPC_ERRNO_OFF                 (16u)
#define DSP_IPC_DEBUG_DSPIRQCOUNT_OFF     (20u)
#define DSP_IPC_DEBUG_DSPITERCOUNT_OFF    (24u)
#define DSP_IPC_DEBUG_INTERNAL_OFF        (28u)

// user defined debug values:
#define DSP_IPC_DEBUG_USR0_OFF            (32u)
#define DSP_IPC_DEBUG_USR1_OFF            (36u)
#define DSP_IPC_DEBUG_USR2_OFF            (40u)
#define DSP_IPC_DEBUG_USR3_OFF            (44u)

// gpp side IPC addresses:
#define DSP_IPC_MSG_SERIAL_GPP  (DSP_IPC_BASE_GPP + DSP_IPC_MSG_SERIAL_OFF)
#define DSP_IPC_MSG_COMP_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_MSG_COMP_OFF)
#define DSP_IPC_BOOTMODE_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_BOOTMODE_OFF)
#define DSP_IPC_MSG_ARG1_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_MSG_ARG1_OFF)
#define DSP_IPC_MSG_ARG2_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_MSG_ARG2_OFF)
#define DSP_IPC_MSG_RET1_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_MSG_RET1_OFF)
#define DSP_IPC_MSG_RET2_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_MSG_RET2_OFF)

#define DSP_IPC_ERRNO_GPP                 (DSP_IPC_BASE_GPP + DSP_IPC_ERRNO_OFF)
#define DSP_IPC_DEBUG_DSPIRQCOUNT_GPP     (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_DSPIRQCOUNT_OFF)
#define DSP_IPC_DEBUG_DSPITERCOUNT_GPP    (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_DSPITERCOUNT_OFF)
#define DSP_IPC_DEBUG_INTERNAL_GPP        (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_INTERNAL_OFF)

#define DSP_IPC_DEBUG_USR0_GPP            (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_USR0_OFF)
#define DSP_IPC_DEBUG_USR1_GPP            (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_USR1_OFF)
#define DSP_IPC_DEBUG_USR2_GPP            (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_USR2_OFF)
#define DSP_IPC_DEBUG_USR3_GPP            (DSP_IPC_BASE_GPP + DSP_IPC_DEBUG_USR3_OFF)


// L1D scratch area for user defined components (accessible on the ARM):
#define DSP_L1SRAM_FSHM_SIZE  (24u * 1024u)  /* reserve up to 24 kbytes for GPP<>DSP userspace-accessible fast shared memory */

#define DSP_L1SRAM_FSHM_BASE_OFF  (IVA2_L1DSRAM_SIZE - DSP_L1SRAM_FSHM_SIZE)  /* placed at end of L1DSRAM */

// spinlock driven fast-calls:
#define DSP_FSHM_FASTCALL_CTL_DONE  (0u)
#define DSP_FSHM_FASTCALL_CTL_IDLE  (1u)  /* written by regular DSP-side RPC to initiate fastcall sequence */
#define DSP_FSHM_FASTCALL_CTL_REQ   (2u)  /* written by GPP to request fast RPC */
#define DSP_FSHM_FASTCALL_CTL_ACK   (3u)  /* written by GPP or DSP to acknowledge request (RPC done) and go back to idle */


/* fastcall 'registers' are located at the very end of the L1DSRAM */                                              
#define DSP_FSHM_REG_SIZE  (64u)
#define DSP_FSHM_TODSP_REG_OFF   (DSP_L1SRAM_FSHM_BASE_OFF + DSP_L1SRAM_FSHM_SIZE - 2*DSP_FSHM_REG_SIZE)
#define DSP_FSHM_TOGPP_REG_OFF   (DSP_L1SRAM_FSHM_BASE_OFF + DSP_L1SRAM_FSHM_SIZE - 1*DSP_FSHM_REG_SIZE)

#define DSP_FSHM_MSG_TODSP_CTL_OFF          (DSP_FSHM_TODSP_REG_OFF +  0u)  /* control 'register' (see above) */
#define DSP_FSHM_MSG_TODSP_CMD_OFF          (DSP_FSHM_TODSP_REG_OFF +  4u)  /* component fastcall cmd (upper 16bits only) */
#define DSP_FSHM_MSG_TODSP_ARG1_OFF         (DSP_FSHM_TODSP_REG_OFF +  8u)
#define DSP_FSHM_MSG_TODSP_ARG2_OFF         (DSP_FSHM_TODSP_REG_OFF + 12u)

#define DSP_FSHM_MSG_TOGPP_CTL_OFF          (DSP_FSHM_TOGPP_REG_OFF +  0u)  /* control 'register' (see above) */
#define DSP_FSHM_MSG_TOGPP_CMD_OFF          (DSP_FSHM_TOGPP_REG_OFF +  4u)  /* component fastcall cmd (upper 16bits only) */
#define DSP_FSHM_MSG_TOGPP_RET1_OFF         (DSP_FSHM_TOGPP_REG_OFF +  8u)
#define DSP_FSHM_MSG_TOGPP_RET2_OFF         (DSP_FSHM_TOGPP_REG_OFF + 12u)


/* GPP-side offsets */
#define DSP_FSHM_REG_TODSP_GPP_OFF  (DSP_L1SRAM_FSHM_SIZE - 2*DSP_FSHM_REG_SIZE)
#define DSP_FSHM_REG_TOGPP_GPP_OFF  (DSP_L1SRAM_FSHM_SIZE - 1*DSP_FSHM_REG_SIZE)


#define DSP_FSHM_MSG_TODSP_CTL_GPP_OFF          (DSP_FSHM_REG_TODSP_GPP_OFF +  0u)  /* control 'register' (see above) ->5cf0fff0 */
#define DSP_FSHM_MSG_TODSP_CMD_GPP_OFF          (DSP_FSHM_REG_TODSP_GPP_OFF +  4u)  /* component fastcall cmd (upper 16bits only) */
#define DSP_FSHM_MSG_TODSP_ARG1_GPP_OFF         (DSP_FSHM_REG_TODSP_GPP_OFF +  8u)
#define DSP_FSHM_MSG_TODSP_ARG2_GPP_OFF         (DSP_FSHM_REG_TODSP_GPP_OFF + 12u)


#define DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF          (DSP_FSHM_REG_TOGPP_GPP_OFF +  0u)  /* control 'register' (see above) ->5cf0fff0 */
#define DSP_FSHM_MSG_TOGPP_CMD_GPP_OFF          (DSP_FSHM_REG_TOGPP_GPP_OFF +  4u)  /* component fastcall cmd (upper 16bits only) */
#define DSP_FSHM_MSG_TOGPP_ARG1_GPP_OFF         (DSP_FSHM_REG_TOGPP_GPP_OFF +  8u)
#define DSP_FSHM_MSG_TOGPP_ARG2_GPP_OFF         (DSP_FSHM_REG_TOGPP_GPP_OFF + 12u)

#define DSP_FSHM_MSG_TOGPP_RET1_GPP_OFF         (DSP_FSHM_REG_TOGPP_GPP_OFF +  8u)  /* same as args */
#define DSP_FSHM_MSG_TOGPP_RET2_GPP_OFF         (DSP_FSHM_REG_TOGPP_GPP_OFF + 12u)


#define DSP_L1SRAM_FSHM_BASE_GPP  (IVA2_L1DSRAM_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF)


#define DSP_L1DSRAM_SCRATCHBUFFER_BASE_DSP  (IVA2_L1DSRAM_BASE_DSP + DSP_IPC_SIZE)
#define DSP_L1DSRAM_SCRATCHBUFFER_SIZE_DSP  (IVA2_L1DSRAM_SIZE - DSP_L1SRAM_FSHM_SIZE - DSP_IPC_SIZE)  /* ~24kb */


#define DSP_L2SRAM_FSHM_BASE_OFF  (0u)
#define DSP_L2SRAM_FSHM_SIZE      (IVA2_L2SRAM_SIZE - DSP_L2SRAM_FSHM_BASE_OFF)
#define DSP_L2SRAM_FSHM_BASE_GPP  (IVA2_L2SRAM_BASE_GPP + DSP_L2SRAM_FSHM_BASE_OFF)


typedef struct {
   sU32 phys_addr;  // GPP physical address (except for L1/l2 SRAM areas, this is also the DSP phys.addr)
   sU32 virt_addr;  // on ARM-side (mmap'd) (!= 0 while mapped)
   sU32 size;       // number of bytes
} dsp_mem_region_t;


#include "cplusplus_end.h"

#endif /* __C64_TOOLS_DSP_COMMON_H__ */
