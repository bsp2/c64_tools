/* ----
 * ---- file   : dsp_err.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : DSP error codes. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_DSP_ERR_H__
#define __C64_TOOLS_DSP_ERR_H__

#include "cplusplus_begin.h"


#define DSP_ERR_OK       (0)

/* (note) the DSP error codes follow after the COFF error codes (see coff_err.h) */

#define DSP_ERR            (0x00030000)

#define DSP_ERR_MEM        (DSP_ERR + 0x1000)
#define DSP_ERR_MEM_OPEN   (DSP_ERR_MEM + 0x001)
#define DSP_ERR_MEM_MAP    (DSP_ERR_MEM + 0x002)
#define DSP_ERR_MEM_UNMAP  (DSP_ERR_MEM + 0x003)
#define DSP_ERR_MEM_CHECK  (DSP_ERR_MEM + 0x004)


typedef s_error_t dsp_error_t;


#include "cplusplus_end.h"

#endif /* __C64_TOOLS_DSP_ERR_H__ */
