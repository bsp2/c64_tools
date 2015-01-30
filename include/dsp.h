/* ----
 * ---- file   : dsp.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : libc64 main API. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 18Sep2013, 19Sep2013, 23Sep2013, 02Oct2013, 03Oct2013
 * ----          05Oct2013, 15Oct2013, 16Oct2013, 18Oct2013, 19Oct2013, 20Oct2013, 21Oct2013
 * ----          22Oct2013, 23Oct2013, 02Nov2013, 08Dec2013, 13Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_DSP_H__
#define __C64_TOOLS_DSP_H__

#include "cplusplus_begin.h"


typedef void *dsp_mspace_handle_t;


/*
 * Open client connection.
 *
 *  (note) calls osal_init()
 *
 *  Returns 0 on success.
 */
S_EXTERN int dsp_open (void);


/*
 * Close client connection.
 *
 *  (note) calls dsp_l1sram_free(), dsp_l2sram_free() in case application forgot to.
 *
 *  (note) frees shared memory allocated with dsp_shm_alloc() in case application forgot to.
 *
 *  (note) unloads overlays in case application forgot to.
 *
 *  (note) calls osal_exit()
 *
 *
 */
S_EXTERN void dsp_close (void);


/*
 * Power down and suspend the DSP.
 *
 *  (note) the DSP won't power off until have DSP client applications have
 *          requested the power off state.
 *
 */
S_EXTERN int S_CALL dsp_suspend (void);


/*
 *  Power on the DSP and resume program execution.
 *
 *   (note) unless autoresume was set to S_FALSE using dsp_autoresume_set(),
 *           DSP execution resumes when dsp_open() is called.
 *
 */
S_EXTERN int S_CALL dsp_resume (void);


/*
 * Set initial DSP resume mode.
 *
 *  The default behaviour is to resume DSP program execution when a client
 *   application connects using dsp_open()
 *
 *  (note) this function must be called before dsp_open() to have any effect
 */
S_EXTERN void dsp_autoresume_set (sBool _bAutoResume);


/*
 * Allocate GPP<>DSP shared memory and return physical+virtual addresses and size.
 *
 *  One allocation is allowed for each shared memory cache type (uncached, write-back, write-through)
 *
 *  (note) use dsp_mspace_create() to create a malloc()/free() style allocator for a memory region
 *
 *  (note) dsp_shm_free() must be called before this fxn can be called again with the same cache type.
 *
 *  (note) numBytes will be rounded up to the next page size (4096 bytes).
 *
 *  (note) shared memory will automatically be freed when dsp_close() is called
 *
 *  (note) when _type is DSP_CACHE_HUGETLB, the returned memory is NOT PHYSICALLY CONTIGUOUS if _numBytes
 *          exceeds the hugetlb page size (2MBytes). In that case, applications must call
 *          dsp_virt_to_phys() for each page to obtain its physical address.
 *
 *          The phys_addr field of dsp_mem_region_t will return only the physical address of the very
 *           first page!
 *
 *  The 'size' field of the returned struct is >=_numBytes on success, or 0 if the allocation failed.
 *
 */
S_EXTERN dsp_mem_region_t dsp_shm_alloc (dsp_cache_t _type, sU32 _numBytes);


/*
 * Allocate GPP<>DSP shared memory and return physical+virtual addresses and size.
 * This is identical to dsp_shm_alloc(), but allows to specify desired GPP virtual address.
 *
 *  (note) if desired GPP virtual address range is not available, the function may still succeed and
 *          return different address. It is the caller's responsibility to ensure the virtual address
 *          returned is suitable for it's needs.
 *
 *  The 'size' field of the returned struct is >=_numBytes on success, or 0 if the allocation failed.
 *
 */
S_EXTERN dsp_mem_region_t dsp_shm_alloc_at (dsp_cache_t _type, sU32 _numBytes, void *addr);


