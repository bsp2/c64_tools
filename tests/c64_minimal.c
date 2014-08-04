/* ----
 * ---- file   : c64_minimal.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 20Oct2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/inc_libc64_mini.h"

#include <components/demo_calc/demo_calc.h>


/*--------------------------------------------------------------------------- module vars */
static dsp_component_id_t compid_demo_calc;


/*--------------------------------------------------------------------------- loc_lazy_load_components() */
sBool loc_lazy_load_components(void) {
   sBool ret = S_FALSE;

   /* Dynamically load "demo_calc" DSP component */
   if(0 == dsp_component_load(NULL, COMPONENT_NAME_DEMO_CALC, &compid_demo_calc))
   {
      /* Succeeded */
      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_rpc_add_test() */
static void loc_rpc_add_test(void) {
   /* Test single DSP remote procedure call */
   dsp_msg_t msg;
   dsp_msg_t reply;
   int ret;

   /* Initialize DSP remote procedure call message */
   DSP_MSG_INIT(&msg, compid_demo_calc, DEMO_CALC_CMD_ADD,
                42 + (rand() & 31),           // argument #1
                22 + (rand() & 31)            // argument #1
                );
   
   /* Send message to DSP and wait for reply */
   ret = dsp_rpc(&msg, &reply);
   
   if(0 == ret)
   {
      printf("[...] dsp_rpc() OK, result=%d (expected %d)\n", 
             *(sS32*) &reply.data[0].ret,   /* re-interpet unsigned int as signed */
             (msg.data[0].arg + msg.data[1].arg)
             );
   }
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret;

   /* Open client connection */
   ret = dsp_open();

   if(0 == ret)
   {
      if(loc_lazy_load_components())
      {
         loc_rpc_add_test();
      }
      else
      {
         /* Failed: loc_lazy_load_components() */
         printf("[---] failed to load/query required DSP components.\n");
      }
         
      /* Disconnect client */
      dsp_close();

   } /* if dsp_open() */

   return ret;
}
