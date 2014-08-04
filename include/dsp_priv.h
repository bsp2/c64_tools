/* ----
 * ---- file   : dsp_priv.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : libc64 private API. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 18Sep2013, 19Sep2013, 23Sep2013, 02Oct2013, 03Oct2013
 * ----          05Oct2013, 15Oct2013, 18Oct2013, 27Oct2013, 08Dec2013, 13Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_DSP_PRIV_H__
#define __C64_TOOLS_DSP_PRIV_H__

#include "cplusplus_begin.h"


/*
 * Return human-readable string for DSP error code.
 *
 */
S_EXTERN const char * S_CALL dsp_strerror (dsp_error_t _err);


/* **deprecated** 
 *
 *  (superceded by dsp_resume())
 */
S_EXTERN int dsp_power_off (void);


/*
 * Print c64_tools internal debug value (debugging)
 *
 *  (note) client applications should not call this fxn.
 *
 */
S_EXTERN void dsp_debug_internal_print (void);


/*
 * Call TEST ioctl (debugging).
 *
 *  (note) client applications should not call this fxn.
 *
 */
S_EXTERN int dsp_ioctl_test (void);


/*
 * **Deprecated**
 *
 *  Applications should use dsp_l1sram_scratchbuffer_get() instead
 */
S_EXTERN dsp_mem_region_t dsp_fshm_scratchbuffer_get (void);


/*
 * **Deprecated**
 *
 *  Applications should use dsp_l1sram_alloc() instead.
 *
 */
S_EXTERN dsp_mem_region_t dsp_fshm_alloc (sU32 _numBytes);


/*
 * **Deprecated**
 *
 *  Applications should use dsp_l1sram_free() instead.
 *
 */
S_EXTERN int dsp_fshm_free (dsp_mem_region_t _fshm);


/*
 * Call L1SRAM_FSHM_DEBUG ioctl (debugging).
 *
 *  (note) client applications should not call this fxn.
 *
 */
S_EXTERN int dsp_l1sram_debug (sUI _sequenceId);


/*
 * **Deprecated**
 *
 *  dsp_l1sram_debug() should be used instead.
 *
 */
S_EXTERN int dsp_fshm_debug (sUI _sequenceId);


/*
 * Call L2SRAM_FSHM_DEBUG ioctl (debugging).
 *
 *  (note) client applications should not call this fxn.
 *
 */
S_EXTERN int dsp_l2sram_debug (sUI _sequenceId);


/*
 * Send testmessage, bypassing client handlers (debugging).
 *
 *  (note) client applications should not call this fxn.
 *
 */
S_EXTERN void dsp_send_forced_message (sU32 _val);


/*
 * Used to skip the dsp_open() call in dsp_init_root()
 *
 */
S_EXTERN void dsp_set_enable_skip_dsp_open (sBool _bEnabled);


/*
 * To be used only by "c64_load", client applications must call dsp_close()
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_init_root (void);


/*
 * To be used only by "c64_load", client applications must call dsp_close()
 *
 */
S_EXTERN void S_CALL dsp_exit_root (void);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_mmap (sUI _physAddr, sUI _numBytes, sUI *_retAddr);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_munmap (sUI _virtAddr, sUI _numBytes);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_reg_write_32 (sUI _physAddr, sUI _value);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_reg_read_32 (sUI _physAddr, sUI *_retValue);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_reg_bit_set (sUI _physAddr, sUI _bitNr);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_reg_bit_clear (sUI _physAddr, sUI _bitNr);


/*
 * Find DSP component ID by name.
 *
 *  This is used for dynamically loaded components ("overlays").
 *
 *  Application usually do not have to call this, it is rather used by libc64 internally.
 *
 */
S_EXTERN int dsp_component_id_find_by_name (const char *_name, sU32 *_retSecIdxAndComponentId, sBool _bIncRef);


/*
 * Power up the DSP and hold it in reset. Uses dev ioctl.  
 *
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_power_on_and_reset (void);


/*
 * To be used only by "c64_load".
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_start (sU32 _entryPoint);


/*
 * (todo) document me
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_copy_to (sUI _physAddr, void *_srcBuf, sUI _numBytes);


/*
 * (todo) document me
 *
 */
S_EXTERN dsp_error_t S_CALL dsp_memset (sUI _physAddr, sU8 _c, sUI _numBytes);


#include "cplusplus_end.h"

#endif /* C64_TOOLS_DSP_PRIV_H__ */