/*
 * Free GPP<>DSP shared memory.
 *
 *  (note) applications must ensure that this memory is not in use anymore.
 *
 *  Returns 0 on success.
 *
 */
S_EXTERN int dsp_shm_free (dsp_mem_region_t _mem);


/*
 * Create a malloc()/free() style allocator for a memory region.
 *
 *  (note) Uses Doug Lea's "dl_malloc" engine
 *
 */
S_EXTERN dsp_mspace_handle_t dsp_mspace_create (sU32 _baseAddr, sU32 _size);


/*
 * Allocate memory within the given memory space.
 *
 *  (note) Uses Doug Lea's "dl_malloc" engine
 *
 */
S_EXTERN void *dsp_mspace_malloc (dsp_mspace_handle_t _mspace, sU32 _numBytes);


/*
 * Allocate aligned memory within the given memory space.
 *
 *  (note) Uses Doug Lea's "dl_malloc" engine
 *
 */
S_EXTERN void *dsp_mspace_memalign (dsp_mspace_handle_t _mspace, sU32 _alignment, sU32 _numBytes);


/*
 * Free memory within the given memory space.
 *
 *  (note) Uses Doug Lea's "dl_malloc" engine
 *
 */
S_EXTERN void dsp_mspace_free (dsp_mspace_handle_t _mspace, void *_ptr);


/*
 * Destroy malloc()/free() style allocator instance.
 *
 *  (note) Uses Doug Lea's "dl_malloc" engine
 *
 */
S_EXTERN void dsp_mspace_destroy (dsp_mspace_handle_t _mspace);


/*
 * Query L1SRAM scratchbuffer physical+virtual addresses and size.
 *
 *  (note) the L1DSRAM scratchbuffer is shared by all processes. Its contents
 *          are not persistent across DSP remote procedure calls.
 *
 *  (note) Applications that need persistent L1DSRAM must call dsp_l1sram_alloc()/free()
 *
 *
 */
S_EXTERN dsp_mem_region_t dsp_l1sram_scratchbuffer_get (void);


/*
 * Dynamically allocate persistent L1SRAM.
 *
 *  Returns physical+virtual addresses and size. If size is 0, the allocation failed.
 *
 *  (note) All allocation requests will be rounded up to DSP_L1SRAM_ALLOC_GRANULARITY (see dsp_config.h)
 *
 *  (note) The L1SRAM should be freed using dsp_l1sram_free() when it is not longer used.
 *
 *  (note) All L1SRAM regions allocated by a process will be freed when the process terminates resp. the
 *          /dev/c64 filehandle is closed.
 *
 *  (note) L1SRAM is a precious resource. Please think twice about whether you could use regular shared memory instead.
 *
 *  The 'size' field of the returned struct is >=_numBytes on success, or 0 if the allocation failed.
 *
 */
S_EXTERN dsp_mem_region_t dsp_l1sram_alloc (sU32 _numBytes);


/*
 * Free dynamically allocated (persistent) L1SRAM
 *
 */
S_EXTERN int dsp_l1sram_free (dsp_mem_region_t _l1sram);


/*
 * Dynamically allocate persistent L2SRAM.
 *
 *  Returns physical+virtual addresses and size. If size is 0, the allocation failed.
 *
 *  (note) All allocation requests will be rounded up to DSP_L2SRAM_ALLOC_GRANULARITY (see dsp_config.h)
 *
 *  (note) The L2SRAM should be freed using dsp_l2sram_free() when it is not longer used.
 *
 *  (note) All L2SRAM regions allocated by a process will be freed when the process terminates resp. the
 *          /dev/c64 filehandle is closed.
 *
 *  (note) L2SRAM is a precious resource. Please think twice about whether you could use regular shared memory instead.
 *
 *  The 'size' field of the returned struct is >=_numBytes on success, or 0 if the allocation failed.
 *
 */
S_EXTERN dsp_mem_region_t dsp_l2sram_alloc (sU32 _numBytes);


