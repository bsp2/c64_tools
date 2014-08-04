/* ----
 * ---- file   : mlb.h
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
 * ---- changed: 10Sep2013, 12Sep2013, 14Sep2013, 15Sep2013, 18Sep2013, 19Sep2013
 * ----          02Oct2013, 03Oct2013, 04Oct2013, 11Dec2013
 * ----
 * ----
 */

#ifndef __MLB_H__
#define __MLB_H__


#define MLB_VERSION  (2u)


#define MLB_INT_VEC_ID  IVA2_INT_10


/*
 * Register resident DSP core component (mem. area 1).
 *
 *  Each component must have a unique id.
 *
 *  The dsp_component_t structure may be allocated on the stack -- it will
 *   be copied by this function.
 *
 *  This function return S_TRUE (1) when the component has been registered successfully.
 *
 */
S_EXTERN sBool mlb_core_component_register (dsp_component_id_t     _compId,
                                            const dsp_component_t *_comp
                                            );


/*
 * Start mailbox task / create mlb task.
 *
 * Must be called in main() after all components have been registered.
 *
 */
S_EXTERN void mlb_start (void);


/*
 * Write debug value that can be examined on the GPP side by reading 4 bytes from /dev/c64.
 *
 *  (note) idx must be in the range 0..3
 */
S_EXTERN void mlb_debug_usr (sU32 _idx, sU32 _val);


/*
 * This function can be called by DSP components in order to initiate a fastcall sequence.
 */
S_EXTERN void mlb_fastcall_initiate (void);


/*
 * This function should be called when the system (the DSP) becomes unusable.
 *
 *  (note) This function never returns.
 *
 */
S_EXTERN void mlb_panic (void);


/*
 * Return S_TRUE when DSP resume executeion after powersave mode.
 *
 */
S_EXTERN sBool mlb_resume (void);


/* Must be implemented by main application.
 *
 *  This is called after mlb task has been started and DSP was not just released from suspend mode.
 */
S_EXTERN sBool core_components_register (void);


#endif /* __MLB_H__ */
