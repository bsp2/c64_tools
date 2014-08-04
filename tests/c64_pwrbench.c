/* ----
 * ---- file   : c64_pwrbench.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 15Dec2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "../include/inc_libc64.h"



/*--------------------------------------------------------------------------- module vars */
static sBool b_running;

static sUI iter;


/*--------------------------------------------------------------------------- loc_sigint() */
static void loc_sigint(int _sig) {

   log_printf(LOG_INFO "c64_pwrbench: caught signal %d after %u iterations\n", _sig, iter);

   if(SIGINT == _sig)
   {
      b_running = S_FALSE;
   }
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret;
   sU32 numIterations = 0;

   if(argc != 2)
   {
      printf("usage: %s <num_iterations>\n", argv[0]);
      return 0;
   }
   else
   {
      sscanf(argv[1], "%u", &numIterations);
   }

   /* Open client connection */
   ret = dsp_open();

   if(0 == ret)
   {
      iter = 0;
      
      log_printf(LOG_INFO "c64_pwrbench: numIterations=%u\n",
                 numIterations
                 );

      signal(SIGINT, &loc_sigint); 
      
      b_running = S_TRUE;

      {
         sU32 tStart = osal_milliseconds_get();
         sU32 tDelta;
      
         while(b_running)
         {
            /* DSP power-off request */
            dsp_suspend();

            //osal_msleep(1);
            
            /* DSP power-on request */
            dsp_resume();
            
            iter++;
            
            if(0 != numIterations)
            {
               if(iter == numIterations)
               {
                  break;
               }
            } /* for;; */
         }

         tDelta = osal_milliseconds_get() - tStart;

         log_printf(LOG_INFO "c64_pwrbench: %u iterations in %u milliseconds (=> %f ms per power cycle)\n",
                    iter,
                    tDelta,
                    ((sF32)tDelta) / iter
                    );
      }

      /* Disconnect client */
      dsp_close();

   } /* if dsp_open() */

   return ret;
}