/*
 * Free dynamically allocated (persistent) L2SRAM
 *
 */
S_EXTERN int dsp_l2sram_free (dsp_mem_region_t _l2sram);


/*
 * Translate virtual (GPP) to physical (DSP) address
 *
 *  (note) (currently) only works for addresses within the fshm (L1/L2) and shm regions
 *
 */
S_EXTERN sU32 dsp_virt_to_phys (void *_virt);


/*
 * Translate physical (DSP) to virtual (GPP) address
 *
 *  (note) (currently) only works for addresses within the fshm (L1/L2) and shm regions
 *
 */
S_EXTERN void *dsp_phys_to_virt (sU32 _phys);


/*
 * Translate physical GPP address to physical DSP address
 *
 *  (note) This only needs to be used for L1/L2 SRAM addresses
 *          (the DSP local interconnect view of the address is different from the GPP L3 interconnect view)
 *          Regular RAM physical addresses are the same for both GPP and DSP and do not need to be translated.
 *
 */
S_EXTERN sU32 dsp_physgpp_to_physdsp (sU32 _physGPP);


/*
 * Load component overlay from filesystem and return the component id.
 *
 *  If the component is already loaded, this simply returns the component id (lazy load).
 *
 *  If the searchpath argument is NULL, the .out file will be searched first in the current working directory,
 *   then in the system-wide "/lib/c64_tools/images/" directory.
 *
 *  For the componentName argument, it is recommended to use the name constant defined in the
 *   component include file, e.g. COMPONENT_NAME_CORE.
 *
 *  "bIncRef" should be set to S_TRUE to prevent the component from being unloaded when the application
 *   that originally registered it exits.
 *
 */
S_EXTERN int dsp_component_load(const char *_searchPath,
                                const char *_componentName,
                                dsp_component_id_t *_retComponentId
                                );


/*
 * Do DSP remote procedure call and do NOT wait for reply.
 *
 *  Returns 0 on success.
 *
 *  dsp_rpc_recv() can be called to retrieve the RPC return values.
 *
 */
S_EXTERN int dsp_rpc_send (const dsp_msg_t *_msgTo);


/*
 * Query return values of last remote procedure call.
 *
 *  Returns 0 on success.
 *
 *  _msgFrom may be NULL (i.e. simply wait for RPC to finish)
 *
 */
S_EXTERN int dsp_rpc_recv (dsp_msg_t *_msgFrom);


/*
 * Do DSP remote procedure call and wait for reply.
 *
 *  Returns 0 on success.
 *
 *  _msgFrom may be NULL (i.e. simply wait for RPC to finish)
 *
 */
S_EXTERN int dsp_rpc (const dsp_msg_t *_msgTo,
                            dsp_msg_t *_msgFrom
                      );


/*
 * Enable polling mode (~5 times lower message latency but not very OS friendly).
 *
 *  Default=off (use select())
 */
S_EXTERN void dsp_poll_enable (sBool _bEnabled);


/*
 * Do DSP remote procedure call that expects two arguments
 *  and has one return value.
 *
 *  Returns 0 on success.
 *
 */
S_EXTERN int dsp_rpc_2a_1r (sU16 _compId, 
                            sU16 _cmdId, 
                            sU32 _arg1, 
                            sU32 _arg2, 
                            sU32 *_ret
                            );


/*
 * Do DSP remote procedure call that expects two arguments
 *  and has two return values.
 *
 *  Returns 0 on success.
 *
 */
S_EXTERN int dsp_rpc_2a_2r(sU16 _compId, 
                           sU16 _cmdId, 
                           sU32 _arg1, 
                           sU32 _arg2, 
                           sU32 *_ret1,
                           sU32 *_ret2
                           );


/*
 * Perform fastcall RPC / wait for result.
 *
 *  (note) The fastcall sequence must have been initiated by the DSP.
 *
 */
