/* ----
 * ---- file   : kmod.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 14Sep2013, 17Sep2013, 02Oct2013, 29Oct2013, 13Dec2013, 15Dec2013, 18Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_H__
#define __C64_TOOLS_KMOD_H__


/* (note) in order for this to work, the kernel must be patched so that it returns 
 *         a CMA block at a fixed physical address when DSP_MAX_IMAGE_SIZE bytes are
 *         allocated via dma_alloc_coherent() by the "c64_tools" _driver.
 *
 *          (e.g. the latest Pandora kernel, thanks notaz!)
 */
#define USE_PLATFORM_DRIVER defined


/* If defined, the kernel function "void dsp_power_notify(int _bEnabled)" must exist.
 *
 *  dsp_power_notify(1) will be called before the DSP is powered on / resumed, and
 *  dsp_power_notify(0) will be called after the DSP is powered off / suspended.
 *
 *  The kernel can implement this function so that it e.g. updates the voltage and
 *   DSP clock rate (OPP). While the DSP is powered off, the user-configurable DSP
 *   clock rate can be ignored and a lower voltage level can be configured.
 *
 *  (feature requested by notaz)
 *
 */
#define USE_DSP_POWER_NOTIFY defined


/* If defined, force low power bypass mode during DSP resume.
 *  The DSP cannot be over/underclocked in this case.
 *
 *   (note) only works with kmod/__ATTIC/dsp_c64.c__ORIG
 */
//#define USE_FORCED_LOWPOWER_BYPASS defined


#ifdef USE_PLATFORM_DRIVER
/* DSP reset vector and code memory area */
S_EXTERN void *dma_virt_4m;
#endif

#ifdef USE_DSP_POWER_NOTIFY
S_EXTERN void dsp_power_notify(int _bEnabled);
#else
S_EXTERN void dsp_power_notify__STUB(int _bEnabled);
#endif /* USE_DSP_POWER_NOTIFY */

S_EXTERN sUI c64_debug_level;

S_EXTERN sBool c64_pwrlog;

S_EXTERN sBool c64_pwroff;


#define C64_DEBUG_LEVEL(lvl)  (c64_debug_level >= (lvl))

//#define C64_DEBUG_LEVEL(lvl)  (0)


#endif /* __C64_TOOLS_KMOD_H__ */
