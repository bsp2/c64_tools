/* ----
 * ---- file   : c64_pwrtest.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 13Dec2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "../include/inc_libc64.h"

#include <components/demo_calc/demo_calc.h>


/*--------------------------------------------------------------------------- module vars */
static dsp_component_id_t compid_demo_calc;

static sBool b_running;

static sUI iter;


/*--------------------------------------------------------------------------- loc_sigint() */
static void loc_sigint(int _sig) {

   log_printf(LOG_INFO "c64_pwrtest: caught signal %d after %u iterations\n", _sig, iter);

   if(SIGINT == _sig)
   {
      b_running = S_FALSE;
   }
}


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
      printf("[...] [%04u] dsp_rpc() OK, result=%d (expected %d)\n", 
             iter,
             *(sS32*) &reply.data[0].ret,   /* re-interpet unsigned int as signed */
             (msg.data[0].arg + msg.data[1].arg)
             );
   }
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret;
   sU32 msPwrOn;
   sU32 msPwrOff;
   sU32 numIterations = 0;

   if(argc > 1)
   {
      if(argc < 3)
      {
         printf("usage: %s <msec_pwron> <msec_pwroff> [num_iterations]\n", argv[0]);
         return 0;
      }
      else
      {
         sscanf(argv[1], "%u", &msPwrOn);
         sscanf(argv[2], "%u", &msPwrOff);
         
         if(argc > 3)
         {
            sscanf(argv[3], "%u", &numIterations);
         }
      }
   }
   else
   {
      msPwrOn = 50;
      msPwrOff = 1000;
      numIterations = 5;
   }

   /* Open client connection */
   ret = dsp_open();

   if(0 == ret)
   {
      if(loc_lazy_load_components())
      {
         iter = 0;

         log_printf("c64_pwrtest: msPwrOn=%u msPwrOff=%u numIterations=%u\n",
                    msPwrOn,
                    msPwrOff,
                    numIterations
                    );

         signal(SIGINT, &loc_sigint); 

         b_running = S_TRUE;

         while(b_running)
         {
            /* Do sth. with the DSP */
            loc_rpc_add_test();

            /* Sleep for a while */
            osal_msleep(msPwrOn);

            /* DSP power-off request */
            dsp_suspend();

            /* Sleep for a while */
            osal_msleep(msPwrOff);

            /* DSP power-on request */
            dsp_resume();

            iter++;

            if(0 != numIterations)
            {
               if(iter == numIterations)
               {
                  break;
               }
            }
         } /* for;; */
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