S_EXTERN int dsp_fastcall_rpc (sU32 _cmdFC,
                               sU32 _arg1, sU32 _arg2,
                               sU32 *_r1FC, sU32 *_r2FC
                               );


/*
 * Perform fastcall RPC but do not wait for result.
 *
 *  (note) The fastcall sequence must have been initiated by the DSP.
 *
 *  (note) Applications MUST call dsp_fastcall_rpc_recv() if this call was successfuly
 *
 */
S_EXTERN int dsp_fastcall_rpc_send (sU32 _cmdFC,
                                    sU32 _arg1, sU32 _arg2
                                    );


/*
 * Wait and pickup result / return values of last fastcall RPC.
 *
 *  (note) dsp_fastcall_rpc_send() must have been called 
 *
 */
S_EXTERN int dsp_fastcall_rpc_recv (sU32 *_r1FC, sU32 *_r2FC);


/*
 * End fastcall sequence.
 *
 */
S_EXTERN void dsp_fastcall_end (void);


/*
 * Writeback the GPP caches for the given shared memory region.
 *
 *  (note) the given address range must be located within the GPP<>DSP shared memory area.
 *
 *  (note) dsp_shm_alloc() must be called before this fxn can be used.
 *
 *  (note) does this fxn ever have to be used ? does not seem any difference, memory always seems write-through!??
 *
 */
S_EXTERN int dsp_cache_wb (sU32 _physAddr, sU32 _size);


/*
 * Writeback the GPP caches for the given shared memory region.
 *
 *  (note) semantics are the same as for dsp_cache_wb() except that this fxn expects a virtual address
 *
 */
S_EXTERN int dsp_cache_wb_virt (void *_virtAddr, sU32 _size);


/*
 * Invalidate the GPP caches for the given shared memory region.
 *
 *  (note) the given address range must be located within the GPP<>DSP shared memory area.
 *
 *  (note) dsp_shm_alloc() must be called before this fxn can be used.
 *
 */
S_EXTERN int dsp_cache_inv (sU32 _physAddr, sU32 _size);


/*
 * Invalidate the GPP caches for the given shared memory region.
 *
 *  (note) semantics are the same as for dsp_cache_inv() except that this fxn expects a virtual address
 *
 */
S_EXTERN int dsp_cache_inv_virt (void *_virtAddr, sU32 _size);


/*
 * Writeback and invalidate the GPP caches for the given shared memory region.
 *
 *  (note) the given address range must be located within the GPP<>DSP shared memory area.
 *
 *  (note) dsp_shm_alloc() must be called before this fxn can be used.
 *
 */
S_EXTERN int dsp_cache_wbinv (sU32 _physAddr, sU32 _size);


/*
 * Writeback and invalidate the GPP caches for the given memory region.
 *
 *  (note) semantics are the same as for dsp_cache_wbinv() except that this fxn expects a virtual address
 *
 */
S_EXTERN int dsp_cache_wbinv_virt (void *_virtAddr, sU32 _size);


/*
 * Writeback and invalidate all GPP caches (expensive, ~1ms)
 *
 */
S_EXTERN int dsp_cache_wbinvall (void);


/*
 * Query user defined DSP debug value.
 *
 *  "idx" must be in the range 0..3.
 *
 *  Returns 0 on success.
 */
S_EXTERN int dsp_debug_usr_get (sU32 _idx, sU32 *_retVal);


/*
 * Print user defined DSP debug value.
 *
 *  "idx" must be in the range 0..3.
 *
 */
S_EXTERN void dsp_debug_usr_print (sU32 _idx);


/*
 * Print (new) log ringbuffer lines.
 *
 *  (note) this is not multi-process safe
 *          (it won't crash but processes will race for the ringbuffer output)
 *
 */
S_EXTERN void dsp_logbuf_print (void);


#include "cplusplus_end.h"

#endif /* C64_TOOLS_DSP_H__ */
