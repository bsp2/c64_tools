/* ----
 * ---- file   : osal.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : OS abstraction layer utility functions. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013
 * ----
 * ----
 */

#include "../../include/types.h"
#include "../../include/log.h"
#include "../../include/osal_err.h"
#include "../../include/osal.h"


/*--------------------------------------------------------------------------- osal_strerror() */
const char * S_CALL osal_strerror(osal_error_t _err) {
   switch(_err)
   {
      default:
         return NULL;

      case OSAL_ERR_OK:           return "OSAL_ERR_OK";

      case OSAL_ERR_MEM:          return "OSAL_ERR_MEM";
      case OSAL_ERR_MEM_OPEN:     return "OSAL_ERR_MEM_OPEN";
      case OSAL_ERR_MEM_MAP:      return "OSAL_ERR_MEM_MAP";
      case OSAL_ERR_MEM_UNMAP:    return "OSAL_ERR_MEM_UNMAP";
   }
}
