/* ----
 * ---- file   : c64_sramtest.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          ARM fixpoint optimized code contributed by M-HT.
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 12Nov2013, 13Nov2013, 14Nov2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <unistd.h>

#include "../include/inc_libc64.h"
#include "../include/arch/dm3730/dm3730.h"

#include <core/components.h>
#include <components/sramtest/sramtest.h>


/*--------------------------------------------------------------------------- module vars */
static dsp_component_id_t compid_sramtest;


/*--------------------------------------------------------------------------- loc_lazy_load_components() */
sBool loc_lazy_load_components(void) {
   sBool ret;

   ret = S_FALSE;

   if(0 == dsp_component_load(NULL, COMPONENT_NAME_SRAMTEST, &compid_sramtest))
   {
      /* Succeeded */
      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_l1sram_r_speed() */
static void loc_l1sram_r_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_R_NUM_ITERATIONS  (200000u)

   log_printf(LOG_INFO "starting L1 SRAM read speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_R_SPEED,
                L1SRAM_R_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_R_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_R_NUM_ITERATIONS * 24.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
      
}


/*--------------------------------------------------------------------------- loc_l1sram_w_speed() */
static void loc_l1sram_w_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;
   
#define L1SRAM_W_NUM_ITERATIONS  (200000u)

   log_printf(LOG_INFO "starting L1 SRAM read speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_W_SPEED,
                L1SRAM_W_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_W_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_W_NUM_ITERATIONS * 24.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
}


/*--------------------------------------------------------------------------- loc_l1sram_rw_speed() */
static void loc_l1sram_rw_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_RW_NUM_ITERATIONS  (50000u)

   log_printf(LOG_INFO "starting L1 SRAM read/write speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_RW_SPEED,
                L1SRAM_RW_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_RW_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_RW_NUM_ITERATIONS * 12.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
}


/*--------------------------------------------------------------------------- loc_l1sram_r2w2_speed() */
static void loc_l1sram_r2w2_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_R2W2_NUM_ITERATIONS  (50000u)

   log_printf(LOG_INFO "starting L1 SRAM read2/write2 speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_R2W2_SPEED,
                L1SRAM_R2W2_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
         
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_R2W2_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_R2W2_NUM_ITERATIONS * 12.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );

}


/*--------------------------------------------------------------------------- loc_l1sram_r4w4_speed() */
static void loc_l1sram_r4w4_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_R4W4_NUM_ITERATIONS  (50000u)

   log_printf(LOG_INFO "starting L1 SRAM read4/write4 speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_R4W4_SPEED,
                L1SRAM_R4W4_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
         
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_R4W4_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_R4W4_NUM_ITERATIONS * 12.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );

}


/*--------------------------------------------------------------------------- loc_l1sram_r6w6_speed() */
static void loc_l1sram_r6w6_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_R6W6_NUM_ITERATIONS  (50000u)

   log_printf(LOG_INFO "starting L1 SRAM read6/write6 speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_R6W6_SPEED,
                L1SRAM_R6W6_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
         
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_R6W6_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_R6W6_NUM_ITERATIONS * 12.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );

}


/*--------------------------------------------------------------------------- loc_l1sram_r8w8_speed() */
static void loc_l1sram_r8w8_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L1SRAM_R8W8_NUM_ITERATIONS  (50000u)

   log_printf(LOG_INFO "starting L1 SRAM read8/write8 speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_R8W8_SPEED,
                L1SRAM_R8W8_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
         
   log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_R8W8_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L1SRAM_R8W8_NUM_ITERATIONS * 12.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );

}


/*--------------------------------------------------------------------------- loc_l1sram_rwram_speed() */
static void loc_l1sram_rwram_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;
   dsp_mem_region_t shm;

#define L1SRAM_RWRAM_NUM_ITERATIONS  (30000u)

   log_printf(LOG_INFO "starting L1 SRAM-read / RAM-write speed test\n");
   
   shm = dsp_shm_alloc(DSP_CACHE_NONE, 24*1024);
   
   if(shm.size > 0)
   {
      tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_RWRAM_SPEED,
                   L1SRAM_RWRAM_NUM_ITERATIONS, 0
                   );
      
      dsp_rpc(&msg, NULL);
      
      tEnd = osal_milliseconds_get();
      
      log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_RWRAM_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
                 ( ((L1SRAM_RWRAM_NUM_ITERATIONS * 24.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
                 );
      
      dsp_shm_free(shm);
   }
}


/*--------------------------------------------------------------------------- loc_l1sram_rwramdma_speed() */
static void loc_l1sram_rwramdma_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;
   dsp_mem_region_t shm;
   
#define L1SRAM_RWRAMDMA_NUM_ITERATIONS  (100000u)

   log_printf(LOG_INFO "starting L1 SRAM-read+write + parallel RAM-DMA-write speed test\n");
   
   shm = dsp_shm_alloc(DSP_CACHE_NONE, (24u*1024u / 4u));
   
   if(shm.size > 0)
   {
      tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L1SRAM_RWRAMDMA_SPEED,
                   L1SRAM_RWRAMDMA_NUM_ITERATIONS, 0
                   );
      
      dsp_rpc(&msg, NULL);
      
      tEnd = osal_milliseconds_get();
      
      log_printf(LOG_INFO "SRAMTEST_CMD_L1SRAM_RWRAMDMA_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
                 ( ((L1SRAM_RWRAMDMA_NUM_ITERATIONS * (24.0f * 1024.0f) / 4.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
                 );
      
      dsp_shm_free(shm);
   }
}


/*--------------------------------------------------------------------------- loc_l2sram_r_speed() */
static void loc_l2sram_r_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L2SRAM_R_NUM_ITERATIONS  (30000u)

   log_printf(LOG_INFO "starting L2 SRAM read speed test\n");
            
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L2SRAM_R_SPEED,
                L2SRAM_R_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L2SRAM_R_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L2SRAM_R_NUM_ITERATIONS * 96.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
}


