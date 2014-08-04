/* ----
 * ---- file   : core.h
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
 * ---- changed: 02Oct2013, 04Oct2013, 13Dec2013
 * ----
 * ----
 */

#ifndef __COMPONENT_CORE_H__
#define __COMPONENT_CORE_H__


#define COMPONENT_NAME_CORE  "core"


enum {
   CORE_CMD_COM_OVERLAY_INITIATE = 1,  /* initiate dynamic (overlay) loader fastcall sequence */
   CORE_CMD_COM_FIND             = 2,  /* find component by name (**deprecated**, use CORE_FC_CMD_COM_OVERLAY_FIND instead) */
   CORE_CMD_COM_EMERGENCY_UNLOAD = 3,  /* emergency unload overlay section (do NOT call) */

   NUM_CORE_COMMANDS
};


enum {
   CORE_FC_CMD_COM_OVERLAY_CACHE_INV  = DSP_COMPONENT_FC_CMD_DECLARE(1),
   CORE_FC_CMD_COM_OVERLAY_REGISTER   = DSP_COMPONENT_FC_CMD_DECLARE(2),  /* arg1=sectionIdx (0..3, or 4) */
   CORE_FC_CMD_COM_OVERLAY_UNREGISTER = DSP_COMPONENT_FC_CMD_DECLARE(3),  /* arg1=sectionIdx (0..3, or 4) */
   CORE_FC_CMD_COM_OVERLAY_FIND       = DSP_COMPONENT_FC_CMD_DECLARE(4),

   __PAD__
};


enum {
   CORE_ERR_OK            = 0,
   CORE_ERR_ILLCMD        = 1,  /* illegal cmd */
   CORE_ERR_ILLFCCMD      = 2,  /* illegal fastcall cmd */
   CORE_ERR_COM_NOT_FOUND = 3,  /* component not found */
   CORE_ERR_ILLSECIDX     = 4,  /* illegal section index */
   CORE_ERR_COM_ALLUSED   = 5,  /* all components used, cannot register */
   CORE_ERR_COM_INIT      = 6,  /* component init() failed */
   CORE_ERR_NOEXEC        = 7,  /* trying to register component that has no exec function */
   CORE_ERR_NULLPTR       = 8,  /* null ptr argument where none is expected */

   NUM_CORE_ERROR_CODES
};


#endif /* __COMPONENT_CORE_H__ */
