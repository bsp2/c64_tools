/* ----
 * ---- file   : dsp_config.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : DSP memory configuration. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 07Sep2013, 08Sep2013, 11Sep2013, 13Sep2013, 18Sep2013, 02Oct2013
 * ----
 * ----
 */

#ifndef C64_KBUILD
#include "../../../include/types.h"
#include "../../../include/dsp_common.h"
#include "../../../include/dsp_config.h"
#endif // C64_KBUILD


#define CFG_PANDORA_4MB     // 0x86000000..86400000


#if defined(CFG_PANDORA_4MB)
dsp_config_t dsp_config = {
#ifndef CCS
   .reset_vector = 
#endif
   {
      0x86000000u,
      0x00000000u,
      0x00001000u
   },
   
#ifndef CCS
   .ram =
#endif
   {
      0x86002000u,
      0x00000000u,
      0x003FE000u
   },
};

#else
#error No dsp_config defined. Please edit "dsp_config.c".
#endif
