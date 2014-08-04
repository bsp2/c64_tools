/* ----
 * ---- file   : dsp_c64.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 02Oct2013, 14Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_DSP_H__
#define __C64_TOOLS_KMOD_DSP_H__


/*
 * Does nothing.
 *
 */
sBool dsp_init (void);


/*
 * Power down the DSP.
 *
 */
void dsp_exit (void);


/*
 * Power on IVA2 subsystem and hold the DSP in reset.
 *
 */
int dsp_poweron_and_reset (void);


/*
 * Power down / suspend the IVA2 subsystem.
 *
 *  (note) the caller must ensure that no other applications use the DSP
 *
 */
int dsp_poweroff (void);


/*
 * Start DSP program execution.
 *
 *  (note) DSP must be in reset state
 *
 */
int dsp_start (sU32 _entryPoint);


/*
 * Resume DSP program execution.
 *
 *  (note) DSP must be in off state and dsp_start() must have been called before
 *
 */
int dsp_resume (void);


#endif /* __C64_TOOLS_KMOD_DSP_H__ */