/*--------------------------------------------------------------------------- loc_l2sram_rw_speed() */
static void loc_l2sram_rw_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L2SRAM_RW_NUM_ITERATIONS  (7000u)

   log_printf(LOG_INFO "starting L2 SRAM read/write speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L2SRAM_RW_SPEED,
                L2SRAM_RW_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L2SRAM_RW_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L2SRAM_RW_NUM_ITERATIONS * 48.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
}


/*--------------------------------------------------------------------------- loc_l2sram_r1_w2_speed() */
static void loc_l2sram_r1_w2_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;

#define L2SRAM_R1_W2_NUM_ITERATIONS  (10000u)

   log_printf(LOG_INFO "starting SRAM read L1, write L2 speed test\n");
   
   tStart = osal_milliseconds_get();
   
   DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L2SRAM_R1_W2_SPEED,
                L2SRAM_R1_W2_NUM_ITERATIONS, 0
                );
   
   dsp_rpc(&msg, NULL);
   
   tEnd = osal_milliseconds_get();
   
   log_printf(LOG_INFO "SRAMTEST_CMD_L2SRAM_R1_W2_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
              ( ((L2SRAM_R1_W2_NUM_ITERATIONS * 48.0f * 1024.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
              );
}


/*--------------------------------------------------------------------------- loc_l2sram_r1_w2ramdma_speed() */
static void loc_l2sram_r1_w2ramdma_speed(void) {
   dsp_msg_t msg;
   sU32 tStart;
   sU32 tEnd;
   dsp_mem_region_t shm;

#define L2SRAM_R1_W2RAMDMA_NUM_ITERATIONS  (40000u)

   log_printf(LOG_INFO "starting SRAM read L1, write L2 + RAM via DMA in parallel speed test\n");
   
   shm = dsp_shm_alloc(DSP_CACHE_NONE, (24u*1024u / 2u));
   
   if(shm.size > 0)
   {
      tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_sramtest, SRAMTEST_CMD_L2SRAM_R1_W2RAMDMA_SPEED,
                   L2SRAM_R1_W2RAMDMA_NUM_ITERATIONS, 0
                   );
      
      dsp_rpc(&msg, NULL);
      
      tEnd = osal_milliseconds_get();
      
      log_printf(LOG_INFO "SRAMTEST_CMD_L2SRAM_R1_W2RAMDMA_SPEED took %u milliseconds (=>%.3f MBytes/sec).\n", (tEnd-tStart),
                 ( ((L2SRAM_R1_W2RAMDMA_NUM_ITERATIONS * (24.0f * 1024.0f) / 2.0f) / ((tEnd-tStart) / 1000.0f)) / (1024.0f*1024.0f) )
                 );
      
      dsp_shm_free(shm);
   }
}


/*--------------------------------------------------------------------------- loc_usage() */
static void loc_usage(void) {
   printf(
      "Usage: ./c64_sramtest <arg>\n"
      "\n"
      "   arg:\n"
      "         -l1sramr: run L1SRAM read speed test\n"
      "         -l1sramw: run L1SRAM write speed test\n"
      "        -l1sramrw: run L1SRAM read/write speed test\n"
      "      -l1sramr2w2: run L1SRAM read2/write2 speed test\n"
      "      -l1sramr4w4: run L1SRAM read4/write4 speed test\n"
      "      -l1sramr6w6: run L1SRAM read6/write6 speed test\n"
      "      -l1sramr8w8: run L1SRAM read8/write8 speed test\n"
      "     -l1sramrwram: run L1SRAM read / RAM-write speed test\n"
      "  -l1sramrwramdma: run L1SRAM read/write + parallel RAM-DMA-write speed test\n"
      "\n"
      "         -l2sramr: run L2SRAM read speed test\n"
      "        -l2sramrw: run L2SRAM read/write speed test\n"
      "      -l2sramr4w4: run L2SRAM read4/write4 speed test\n"
      "      -l2sramr1w2: run L2SRAM read L1, write L2 speed test\n"
      "-l2sramr1w2ramdma: run L2SRAM read L1, write L2 + RAM via DMA in parallel speed test\n"
      "\n"
          );
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {

   if(argc > 1)
   {
      int ret;

      /* Open client connection */
      ret = dsp_open();
   
      if(0 == ret)
      {
         if(loc_lazy_load_components())
         {

            if(!strcmp(argv[1], "-l1sramr"))
            {
               loc_l1sram_r_speed();
               
               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramw"))
            {
               loc_l1sram_w_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramrw"))
            {
               loc_l1sram_rw_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramr2w2"))
            {
               loc_l1sram_r2w2_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramr4w4"))
            {
               loc_l1sram_r4w4_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramr6w6"))
            {
               loc_l1sram_r6w6_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramr8w8"))
            {
               loc_l1sram_r8w8_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramrwram"))
            {
               loc_l1sram_rwram_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l1sramrwramdma"))
            {
               loc_l1sram_rwramdma_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l2sramr"))
            {
               loc_l2sram_r_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l2sramrw"))
            {
               loc_l2sram_rw_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l2sramr1w2"))
            {
               loc_l2sram_r1_w2_speed();

               return 0;
            }
            else if(!strcmp(argv[1], "-l2sramr1w2ramdma"))
            {
               loc_l2sram_r1_w2ramdma_speed();

               return 0;
            }

         } /* if load comp. */

         /* Disconnect client */
         dsp_close();

      } /* if dsp_open() */

   }
   else
   {
      loc_usage();
   }

   return 0;
}
