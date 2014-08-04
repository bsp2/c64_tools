/* ----
 * ---- file   : com.h
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
 * ---- changed: 14Sep2013, 15Sep2013, 18Sep2013, 21Sep2013, 02Oct2013, 03Oct2013, 05Oct2013
 * ----          20Oct2013
 * ----
 * ----
 */

#ifndef __MLB_COM_H__
#define __MLB_COM_H__

#include "../../../include/cplusplus_begin.h"


#define MLB_MAX_CORE_COMPONENTS    (16u)  /* first 2MB code area */

#define MLB_MAX_OVERLAY_COMPONENTS (32u)  


/* overlay section indices for CORE_FC_CMD_COM_OVERLAY_REGISTER (and unregister), and
 *  for return value of C64_IOCTL_OVERLAY_ALLOC (see "c64_dev.h")
 */
#define MLB_COM_SECTIONIDX_AREA2_1  (0u) /* second 1MB code area (4 sections/components) */
#define MLB_COM_SECTIONIDX_AREA2_2  (1u)
#define MLB_COM_SECTIONIDX_AREA2_3  (2u)
#define MLB_COM_SECTIONIDX_AREA2_4  (3u)
#define MLB_COM_SECTIONIDX_AREA3    (4u) /* third 2MB code area (1 section/component) */

#define MLB_COM_NUM_SECTION_INDICES (5u)


/* must match linker scripts */
#define MLB_SYSCALLS_ADDR  (0x86001000u)


/* Must at least be 1 */
#define MLB_MAX_COMPONENTS  (MLB_MAX_CORE_COMPONENTS + MLB_MAX_OVERLAY_COMPONENTS)

#define MLB_MAX_COMPONENT_NAME_LENGTH  (24u)


typedef sU16 dsp_component_id_t;
typedef sU16 dsp_component_cmd_t;


/*
 * Utility macro to interpret an unsigned component argument
 *  as a signed value.
 *
 *  (note) this macro is meant for use in dsp_component_exec_fxn functions
 *          (to cast the _arg1 / _arg2 parameters)
 */
#define DSP_ARG_S32(v)  (*(sS32*)&(v))


/*
 * Utility macro to read the _arg1/_arg2 parameters in a dsp_component_exec_fxn.
 *
 *  (note) yep, arg1/2 could be used directly but since there is DSP_ARG_S32, this
 *          macro was added for completeness' sake.
 */
#define DSP_ARG_U32(v)  (*(sU32*)&(v))


/*
 * Utility macro to cast a component id (enum) to an unsigned short.
 */
#define DSP_COMPONENT_ID(v) ((dsp_component_id_t)(v))


/*
 * Utility macro to cast a component command id (enum) to an unsigned short.
 */
#define DSP_COMPONENT_CMD_ID(v) ((dsp_component_cmd_t)(v))


/*
 * Utility macro to define a fastcall cmd (32bit, lower 16bits must be 0)
 */
#define DSP_COMPONENT_FC_CMD_DECLARE(c)  ((c) << 16u)


/*
 * Utility macro to dispatch a fastcall cmd (32bit, lower 16bits must be 0)
 *
 *   (this is mainly done so the compiler can generate a jump table for the switchcase)
 */
#define DSP_COMPONENT_FC_CMD_DISPATCH(c)  ((c) >> 16u)


/*
 * Component "init" callback function pointer type.
 *
 *  (note) When the callback returns S_FALSE, mlb_panic() will be invoked
 *          and the DSP will be stuck in an infinite loop.
 *
 */
typedef sBool (*dsp_component_init_fxn) (void);


/*
 * Component "exec" callback function pointer type.
 *
 *  "exec" functions will be called when a request has been received from the GPP.
 *
 *  The meaning of _cmd depends on the respective component.
 *  Usually this parameter is used as a function selector.
 *
 *  The "ret1" and "ret" output parameters can be used to return data to the GPP side.
 *   (the pointers are always != NULL).
 *
 *  The function should return 0 if the command was executed successfully.
 *
 *  A return value != 0 will signal the mailbox system that the command could
 *  not be executed.
 *
 *  (note) this is the only required component function
 *
 */
typedef sU32 (*dsp_component_exec_fxn) (dsp_component_cmd_t _cmd,
                                        sU32  _arg1, sU32  _arg2,
                                        sU32 *_ret1, sU32 *_ret2
                                        );


/*
 * Component "exit" callback function pointer type.
 *
 */
typedef void (*dsp_component_exit_fxn) (void);


/*
 * Component structure
 *
 *  (note) the component_id is filled out when the component is registered
 *          using mlb_component_register().
 *          It must not be 0.
 *
 *
 */
typedef struct {

   struct {
      dsp_component_init_fxn init;    /* [optional] Called after main() when mlb task is starting up */
      dsp_component_exec_fxn exec;    /* [required] Called when an RPC was received from the GPP */
      dsp_component_exec_fxn exec_fc; /* [optional] Called when a fastcall RPC is detected */
      dsp_component_exit_fxn exit;    /* [optional] Called when component is unregistered */
   } fxns;

   char name[MLB_MAX_COMPONENT_NAME_LENGTH];

   sU32/*dsp_component_id_t*/ component_id;  /* will be assigned in mlb_component_register() */
   
   sU32 overlay_section_idx;

   sSI ref_count;

} dsp_component_t;


/* Used to implement dummy main to satisfy linker when creating overlay images */
#define DSP_COMPONENT_MAIN void main(void) {}


#include "../../../include/cplusplus_end.h"

#endif /* __MLB_COM_H__ */
