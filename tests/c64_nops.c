/* ----
 * ---- file   : c64_nops.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          ARM fixpoint optimized code contributed by M-HT.
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 23Oct2013, 24Oct2013, 25Oct2013, 02Nov2013, 14Nov2013, 11Dec2013
 * ----
 * ----
 */

#include "../include/inc_libc64.h"
#include "../include/arch/dm3730/dm3730.h"

#include <core/components.h>
#include <components/nops/nops.h>


/*--------------------------------------------------------------------------- module vars */
static dsp_component_id_t compid_nops;


/*--------------------------------------------------------------------------- loc_lazy_load_components() */
sBool loc_lazy_load_components(void) {
   sBool ret;

   ret = S_FALSE;

   if(0 == dsp_component_load(NULL, COMPONENT_NAME_NOPS, &compid_nops))
   {
      /* Succeeded */
      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_nops() */
static void loc_nops(void) {
   int ret;

   log_printf(LOG_INFO "starting NOP / calibration test\n");
   
   /* Open client connection */
   ret = dsp_open();
   
   if(0 == ret)
   {
      if(loc_lazy_load_components())
      {
         dsp_msg_t msg;
         sU32 tStart;
         sU32 tEnd;
         
         tStart = osal_milliseconds_get();
         
         DSP_MSG_INIT(&msg, compid_nops, NOPS_CMD_NOPS, 0, 0);
         
         dsp_rpc(&msg, NULL);
         
         tEnd = osal_milliseconds_get();

         {
            sU32 tDelta = (tEnd-tStart);
         
            log_printf(LOG_INFO "NOPS_CMD_NOPS took %u milliseconds => DSP @%3u MHz.\n",
                       tDelta,
                       (sU32) (800.0f * (1000.0f / tDelta))
                       );
         }
      }
      
      /* Disconnect client */
      dsp_close();
   }
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {

   loc_nops();

   return 0;
}
