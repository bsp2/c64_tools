/* ----
 * ---- file   : coff_err.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : COFF2 constants, types and structs. This is part of the "c64_load" utility.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_COFF_ERR_H__
#define __C64_TOOLS_COFF_ERR_H__

#include "../include/cplusplus_begin.h"


/* (note) the COFF error codes follow after the OSAL error codes (see osal_err.h) */


#define COFF_ERR_OK                    (0)

#define COFF_ERR                       (0x00020000)

#define COFF_ERR_GEN                   (COFF_ERR + 0x1000)
#define COFF_ERR_GEN_STRUCTPACK        (COFF_ERR_GEN + 0x001)
#define COFF_ERR_GEN_BIOS5XX           (COFF_ERR_GEN + 0x002)
#define COFF_ERR_GEN_NOCODEORDATA      (COFF_ERR_GEN + 0x003)
#define COFF_ERR_GEN_RAWBUFALLOC       (COFF_ERR_GEN + 0x004)

#define COFF_ERR_FILE                  (COFF_ERR + 0x2000)
#define COFF_ERR_FILE_ALREADYOPEN      (COFF_ERR_FILE + 0x001)
#define COFF_ERR_FILE_NOTFOUND         (COFF_ERR_FILE + 0x002)
#define COFF_ERR_FILE_NOTOPEN          (COFF_ERR_FILE + 0x003)
#define COFF_ERR_FILE_READOUTOFBOUNDS  (COFF_ERR_FILE + 0x004)
#define COFF_ERR_FILE_READ             (COFF_ERR_FILE + 0x005)
#define COFF_ERR_FILE_SEEK             (COFF_ERR_FILE + 0x006)

#define COFF_ERR_FILE_HEADER            (COFF_ERR + 0x4000)
#define COFF_ERR_FILE_HEADER_VERSION    (COFF_ERR_FILE_HEADER + 0x001)
#define COFF_ERR_FILE_HEADER_FLAGS      (COFF_ERR_FILE_HEADER + 0x002)
#define COFF_ERR_FILE_HEADER_OPTSIZE    (COFF_ERR_FILE_HEADER + 0x003)
#define COFF_ERR_FILE_HEADER_TARGETID   (COFF_ERR_FILE_HEADER + 0x004)

#define COFF_ERR_OPT_HEADER             (COFF_ERR + 0x5000)
#define COFF_ERR_OPT_HEADER_MAGIC       (COFF_ERR_OPT_HEADER + 0x001)

#define COFF_ERR_SEC_HEADER             (COFF_ERR + 0x6000)
#define COFF_ERR_SEC_HEADER_ALLOC       (COFF_ERR_SEC_HEADER + 0x001)

#define COFF_ERR_STRTBL                 (COFF_ERR + 0x7000)
#define COFF_ERR_STRTBL_SEEK            (COFF_ERR_STRTBL + 0x001)
#define COFF_ERR_STRTBL_READSZ          (COFF_ERR_STRTBL + 0x002)
#define COFF_ERR_STRTBL_ALLOC           (COFF_ERR_STRTBL + 0x003)
#define COFF_ERR_STRTBL_READCHARS       (COFF_ERR_STRTBL + 0x004)


typedef s_error_t coff_error_t;


#include "../include/cplusplus_end.h"

#endif /* __C64_TOOLS_COFF_ERR_H__ */
