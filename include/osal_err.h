/* ----
 * ---- file   : osal_err.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : OS abstraction layer. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 08Sep2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_OSAL_ERR_H__
#define __C64_TOOLS_OSAL_ERR_H__

#include "cplusplus_begin.h"


#define OSAL_ERR_OK       (0)

/* (note) the OSAL error codes follow after the COFF error codes (see coff_err.h) */

#define OSAL_ERR            (0x00010000)

#define OSAL_ERR_MEM        (OSAL_ERR + 0x1000)
#define OSAL_ERR_MEM_OPEN   (OSAL_ERR_MEM + 0x001)
#define OSAL_ERR_MEM_MAP    (OSAL_ERR_MEM + 0x002)
#define OSAL_ERR_MEM_UNMAP  (OSAL_ERR_MEM + 0x003)


typedef s_error_t osal_error_t;


#include "cplusplus_end.h"

#endif /* __C64_TOOLS_OSAL_ERR_H__ */
