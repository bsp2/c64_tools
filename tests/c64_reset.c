/* ----
 * ---- file   : c64_reset.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 15Jan2014
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/inc_libc64_mini.h"

#include "../include/osal_err.h"
#include "../include/osal.h"

#include "../include/dsp_err.h"
#include "../include/dsp_priv.h"



/*--------------------------------------------------------------------------- module vars */
static sUI num_iterations;
static sUI sleep_millisec;


/*--------------------------------------------------------------------------- usage() */
static void usage(void) {
   printf(
      "usage: c64_reset <num_iterations> <sleep_millisec>\n"
      "\n"
      "\tnum_iterations: how many times the DSP will be powered on and off\n"
      "\tsleep_millisec: num. millisecs to wait between power on and off\n"
      "\n"
          );
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret;

   if(3 != argc)
   {
      usage();
      
      return 0;
   }

   sscanf(argv[1], "%u", &num_iterations);
   sscanf(argv[2], "%u", &sleep_millisec);

   printf("[...] c64_reset: num_iterations=%u sleep_millisec=%u\n",
          num_iterations,
          sleep_millisec
          );

   /* Don't boot the DSP during dsp_open() */
   dsp_autoresume_set(S_FALSE);

   /* Open client connection */
   ret = dsp_open();

   if(0 == ret)
   {
      sUI iter = 0;

      for(;;)
      {
         /* Power on the DSP and keep it in reset
          *  (no DSP code will be executed)
          */
         (void)dsp_power_on_and_reset();

         /* Some delay */
         osal_msleep(sleep_millisec);

         /* Power off DSP */
         (void)dsp_suspend();

         /* Some delay */
         osal_msleep(sleep_millisec);

         if(0 != num_iterations)
         {
            if(++iter == num_iterations)
            {
               break;
            }
         }
      }

      /* Disconnect client */
      dsp_close();

   } /* if dsp_open() */

   return ret;
}
