/* ----
 * ---- file   : dev.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 11Sep2013, 14Sep2013, 16Sep2013, 17Sep2013, 13Dec2013, 15Dec2013, 14Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_DEV_H__
#define __C64_TOOLS_KMOD_DEV_H__


/* Maximum number of simultaneous /dev/c64 client applications (up to 32) */
#define C64_DEV_MAX_CLIENTS  (16u)


/*
 * Register device, initialize clients + L1/L2 memory allocator
 *
 */
sBool dev_init (void);


/*
 * Allow client connections
 *
 */
void dev_enable(void);


/*
 * Handle incoming DSP message and wake up client process
 *
 * (note) at the moment this function must be as lightweight as possible since
 *         it is directly called from the mlb irq handler.
 *
 */
void dev_handle_reply (sU32 _ret1, sU32 _ret2);


/*
 * Enable/disable IVA2 clk via Linux clk_*() API 
 *
 */
void dev_iva2_clk_set_enable(sBool _bEnabled);


/*
 * Deinitialize clients and unregister device
 *
 */
void dev_exit (void);


#endif /* __C64_TOOLS_KMOD_DEV_H__ */
