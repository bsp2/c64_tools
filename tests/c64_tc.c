/* ----
 * ---- file   : c64_tc.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 12Sep2013, 15Sep2013, 16Sep2013, 17Sep2013, 18Sep2013, 19Sep2013, 20Sep2013
 * ----          23Sep2013, 01Oct2013, 02Oct2013, 03Oct2013, 04Oct2013, 05Oct2013, 09Oct2013
 * ----          15Oct2013, 16Oct2013, 18Oct2013, 19Oct2013, 21Oct2013, 22Oct2013, 23Oct2013
 * ----          01Nov2013, 02Nov2013, 08Dec2013, 11Dec2013, 13Dec2013, 09Jan2014, 10Jan2014
 * ----
 * ----
 */

/* If defined, include "dsp_priv.h" for some internal tests */
#define USE_DSP_PRIV defined


#define NUM_BENCHMARK_ITERATIONS (100000u)
//#define NUM_BENCHMARK_ITERATIONS (10u)


#define NUM_FASTCALL_BENCHMARK_ITERATIONS (1000000u)


#define NUM_L1SRAM_ITERATIONS (10000000u)
#define L1SRAM_REG(a) (*(volatile sU32*)(l1sram_scratch.virt_addr + (a)))


//#define TC_SHM_ACCESS_SIZE  (4u * 1024u * 2)
#define TC_SHM_ACCESS_SIZE  (4u)
//#define TC_SHM_ACCESS_SIZE  (1u * 1024u * 1024u)


//#define TC_SHM_SPEEDCHECK_SIZE  (8u * 1024u * 1024u)
#define TC_SHM_SPEEDCHECK_SIZE  (3u * 1024u * 1024u)
//#define TC_SHM_SPEEDCHECK_ITERATIONS  (100u)
#define TC_SHM_SPEEDCHECK_ITERATIONS  (40u)


#define TC_SHM_MSPACE_SIZE (2u * 1024u * 1024u)


#define TC_QDMA_COPY1D_SIZE  (64u)

#define TC_QDMA_COPY2D_SRC_PITCH (2 * sizeof(sU32))
#define TC_QDMA_COPY2D_SRC_NUM_Y (2)
#define TC_QDMA_COPY2D_SRC_SIZE  (TC_QDMA_COPY2D_SRC_PITCH * TC_QDMA_COPY2D_SRC_NUM_Y)
#define TC_QDMA_COPY2D_DST_PITCH (4 * sizeof(sU32))
#define TC_QDMA_COPY2D_DST_NUM_Y (4)
#define TC_QDMA_COPY2D_DST_SIZE  (TC_QDMA_COPY2D_DST_PITCH * TC_QDMA_COPY2D_DST_NUM_Y)

#define TC_QDMA_LINK1D_SIZE  (64u)

#define TC_QDMA_LINK2D_SRC_PITCH (2 * sizeof(sU32))
#define TC_QDMA_LINK2D_SRC_NUM_Y (2)
#define TC_QDMA_LINK2D_SRC_SIZE  (TC_QDMA_LINK2D_SRC_PITCH * TC_QDMA_LINK2D_SRC_NUM_Y * TEST_QDMA_MAX_LINKS)
#define TC_QDMA_LINK2D_DST_PITCH (4 * sizeof(sU32))
#define TC_QDMA_LINK2D_DST_NUM_Y (4)
#define TC_QDMA_LINK2D_DST_SIZE  (TC_QDMA_LINK2D_DST_PITCH * TC_QDMA_LINK2D_DST_NUM_Y * TEST_QDMA_MAX_LINKS)

//#define TC_SHM_CACHE_RAND_SIZE  (6u * 1024u * 1024u)
#define TC_SHM_CACHE_RAND_SIZE  (4 * 4u * 1024u)

#define NUM_TC_SHM_CACHE_RAND_ITERATIONS  (10000u)


/* Number of 2MB huge pages for TC_SHM_HUGETLB_V2P2V */
#define TC_SHM_HUGETLB_NUM_PAGES 8


#define TC_L2SRAM_RAND_CHKSUM_SIZE            (1024u * 64u)
#define NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS  (1000u)


// Testcase enumeration:
#define TC_NONE                         ( 0u)
#define TC_TESTMSG                      ( 1u)  // Send 4 byte test message to DSP, bypassing client handlers
#define TC_RPC_ADD                      ( 2u)  // Perform single remote procedure call (add two signed 32bit ints)
#define TC_RPC_ADD_BENCHMARK            ( 3u)  // RPC stresstest, repeatedly calls 'add' cmd of 'demo_calc' component
#define TC_IOCTL_TEST                   ( 4u)  // Call C64_IOCTL_TEST (debugging)
#define TC_SHM_ACCESS                   ( 5u)  // Tests shared memory access (read/write)
#define TC_RPC_ADD_FASTCALL_BENCHMARK   ( 6u)
#define TC_L1SRAM_SPEED                 ( 7u)  // Benchmarks L1DSRAM access speed (from GPP side)
#define TC_PWR_OFF                      ( 8u)
#define TC_RESUME                       ( 9u)
#define TC_IDLE                         (10u) // Keeps the DSP powered on, idling around for 30 seconds.
#define TC_SHM_CACHE_NONE               (11u)
#define TC_SHM_CACHE_RW                 (12u)
#define TC_SHM_CACHE_R                  (13u)
#define TC_SHM_CACHE_W                  (14u)
#define TC_L1SRAM_ALLOCATOR             (15u)
#define TC_SHM_MSPACE                   (16u)
#define TC_CTRLC                        (17u)
#define TC_FASTCALL_CTRLC               (18u)
#define TC_LOGBUF                       (19u)
#define TC_LOGBUF_MANY                  (20u)
#define TC_LOGBUF_SPLITLINE             (21u)
#define TC_LOGBUF_NO_LF                 (22u)
#define TC_QDMA_COPY1D                  (23u)
#define TC_QDMA_COPY2D                  (24u)
#define TC_SHM_CACHE_NONE_RAND_DSP      (25u) // DSP generates random data, GPP validates it.
#define TC_SHM_CACHE_RW_RAND_DSP        (26u) // 
#define TC_SHM_CACHE_R_RAND_DSP         (27u) // 
#define TC_SHM_CACHE_W_RAND_DSP         (28u) // 
#define TC_SHM_CACHE_NONE_RAND_GPP      (29u) // GPP generates random data, DSP validates it.
#define TC_SHM_CACHE_RW_RAND_GPP        (30u) // 
#define TC_SHM_CACHE_R_RAND_GPP         (31u) // 
#define TC_SHM_CACHE_W_RAND_GPP         (32u) // 
#define TC_SHM_CACHE_NONE_RAND_DSP_VIRT (33u) // DSP generates random data, GPP validates it.
#define TC_SHM_CACHE_RW_RAND_DSP_VIRT   (34u) // 
#define TC_SHM_CACHE_R_RAND_DSP_VIRT    (35u) // 
#define TC_SHM_CACHE_W_RAND_DSP_VIRT    (36u) // 
#define TC_SHM_CACHE_NONE_RAND_GPP_VIRT (37u) // GPP generates random data, DSP validates it.
#define TC_SHM_CACHE_RW_RAND_GPP_VIRT   (38u) // 
#define TC_SHM_CACHE_R_RAND_GPP_VIRT    (39u) // 
#define TC_SHM_CACHE_W_RAND_GPP_VIRT    (40u) // 
#define TC_SHM_CACHE_HUGETLB_V2P2V      (41u) // (note) must run "/usr/pandora/scripts/op_hugetlb.sh 16" first
#define TC_SHM_CACHE_HUGETLB            (42u) // (note) must run "/usr/pandora/scripts/op_hugetlb.sh 16" first
#define TC_L2SRAM_ALLOCATOR             (43u)
#define TC_QDMA_LINK1D                  (44u)
#define TC_QDMA_LINK2D                  (45u)
#define TC_L2SRAM_RAND_CHKSUM_DSP       (46u) // DSP generates random data, GPP validates it
#define TC_L2SRAM_RAND_CHKSUM_GPP       (47u) // GPP generates random data, DSP validates it
#define TC_L2SRAM_RAND_CHKSUM_DSP_FC    (48u) // DSP generates random data, GPP validates it, uses fastcalls
#define TC_L2SRAM_RAND_CHKSUM_GPP_FC    (49u) // GPP generates random data, DSP validates it, uses fastcalls

#define TC_NUM                          (50u)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "../include/inc_libc64.h"

#ifdef USE_DSP_PRIV
#include "../include/dsp_priv.h"
#endif /* USE_DSP_PRIV */

/* resident/firmware components: */
#include <core/components.h>

/* dynamically loaded overlay components: */
#include <components/demo_calc/demo_calc.h>
#include <components/demo_checksum/demo_checksum.h>
#include <components/demo_calc_fastcall/demo_calc_fastcall.h>
#include <components/test_logbuf/test_logbuf.h>
#include <components/test_qdma/test_qdma.h>


#if 0
void loc_tc_memperf_nodsp(void);
#endif


/*--------------------------------------------------------------------------- module vars */
#define TC_STR(a) #a
static const char *tc_names[TC_NUM] = {
   TC_STR(TC_NONE),
   TC_STR(TC_TESTMSG),
   TC_STR(TC_RPC_ADD),
   TC_STR(TC_RPC_ADD_BENCHMARK),
   TC_STR(TC_IOCTL_TEST),
   TC_STR(TC_SHM_ACCESS),
   TC_STR(TC_FASTCALL_RPC_ADD),
   TC_STR(TC_L1SRAM_SPEED),
   TC_STR(TC_PWR_OFF),
   TC_STR(TC_RESUME),
   TC_STR(TC_IDLE),
   TC_STR(TC_SHM_CACHE_NONE),
   TC_STR(TC_SHM_CACHE_RW),
   TC_STR(TC_SHM_CACHE_R),
   TC_STR(TC_SHM_CACHE_W),
   TC_STR(TC_L1SRAM_ALLOCATOR),
   TC_STR(TC_SHM_MSPACE),
   TC_STR(TC_CTRLC),
   TC_STR(TC_FASTCALL_CTRLC),
   TC_STR(TC_LOGBUF),
   TC_STR(TC_LOGBUF_MANY),
   TC_STR(TC_LOGBUF_SPLITLINE),
   TC_STR(TC_LOGBUF_NO_LF),
   TC_STR(TC_QDMA_COPY1D),
   TC_STR(TC_QDMA_COPY2D),
   TC_STR(TC_SHM_CACHE_NONE_RAND_DSP),
   TC_STR(TC_SHM_CACHE_RW_RAND_DSP),
   TC_STR(TC_SHM_CACHE_R_RAND_DSP),
   TC_STR(TC_SHM_CACHE_W_RAND_DSP),
   TC_STR(TC_SHM_CACHE_NONE_RAND_GPP),
   TC_STR(TC_SHM_CACHE_RW_RAND_GPP),
   TC_STR(TC_SHM_CACHE_R_RAND_GPP),
   TC_STR(TC_SHM_CACHE_W_RAND_GPP),
   TC_STR(TC_SHM_CACHE_NONE_RAND_DSP_VIRT),
   TC_STR(TC_SHM_CACHE_RW_RAND_DSP_VIRT),
   TC_STR(TC_SHM_CACHE_R_RAND_DSP_VIRT),
   TC_STR(TC_SHM_CACHE_W_RAND_DSP_VIRT),
   TC_STR(TC_SHM_CACHE_NONE_RAND_GPP_VIRT),
   TC_STR(TC_SHM_CACHE_RW_RAND_GPP_VIRT),
   TC_STR(TC_SHM_CACHE_R_RAND_GPP_VIRT),
   TC_STR(TC_SHM_CACHE_W_RAND_GPP_VIRT),
   TC_STR(TC_SHM_CACHE_HUGETLB_V2P2V),
   TC_STR(TC_SHM_CACHE_HUGETLB),
   TC_STR(TC_L2SRAM_ALLOCATOR),
   TC_STR(TC_QDMA_LINK1D),
   TC_STR(TC_QDMA_LINK2D),
   TC_STR(TC_L2SRAM_RAND_CHKSUM_DSP),
   TC_STR(TC_L2SRAM_RAND_CHKSUM_GPP),
   TC_STR(TC_L2SRAM_RAND_CHKSUM_DSP_FC),
   TC_STR(TC_L2SRAM_RAND_CHKSUM_GPP_FC)
};
#undef TC_STR


static dsp_mem_region_t l1sram_scratch;

static dsp_component_id_t compid_demo_calc;
static dsp_component_id_t compid_demo_checksum;
static dsp_component_id_t compid_demo_calc_fastcall;
static dsp_component_id_t compid_test_logbuf;
static dsp_component_id_t compid_test_qdma;

static sUI test_cycle_idx;
static sUI test_min_tc_idx;
static sUI test_max_tc_idx;
static sUI test_num_cycles; // 0=infinite
static sUI total_num_tests;


/*--------------------------------------------------------------------------- usage() */
static void usage(void) {
   sUI tcIdx;
   
   printf("Usage:\n");
   printf("  c64_tc [testcase_nr] [[max_testcase_nr] [num_test_cycles]]\n\n");
   printf("    testcase_nr can be one of:\n");
   
   for(tcIdx=1; tcIdx < TC_NUM; tcIdx++)
   {
      printf("\t\t%02u: %s\n",
             tcIdx,
             tc_names[tcIdx]
             );
   }
   
   puts("");
}


/*--------------------------------------------------------------------------- loc_tc_testmsg() */
static void loc_tc_testmsg(void) {
   /* Send a test message bypassing the client handler in the c64 kernel module 
    *
    *  This is just to see whether the GPP<>DSP messaging in general works OK. 
    *
    *   (note) the kernel module should have been loaded with "debug=20", i.e.
    *           $ /sbin/insmod c64.ko debug=20
    */
#ifdef USE_DSP_PRIV
   dsp_send_forced_message(0x37703370u);
#else
   log_printf(LOG_WARN "loc_tc_testmsg: skipping test, c64_tc was compiled w/o USE_DSP_PRIV\n");
#endif /* USE_DSP_PRIV */
}


/*--------------------------------------------------------------------------- loc_tc_rpc_add_test() */
static sBool loc_tc_rpc_add_test(void) {
   /* Test single DSP remote procedure call */
   dsp_msg_t msg;
   dsp_msg_t reply;
   int ret;
   sBool r;
   
   if(LOG_DEBUG_LEVEL(10))
   {
      log_printf(LOG_DEBUG "loc_tc_rpc_add_test: sizeof(dsp_msg_t) == %u\n", sizeof(dsp_msg_t)); // must be 12 bytes
   }
   
   msg.req.fields.dsp_component_id = compid_demo_calc;   // selects "demo_calc" component on DSP-side
   msg.req.fields.dsp_cmd_id       = DEMO_CALC_CMD_ADD;  // selects the component's 'ADD' command
   
   msg.data[0].arg = 42 + (rand()&31);             // argument #1
   msg.data[1].arg = 22 + (rand()&31);             // argument #2
   
   ret = dsp_rpc(&msg, &reply);
   
   if(LOG_DEBUG_LEVEL(10))
   {
#ifdef USE_DSP_PRIV
      dsp_debug_internal_print();
#endif

      dsp_debug_usr_print(0);
      dsp_debug_usr_print(1);
      dsp_debug_usr_print(2);
      dsp_debug_usr_print(3);
   }
  
   if(0 == ret)
   {
      log_printf(LOG_INFO "dsp_rpc() OK, result=%d (expected %d)\n", 
                 *(sS32*) &reply.data[0].ret,   /* re-interpet unsigned int as signed */
                 (msg.data[0].arg + msg.data[1].arg)
                 );

#ifdef USE_DSP_PRIV      
      if(LOG_DEBUG_LEVEL(10))
      {
         dsp_debug_internal_print();
      }
#endif /* USE_DSP_PRIV */

      /* Succceeded */
      r = S_TRUE;
   }
   else
   {
      log_printf(LOG_ERROR "dsp_rpc() failed (ret=%d)\n", ret);

      r = S_FALSE;
   }

   return r;
}


/*--------------------------------------------------------------------------- loc_tc_rpc_add_benchmark() */
static sBool loc_tc_rpc_add_benchmark(void) {
   sBool ret;

   log_printf(LOG_INFO "starting benchmark\n");
   {
      dsp_msg_t msg;
      dsp_msg_t reply;
      sUI       tStart = osal_milliseconds_get();
      sUI       tEnd;
      sUI       tDelta;
      sUI       i;
      int       r;

      msg.req.fields.dsp_component_id = compid_demo_calc;   // selects "demo_calc" component on DSP-side
      msg.req.fields.dsp_cmd_id       = DEMO_CALC_CMD_ADD;  // selects the component's 'ADD' command
      
      msg.data[0].arg = 42;                           // argument #1
      msg.data[1].arg = 0;                            // argument #2

      /* Enable polling mode */
      dsp_poll_enable(S_TRUE);

      for(i=0; i<NUM_BENCHMARK_ITERATIONS; i++)
      {
         r = dsp_rpc(&msg, &reply);

         if(0 == r)
         {
            if(reply.data[0].ret != (msg.data[0].arg + msg.data[1].arg))
            {
               log_printf(LOG_ERROR "iteration=%u: unexpected RPC result: have=0x%08x, expected=0x%08x\n",
                          i,
                          reply.data[0].ret,
                          (msg.data[0].arg + msg.data[1].arg)
                          );

#ifdef USE_DSP_PRIV               
               dsp_debug_internal_print();
#endif /* USE_DSP_PRIV */
               break;
            }
         }
         else
         {
            log_printf(LOG_ERROR "iteration=%u: dsp_rpc() failed (r=%d).\n", r);

#ifdef DSP_USE_PRIV
            dsp_debug_internal_print();
#endif /* DSP_USE_PRIV */
            break;
         }


         /* Next iteration */
         msg.data[0].arg++;
         msg.data[1].arg++;
      }

      /* Disable polling mode */
      dsp_poll_enable(S_FALSE);

      ret = (NUM_BENCHMARK_ITERATIONS == i);

      if(ret)
      {

         tEnd = osal_milliseconds_get();
         
         tDelta = tEnd - tStart;
         
         log_printf(LOG_INFO "benchmark finished. => %u iterations in %u millisecs.\n",
                    NUM_BENCHMARK_ITERATIONS,
                    tDelta
                    );
      }
      else
      {
         log_printf(LOG_ERROR "(benchmark aborted due to error after %u iterations)\n", i);
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_shm_access_test() */
static sBool loc_tc_shm_access_test(void) {
   sUI i;
   sU32 *d;
   int ret;
   sU32 chkGPP = 0;
   dsp_mem_region_t shm;
   sBool r = S_FALSE;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_RW, TC_SHM_ACCESS_SIZE);
   
   if(0 != shm.size)
   {
      d = (sU32*) shm.virt_addr;

      if(LOG_DEBUG_LEVEL(10))
      {
         dsp_debug_usr_print(0);
      }

      log_printf(LOG_INFO "loc_tc_shm_access_test: virtAddr=0x%p, trying to write..\n", d);

      for(i=0; i<(shm.size >> 2); i++)
      {
         d[i] = i;
         chkGPP += i;
      }

      log_printf(LOG_INFO "loc_tc_shm_access_test: OK, wrote %d dwords (%u kBytes).\n", i, (i >> 8));

      /* Writeback GPP caches so DSP can see the updated data */
#if 0
      ret = dsp_cache_wb(shm.phys_addr,
                         shm.size
                         );
#else
      ret = 0;
#endif

      if(0 == ret)
      {
         /* Now verify that DSP sees the same data by calling the DSP-side demo_checksum::add() method */
         sU32 chkDSP = 0xCCccCCccu;

         ret = dsp_rpc_2a_1r(compid_demo_checksum,
                             DEMO_CHECKSUM_CMD_ADD,
                             shm.phys_addr,
                             shm.size,
                             &chkDSP
                             );

#ifdef USE_DSP_PRIV
         if(LOG_DEBUG_LEVEL(10))
         {
            dsp_debug_internal_print();
         }
#endif /* USE_DSP_PRIV */

         if(chkDSP == chkGPP)
         {
            log_printf(LOG_INFO "loc_tc_shm_access_test: OK, DSP+GPP checksums match (chk=0x%08x)\n",
                       chkDSP
                       );

            /* Succeeded */
            r = S_TRUE;
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_shm_access_test: failed, checksum mismatch (gpp=0x%08x dsp=0x%08x)\n",
                       chkGPP,
                       chkDSP
                       );

            if(LOG_DEBUG_LEVEL(10))
            {
               dsp_debug_usr_print(0);
            }
         }
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_shm_access_test: loc_cache_wb() failed (ret=%d)\n", ret);
      }

      /* Free contiguous memory block (may omit this call, dsp_close() does this) */
      dsp_shm_free(shm);
         
   } /* if dsp_shm_alloc() */

   return r;
}


/*--------------------------------------------------------------------------- loc_tc_rpc_fastcall_add_benchmark() */
static sBool loc_tc_rpc_add_fastcall_benchmark(void) {
   sBool ret = S_FALSE;

   log_printf(LOG_INFO "starting fastcall benchmark\n");
   {
      dsp_msg_t msg;
      sUI       tStart = osal_milliseconds_get();
      sUI       tEnd;
      sUI       tDelta;
      sUI       i;
      int       r;

      msg.req.fields.dsp_component_id = compid_demo_calc_fastcall;   // selects "demo_calc_fastcall" component on DSP-side
      msg.req.fields.dsp_cmd_id       = DEMO_CALC_FASTCALL_INITIATE; // selects the component's 'INITIATE' command

      /* Initiate fastcall sequence */
      /* (note) do not care for args */
      r = dsp_rpc_send(&msg);

      log_printf(LOG_INFO "loc_tc_rpc_add_fastcall_benchmark: init rpc sent, r=%d\n", r);

      //dsp_debug_usr_print(0);

      if(0 == r)
      {
         sU32 arg1 = 42;
         sU32 arg2 = 0;

         for(i=0; i<NUM_FASTCALL_BENCHMARK_ITERATIONS; i++)
         {
            sU32 r1FC;
            sU32 r2FC;

            ////printf("xxx fastcall benchmark iter=%u\n", i);

            r = dsp_fastcall_rpc(DEMO_CALC_FASTCALL_FC_CMD_ADD,
                                 arg1, arg2,
                                 &r1FC, &r2FC
                                 );
            
            if(0 == r)
            {
               if(r1FC != (arg1 + arg2))
               {
                  log_printf(LOG_ERROR "iteration=%u: unexpected fastcall RPC result: have=0x%08x, expected=0x%08x\n",
                             i,
                             r1FC,
                             (arg1 + arg2)
                             );

#ifdef USE_DSP_PRIV
                  dsp_debug_internal_print();
#endif /* USE_DSP_PRIV */
                  break;
               }
            }
            else
            {
               log_printf(LOG_ERROR "iteration=%u: dsp_fastcall_rpc() failed (r=%d).\n", i, r);

#ifdef USE_DSP_PRIV               
               dsp_debug_internal_print();
#endif /* USE_DSP_PRIV */
               break;
            }


            /* Next iteration */
            arg1++;
            arg2++;
         }

         ret = (NUM_FASTCALL_BENCHMARK_ITERATIONS == i);

         if(ret)
         {
            tEnd = osal_milliseconds_get();
         
            tDelta = tEnd - tStart;
         
            log_printf(LOG_INFO "fastcall benchmark finished. => %u iterations in %u millisecs.\n",
                       NUM_FASTCALL_BENCHMARK_ITERATIONS,
                       tDelta
                       );
         }
         else
         {
            log_printf(LOG_ERROR "(fastcall benchmark aborted due to error after %u iterations)\n", i);
         }

         dsp_fastcall_end();
      }
      else
      {
         /* Failed: dsp_rpc_send() */
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_l1sram_speed() */
static sBool loc_tc_l1sram_speed(void) {
   sUI i;
   sUI tStart;
   sUI tEnd;
   sU32 readMem[16];
   sU32 chksum;
   sBool ret = S_FALSE;

   /* Write */
   log_printf(LOG_INFO "starting SRAM write benchmark..");

   tStart = osal_milliseconds_get();

   for(i=0; i<(NUM_L1SRAM_ITERATIONS >> 4u); i++)
   {
      L1SRAM_REG( 0) = 0x00000000u;
      L1SRAM_REG( 4) = 0x11111111u;
      L1SRAM_REG( 8) = 0x22222222u;
      L1SRAM_REG(12) = 0x33333333u;
                  
      L1SRAM_REG(16) = 0x44444444u;
      L1SRAM_REG(20) = 0x55555555u;
      L1SRAM_REG(24) = 0x66666666u;
      L1SRAM_REG(28) = 0x77777777u;
                  
      L1SRAM_REG(32) = 0x88888888u;
      L1SRAM_REG(36) = 0x99999999u;
      L1SRAM_REG(40) = 0xaaaaaaaau;
      L1SRAM_REG(44) = 0xbbbbbbbbu;
                  
      L1SRAM_REG(48) = 0xccccccccu;
      L1SRAM_REG(52) = 0xddddddddu;
      L1SRAM_REG(56) = 0xeeeeeeeeu;
      L1SRAM_REG(60) = 0xffffffffu;
   }

   tEnd = osal_milliseconds_get();

   log_printf("..DONE: %u 32bit write accesses in %u milliseconds\n",
              NUM_L1SRAM_ITERATIONS,
              (tEnd - tStart)
              );


   /* Read */
   log_printf(LOG_INFO "starting SRAM read benchmark..");

   tStart = osal_milliseconds_get();

   for(i=0; i<(NUM_L1SRAM_ITERATIONS >> 4u); i++)
   {
      readMem[ 0] = L1SRAM_REG( 0);
      readMem[ 1] = L1SRAM_REG( 4);
      readMem[ 2] = L1SRAM_REG( 8);
      readMem[ 3] = L1SRAM_REG(12);
                                      
      readMem[ 4] = L1SRAM_REG(16);
      readMem[ 5] = L1SRAM_REG(20);
      readMem[ 6] = L1SRAM_REG(24);
      readMem[ 7] = L1SRAM_REG(28);
                                
      readMem[ 8] = L1SRAM_REG(32);
      readMem[ 9] = L1SRAM_REG(36);
      readMem[10] = L1SRAM_REG(40);
      readMem[11] = L1SRAM_REG(44);
                                
      readMem[12] = L1SRAM_REG(48);
      readMem[13] = L1SRAM_REG(52);
      readMem[14] = L1SRAM_REG(56);
      readMem[15] = L1SRAM_REG(60);
   }

   tEnd = osal_milliseconds_get();

   /* Prevent compiler from optimization / discarding reads (maybe not necessary?) */
   chksum = 0;
   for(i=0; i<16; i++)
   {
      //printf("\nxxx readMem[%u] = 0x%08x\n", i, readMem[i]);
      chksum += readMem[i];
   }

   log_printf("..DONE: %u 32bit read accesses in %u milliseconds (chksum=0x%08x)\n",
              NUM_L1SRAM_ITERATIONS,
              (tEnd - tStart),
              chksum
              );

   ret = S_TRUE;

   return ret;
}


/*--------------------------------------------------------------------------- loc_speedcheck_exec() */
#define USE_S2 defined  /* (todo) remove, makes no difference */

static sU8 *speedcheck_drain;
static sU8 *speedcheck_s;
static sU8 *speedcheck_d;

#ifdef USE_S2
static sU8 *speedcheck_s2;
#endif

static dsp_mem_region_t speedcheck_shm = {0,0,0};


static void loc_speedcheck_exec(const char *_name, void(*_fxn)(void)) {
#define MBSEC  ( ( (1000.0f * (TC_SHM_SPEEDCHECK_SIZE / (1024.0f * 1024.0f))) / (tEnd - tStart) ) * TC_SHM_SPEEDCHECK_ITERATIONS )

   sUI iter;
   sUI tStart;
   sUI tEnd;

   memcpy(speedcheck_drain, speedcheck_drain + TC_SHM_SPEEDCHECK_SIZE, TC_SHM_SPEEDCHECK_SIZE);

   if(speedcheck_shm.size > 0)
   {
#if 0
      printf("xxx c64_tc: calling dsp_cache_inv. phys_addr=0x%08x, virt_addr=0x%08x size=0x%08x\n",
             speedcheck_shm.phys_addr,
             speedcheck_shm.virt_addr,
             speedcheck_shm.size
             );
#endif
      dsp_cache_inv_virt((void*)speedcheck_shm.virt_addr, speedcheck_shm.size);
   }

   log_printf(LOG_INFO "loc_tc_shm_speedcheck: %s", _name);
   
   tStart = osal_milliseconds_get();
   
   for(iter=0; iter < TC_SHM_SPEEDCHECK_ITERATIONS; iter++)
   {
      _fxn();
   }

   tEnd = osal_milliseconds_get();
   
   log_printf("..done: %u * %u kbytes in %u ms => %.3f MBytes/sec\n",
              TC_SHM_SPEEDCHECK_ITERATIONS,
              (TC_SHM_SPEEDCHECK_SIZE / 1024u),
              (tEnd - tStart),
              MBSEC
              );
}

static void loc_speedcheck_memset_paged(void) {
   memset(speedcheck_s, 0, TC_SHM_SPEEDCHECK_SIZE);
}

static void loc_speedcheck_fill32_paged(void) {
   sUI i;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      ((sU32*)speedcheck_s)[i] = 0;
   }
}

static void loc_speedcheck_memset_shm(void) {
   memset(speedcheck_d, 0, TC_SHM_SPEEDCHECK_SIZE);
}

static void loc_speedcheck_fill32_shm(void) {
   sUI i;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      ((sU32*)speedcheck_d)[i] = 0;
   }
}

static void loc_speedcheck_memcpy_paged_to_paged(void) {
#ifdef USE_S2
   memcpy(speedcheck_s, speedcheck_s2, TC_SHM_SPEEDCHECK_SIZE);
#else
   memcpy(speedcheck_s, speedcheck_s + TC_SHM_SPEEDCHECK_SIZE, TC_SHM_SPEEDCHECK_SIZE);
#endif /* USE_S2 */
}

static void loc_speedcheck_copy32_paged_to_paged(void) {
   sUI i;
#ifdef USE_S2
   sU32 *s = (sU32*)speedcheck_s2;
#else
   sU32 *s = (sU32*)(speedcheck_s + TC_SHM_SPEEDCHECK_SIZE);
#endif /* USE_S2 */
   sU32 *d = (sU32*)speedcheck_s;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      d[i] = s[i];
   }
}

static void loc_speedcheck_memcpy_shm_to_shm(void) {
   memcpy(speedcheck_d, speedcheck_d + TC_SHM_SPEEDCHECK_SIZE, TC_SHM_SPEEDCHECK_SIZE);
}

static void loc_speedcheck_copy32_shm_to_shm(void) {
   sUI i;
   sU32 *s = (sU32*)(speedcheck_d + TC_SHM_SPEEDCHECK_SIZE);
   sU32 *d = (sU32*)speedcheck_d;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      d[i] = s[i];
   }
}

static void loc_speedcheck_memcpy_shm_to_paged(void) {
   memcpy(speedcheck_s, speedcheck_d, TC_SHM_SPEEDCHECK_SIZE);
}

static void loc_speedcheck_copy32_shm_to_paged(void) {
   sUI i;
   sU32 *s = (sU32*)speedcheck_d;
   sU32 *d = (sU32*)speedcheck_s;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      d[i] = s[i];
   }
}

static void loc_speedcheck_memcpy_paged_to_shm(void) {
   memcpy(speedcheck_d, speedcheck_s, TC_SHM_SPEEDCHECK_SIZE);
}

static void loc_speedcheck_copy32_paged_to_shm(void) {
   sUI i;
   sU32 *s = (sU32*)speedcheck_s;
   sU32 *d = (sU32*)speedcheck_d;

   for(i=0; i<(TC_SHM_SPEEDCHECK_SIZE>>2); i++)
   {
      d[i] = s[i];
   }
}

static sBool loc_tc_shm_speedcheck(dsp_cache_t _type) {
   sBool ret = S_FALSE;

   /* Allocate contiguous memory block */
   speedcheck_shm = dsp_shm_alloc(_type, TC_SHM_SPEEDCHECK_SIZE * 2u);
   
   if(0 != speedcheck_shm.size)
   {
      speedcheck_d = (sU8*) speedcheck_shm.virt_addr;

      speedcheck_s = malloc(TC_SHM_SPEEDCHECK_SIZE * 2u);

      if(NULL != speedcheck_s)
      {
         speedcheck_drain = malloc(TC_SHM_SPEEDCHECK_SIZE * 2);
         
         if(NULL != speedcheck_drain)
         {
#ifdef USE_S2
            speedcheck_s2 = malloc(TC_SHM_SPEEDCHECK_SIZE);

            if(NULL != speedcheck_s2)
#endif /* USE_S2 */
            {
               /* Memset paged */
               loc_speedcheck_exec("memset paged mem...................",
                                   &loc_speedcheck_memset_paged
                                   );

               /* Fill32 paged */
               loc_speedcheck_exec("fill32 paged mem...................",
                                   &loc_speedcheck_fill32_paged
                                   );

               /* Memset shm */
               loc_speedcheck_exec("memset contiguous mem..............",
                                   &loc_speedcheck_memset_shm
                                   );

               /* Fill32 shm */
               loc_speedcheck_exec("fill32 contiguous mem..............",
                                   &loc_speedcheck_fill32_shm
                                   );

               /* Memcpy paged to paged mem */
               loc_speedcheck_exec("memcpy paged to paged mem..........",
                                   &loc_speedcheck_memcpy_paged_to_paged
                                   );

               /* Copy32 paged to paged mem */
               loc_speedcheck_exec("copy32 paged to paged mem..........",
                                   &loc_speedcheck_copy32_paged_to_paged
                                   );

               /* Memcpy contiguous to contiguous mem */
               loc_speedcheck_exec("memcpy contiguous to contiguous mem",
                                   &loc_speedcheck_memcpy_shm_to_shm
                                   );


               /* Copy32 contiguous to contiguous mem */
               loc_speedcheck_exec("copy32 contiguous to contiguous mem",
                                   &loc_speedcheck_copy32_shm_to_shm
                                   );

               /* Memcpy contiguous to paged mem */
               loc_speedcheck_exec("memcpy contiguous to paged mem.....",
                                   &loc_speedcheck_memcpy_shm_to_paged
                                   );

               /* Copy32 contiguous to paged mem */
               loc_speedcheck_exec("copy32 contiguous to paged mem.....",
                                   &loc_speedcheck_copy32_shm_to_paged
                                   );

               /* Memcpy paged to contiguous mem */
               loc_speedcheck_exec("memcpy paged to contiguous mem.....",
                                   &loc_speedcheck_memcpy_paged_to_shm
                                   );

               /* Copy32 paged to contiguous mem */
               loc_speedcheck_exec("copy32 paged to contiguous mem.....",
                                   &loc_speedcheck_copy32_paged_to_shm
                                   );

#ifdef USE_S2
               free(speedcheck_s2);
#endif /* USE_S2 */

               /* Succeeded */
               ret = S_TRUE;
            }
#ifdef USE_S2
            else
            {
               log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes (s2).\n",
                          (TC_SHM_SPEEDCHECK_SIZE)
                          );
            }
#endif /* USE_S2 */

            free(speedcheck_drain);
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes (drain).\n",
                       (TC_SHM_SPEEDCHECK_SIZE)
                       );
         }

         free(speedcheck_s);
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes.\n",
                    (TC_SHM_SPEEDCHECK_SIZE * 2u)
                    );
      }
      

      /* Free contiguous memory block (may omit this call, dsp_close() does this) */
      dsp_shm_free(speedcheck_shm);
         
   } /* if dsp_shm_alloc() */

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_memperf_nodsp() */
#if 0
static sBool loc_tc_memperf_nodsp(void) {
   sBool ret = S_FALSE;

   osal_init(S_FALSE);

   speedcheck_s = malloc(TC_SHM_SPEEDCHECK_SIZE * 2u);

   if(NULL != speedcheck_s)
   {
      speedcheck_drain = malloc(TC_SHM_SPEEDCHECK_SIZE * 2);
      
      if(NULL != speedcheck_drain)
      {
#ifdef USE_S2
         speedcheck_s2 = malloc(TC_SHM_SPEEDCHECK_SIZE);
         
         if(NULL != speedcheck_s2)
#endif /* USE_S2 */
         {
            /* Memset paged */
            loc_speedcheck_exec("memset paged mem...................",
                                &loc_speedcheck_memset_paged
                                );
            
            /* Fill32 paged */
            loc_speedcheck_exec("fill32 paged mem...................",
                                &loc_speedcheck_fill32_paged
                                );
            
            /* Succeeded */
            ret = S_TRUE;
            
#ifdef USE_S2
            free(speedcheck_s2);
#endif /* USE_S2 */
         }
#ifdef USE_S2
         else
         {
            log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes (s2).\n",
                       (TC_SHM_SPEEDCHECK_SIZE)
                       );
         }
#endif /* USE_S2 */
         
         free(speedcheck_drain);
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes (drain).\n",
                    (TC_SHM_SPEEDCHECK_SIZE)
                    );
      }
      
      free(speedcheck_s);
   }
   else
   {
      log_printf(LOG_ERROR "loc_tc_shm_speedcheck: failed to malloc() %u bytes.\n",
                 (TC_SHM_SPEEDCHECK_SIZE * 2u)
                 );
   }

   osal_exit();

   return ret;
}
#endif // 0


/*--------------------------------------------------------------------------- loc_lazy_load_components_demo() */
sBool loc_lazy_load_components_demo(void) {
   sBool ret;

   ret = S_FALSE;

   if(0 == dsp_component_load(NULL, COMPONENT_NAME_DEMO_CALC, &compid_demo_calc))
   {
      if(0 == dsp_component_load(NULL, COMPONENT_NAME_DEMO_CHECKSUM, &compid_demo_checksum))
      {
         if(0 == dsp_component_load(NULL, COMPONENT_NAME_DEMO_CALC_FASTCALL, &compid_demo_calc_fastcall))
         {
            /* Succeeded */
            ret = S_TRUE;

            if(LOG_DEBUG_LEVEL(10))
            {
               sU32 tmpSecIdxAndId = ~0;
               int tmpRet;

               //tmpRet = dsp_component_id_find_by_name(COMPONENT_NAME_DEMO_CALC, &tmpId);
               tmpRet = dsp_component_id_find_by_name(COMPONENT_NAME_DEMO_CALC_FASTCALL, &tmpSecIdxAndId, S_FALSE);

               log_printf(LOG_DEBUG "loc_lazy_load_components: tmpId=0x%08x compid=%u tmpRet=%d\n",
                          tmpSecIdxAndId,
                          //compid_demo_calc,
                          compid_demo_calc_fastcall,
                          tmpRet
                          );
            }
         }
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_lazy_load_components_test_logbuf() */
sBool loc_lazy_load_components_test_logbuf(void) {
   sBool ret;

   ret = S_FALSE;

   if(0 == dsp_component_load(NULL, COMPONENT_NAME_TEST_LOGBUF, &compid_test_logbuf))
   {
      /* Succeeded */
      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_lazy_load_components_test_qdma() */
sBool loc_lazy_load_components_test_qdma(void) {
   sBool ret;

   ret = S_FALSE;

   if(0 == dsp_component_load(NULL, COMPONENT_NAME_TEST_QDMA, &compid_test_qdma))
   {
      /* Succeeded */
      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_l1sram_allocator() */
static sBool loc_tc_l1sram_allocator(void) {
   sBool ret = S_TRUE;
   dsp_mem_region_t mem[8];

   mem[0] = dsp_l1sram_alloc(37);
   dsp_l1sram_debug(1);

   ret = ret && (0 != mem[0].size);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: mem[0] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[0].phys_addr,
              dsp_physgpp_to_physdsp(mem[0].phys_addr),
              mem[0].virt_addr,
              mem[0].size
              );

   /* Return to initial state (one super block @0, all blocks unused) */
   dsp_l1sram_free(mem[0]);
   dsp_l1sram_debug(2);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: freed mem[0]\n");

   /* Allocate blocks 0..2 */
   mem[1] = dsp_l1sram_alloc(750);
   dsp_l1sram_debug(3);

   ret = ret && (0 != mem[1].size);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: mem[1] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[1].phys_addr,
              dsp_physgpp_to_physdsp(mem[1].phys_addr),
              mem[1].virt_addr,
              mem[1].size
              );

   /* Allocate blocks 3..20 */
   mem[2] = dsp_l1sram_alloc(4500);
   dsp_l1sram_debug(4);

   ret = ret && (0 != mem[2].size);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: mem[2] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[2].phys_addr,
              dsp_physgpp_to_physdsp(mem[2].phys_addr),
              mem[2].virt_addr,
              mem[2].size
              );

   /* Create free area at blocks 0..2 */
   dsp_l1sram_free(mem[1]);
   dsp_l1sram_debug(5);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: freed mem[1]\n");

   /* Should pick blocks 0..1 (best fit) and create new empty super block at 2 */
   mem[3] = dsp_l1sram_alloc(300);
   dsp_l1sram_debug(6);

   ret = ret && (0 != mem[3].size);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: mem[3] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[3].phys_addr,
              dsp_physgpp_to_physdsp(mem[3].phys_addr),
              mem[3].virt_addr,
              mem[3].size
              );

   /* Free blocks 3..20 and join with left hand side super block at 2..20 and 
    *  right hand side super block at 21..30
    */
   dsp_l1sram_free(mem[2]);
   dsp_l1sram_debug(7);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: freed mem[2]\n");

   /* Free blocks 0..1. Should join right hand side super block at 2..30 */
   dsp_l1sram_free(mem[3]);
   dsp_l1sram_debug(8);

   log_printf(LOG_DEBUG "loc_tc_l1sram_allocator: freed mem[3]\n");

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_shm_mspace() */
static sBool loc_tc_shm_mspace(void) {
   sBool ret = S_FALSE;
   dsp_mem_region_t shm;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_RW, TC_SHM_MSPACE_SIZE);
   
   if(0 != shm.size)
   {
      dsp_mspace_handle_t msp;

      /* Create allocator */
      msp = dsp_mspace_create(shm.virt_addr, shm.size);

      if(NULL != msp)
      {
         void *ptr;

         /* Allocate some memory using the mspace allocator */
         ptr = dsp_mspace_malloc(msp, 12345);

         if(NULL != ptr)
         {
            log_printf(LOG_INFO "dsp_mspace_malloc() returned ptr=0x%08x\n", (sU32)ptr);

            /* Free the memory */
            dsp_mspace_free(msp, ptr);

            /* Succeeded */
            ret = S_TRUE;
         }

         /* Destroy allocator */
         dsp_mspace_destroy(msp);
      }

      /* Free contiguous memory block */
      dsp_shm_free(shm);
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_ctrlc() */
static sBool loc_tc_ctrlc(void) {
   sBool ret = S_FALSE;

   /* Send DSP remote procedure call, wait 10s (must ctrlc during that time), and see if the system freezes.. */
   dsp_msg_t msg;
   int r;
   
   DSP_MSG_INIT(&msg, compid_demo_calc, DEMO_CALC_CMD_ADD, 0, 0);
   
   r = dsp_rpc_send(&msg);

   if(0 == r)
   {
      /* Wait 10s */
      usleep(1000 * 1000 * 10);

      /* Terminate fastcall sequence */
      dsp_fastcall_end();

      /* Succeeded */
      ret = S_TRUE;
   }
   else
   {
      log_printf(LOG_ERROR "dsp_rpc_send() failed (r=%d)\n", r);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_fastcall_ctrlc() */
static sBool loc_tc_fastcall_ctrlc(void) {
   sBool ret = S_FALSE;

   /* Send DSP remote procedure call, wait 10s (must ctrlc during that time), and see if the system freezes.. */
   dsp_msg_t msg;
   int r;
   
   DSP_MSG_INIT(&msg, compid_demo_calc_fastcall, DEMO_CALC_FASTCALL_INITIATE, 0, 0);
   
   r = dsp_rpc_send(&msg);

   if(0 == r)
   {
      /* Wait 10s */
      usleep(1000 * 1000 * 10);

      /* Terminate fastcall sequence */
      dsp_fastcall_end();

      /* Succeeded */
      ret = S_TRUE;
   }
   else
   {
      log_printf(LOG_ERROR "dsp_rpc_send() failed (r=%d)\n", r);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_logbuf() */
static sBool loc_tc_logbuf(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;

   DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PUTS, 0, 0);
   
   r = dsp_rpc(&msg, NULL);

   if(0 == r)
   {
      /* Print new log messages */
      dsp_logbuf_print();

      DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PRINTF, 0, 0);
      
      r = dsp_rpc(&msg, NULL);

      if(0 == r)
      {
         /* Print new log messages */
         dsp_logbuf_print();

         DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_INC_CNT_I, 1, 0);
         
         r = dsp_rpc(&msg, NULL);

         if(0 == r)
         {
            DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_VPRINTF, 0, 0);
            
            r = dsp_rpc(&msg, NULL);

            if(0 == r)
            {
               union {
                  sF32 f32;
                  sU32 u32;
               } inc;

               /* Print new log messages */
               dsp_logbuf_print();

               inc.f32 = 1.11f;

               DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_INC_CNT_F, inc.u32, 0);
               
               r = dsp_rpc(&msg, NULL);

               if(0 == r)
               {
                  DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PRINTF, 0, 0);
                  
                  r = dsp_rpc(&msg, NULL);

                  if(0 == r)
                  {
                     /* Print new log messages */
                     dsp_logbuf_print();

                     /* Succeeded */
                     ret = S_TRUE;
                  }
                  else
                  {
                     log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PRINTF #2] failed (r=%d)\n", r);
                  }
               }
               else
               {
                  log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_INC_CNT_F] failed (r=%d)\n", r);
               }
            }
            else
            {
               log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_VPRINTF] failed (r=%d)\n", r);
            }
         }
         else
         {
            log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_INC_I] failed (r=%d)\n", r);
         }
      }
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PRINTF] failed (r=%d)\n", r);
      }
   }
   else
   {
      log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PUTS] failed (r=%d)\n", r);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_logbuf_many() */
static sBool loc_tc_logbuf_many(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;
   sUI iter;

   for(iter=0; iter<100; iter++)
   {
      DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PRINTF, 0, 0);
   
      r = dsp_rpc(&msg, NULL);
   
      if(0 == r)
      {
         /* Print new log messages */
         dsp_logbuf_print();

         //dsp_debug_usr_print(3);
      
         DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_INC_CNT_I, 1, 0);
      
         r = dsp_rpc(&msg, NULL);
      
         if(0 == r)
         {
            union {
               sF32 f32;
               sU32 u32;
            } inc;
         
            inc.f32 = 1.11f;
         
            DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_INC_CNT_F, inc.u32, 0);
         
            r = dsp_rpc(&msg, NULL);

            if(0 == r)
            {
               /* Succeeded */
               ret = S_TRUE;
            }
            else
            {
               log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_INC_CNT_F] failed (r=%d)\n", r);
               ret = S_FALSE;
               break;
            }
         }
         else
         {
            log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_INC_CNT_I] failed (r=%d)\n", r);
            ret = S_FALSE;
            break;
         }
      }
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PRINTF] failed (r=%d)\n", r);
         ret = S_FALSE;
         break;
      }
   } /* loop */

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_logbuf_splitline() */
static sBool loc_tc_logbuf_splitline(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;

   DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PRINTF_SPLIT, 0, 0);
   
   r = dsp_rpc(&msg, NULL);
   
   if(0 == r)
   {
      /* Print new log messages */
      dsp_logbuf_print();

      /* Succeeded */
      ret = S_TRUE;
   }      
   else
   {
      log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PRINTF_SPLIT] failed (r=%d)\n", r);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_logbuf_no_lf() */
static sBool loc_tc_logbuf_no_lf(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;

   DSP_MSG_INIT(&msg, compid_test_logbuf, TEST_LOGBUF_CMD_PRINTF_NO_LF, 0, 0);
   
   r = dsp_rpc(&msg, NULL);
   
   if(0 == r)
   {
      /* Print new log messages */
      dsp_logbuf_print();

      /* Succeeded */
      ret = S_TRUE;
   }      
   else
   {
      log_printf(LOG_ERROR "dsp_rpc_send() [TEST_LOGBUF_CMD_PRINTF_NO_LF] failed (r=%d)\n", r);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_qdma_copy1d() */
static sBool loc_tc_qdma_copy1d(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;
   dsp_mem_region_t shm;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_NONE, sizeof(test_qdma_cmd_copy1d_args_t) + (2u * TC_QDMA_COPY1D_SIZE));
   
   if(0 != shm.size)
   {
      test_qdma_cmd_copy1d_args_t *args = (void*) shm.virt_addr;
      sU32 *src = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_copy1d_args_t));
      sU32 *dst = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_copy1d_args_t) + TC_QDMA_COPY1D_SIZE);

      /* Fill src mem and clear dst mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_COPY1D_SIZE >> 2u); i++)
         {
            src[i] = i;
            dst[i] = 0xCCddCCddu;
         }
      }
      
      args->src_phys_addr = dsp_virt_to_phys(src);
      args->dst_phys_addr = dsp_virt_to_phys(dst);
      args->num_bytes     = TC_QDMA_COPY1D_SIZE;

      log_printf(LOG_DEBUG "args->src_phys_addr=0x%08x\n", args->src_phys_addr);
      log_printf(LOG_DEBUG "args->dst_phys_addr=0x%08x\n", args->dst_phys_addr);
      log_printf(LOG_DEBUG "args->num_bytes    =0x%08x\n", args->num_bytes);

      DSP_MSG_INIT(&msg, compid_test_qdma, TEST_QDMA_CMD_COPY1D,
                   (sU32) dsp_virt_to_phys(args),
                   0
                   );
      
      r = dsp_rpc(&msg, NULL);
      
      if(0 == r)
      {
         sUI i;
         ret = S_TRUE;

         for(i=0; i < (TC_QDMA_COPY1D_SIZE >> 2u); i++)
         {
            log_printf(LOG_INFO "src[%04u] = 0x%08x  dst[%04u] = 0x%08x\n",
                       i, src[i],
                       i, dst[i]
                       );

            ret = ret && (src[i] == dst[i]);
         }

         if(!ret)
         {
            log_printf(LOG_ERROR "src/dst mismatch!\n");
         }
      }      
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_QDMA_CMD_COPY1D] failed (r=%d)\n", r);
      }

      dsp_shm_free(shm);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_qdma_copy2d() */
static sBool loc_tc_qdma_copy2d(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;
   dsp_mem_region_t shm;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_NONE,
                       sizeof(test_qdma_cmd_copy2d_args_t) + TC_QDMA_COPY2D_SRC_SIZE + TC_QDMA_COPY2D_DST_SIZE
                       );
   
   if(0 != shm.size)
   {
      test_qdma_cmd_copy2d_args_t *args = (void*) shm.virt_addr;
      sU32 *src = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_copy2d_args_t));
      sU32 *dst = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_copy2d_args_t) + TC_QDMA_COPY2D_SRC_SIZE);

      /* Fill src mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_COPY2D_SRC_SIZE >> 2u); i++)
         {
            src[i] = i;
         }
      }

      /* Clear dst mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_COPY2D_DST_SIZE >> 2u); i++)
         {
            dst[i] = 0xCCddCCddu;
         }
      }
      
      args->src_phys_addr = dsp_virt_to_phys(src);
      args->src_pitch     = TC_QDMA_COPY2D_SRC_PITCH;
      args->dst_phys_addr = dsp_virt_to_phys(dst);
      args->dst_pitch     = TC_QDMA_COPY2D_DST_PITCH;
      args->num_bytes_x   = TC_QDMA_COPY2D_SRC_PITCH;
      args->num_y         = TC_QDMA_COPY2D_SRC_NUM_Y;

      log_printf(LOG_DEBUG "args->src_phys_addr=0x%08x\n", args->src_phys_addr);
      log_printf(LOG_DEBUG "args->src_pitch    =%d\n",     args->src_pitch);
      log_printf(LOG_DEBUG "args->dst_phys_addr=0x%08x\n", args->dst_phys_addr);
      log_printf(LOG_DEBUG "args->dst_pitch    =%d\n",     args->dst_pitch);
      log_printf(LOG_DEBUG "args->num_bytes_x  =%u\n",     args->num_bytes_x);
      log_printf(LOG_DEBUG "args->num_y        =%u\n",     args->num_y);

      DSP_MSG_INIT(&msg, compid_test_qdma, TEST_QDMA_CMD_COPY2D,
                   (sU32) dsp_virt_to_phys(args),
                   0
                   );
      
      r = dsp_rpc(&msg, NULL);
      
      if(0 == r)
      {
         sUI i;

         for(i=0; i < (TC_QDMA_COPY2D_DST_SIZE >> 2u); i++)
         {
            log_printf(LOG_INFO "dst[%04u] = 0x%08x\n",
                       i, dst[i]
                       );
         }

         /* Verify */
         ret = 
            (0x00000000u == dst[0]) &&
            (0x00000001u == dst[1]) &&
            (0x00000002u == dst[4]) &&
            (0x00000003u == dst[5])  ;
      }      
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_QDMA_CMD_COPY2D] failed (r=%d)\n", r);
      }

      dsp_shm_free(shm);
   }

   return ret;
}


/* ---------------------------------------------------------------------------- loc_tc_shm_cache_rand_dsp() */
static sBool loc_tc_shm_cache_rand_dsp(dsp_cache_t _type, sBool _bDoInv, sBool _bVirt) {
   sBool ret = S_FALSE;

   /* Allocate contiguous memory block */
   dsp_mem_region_t shm = dsp_shm_alloc(_type, TC_SHM_CACHE_RAND_SIZE);
   
   if(0 != shm.size)
   {
      dsp_msg_t msg;
      dsp_msg_t reply;
      int r;
      sUI numDWords = (shm.size >> 2u);
      sU32 *s = (sU32*)shm.virt_addr;
      sUI iter = 0;
      sUI tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_RAND_WRITE,
                   shm.phys_addr, numDWords
                   );

      for(; iter < NUM_TC_SHM_CACHE_RAND_ITERATIONS; iter++)
      {
         r = dsp_rpc(&msg, &reply);

         if(0 == r)
         {
            //dsp_debug_usr_print(0);

            /* Invalidate GPP caches since DSP modified the data */
            if(_bDoInv)
            {
               if(_bVirt)
               {
                  dsp_cache_inv_virt((void*)shm.virt_addr, shm.size);
               }
               else
               {
                  dsp_cache_inv(shm.phys_addr, shm.size);
               }
            }

            /* Calc checksum */
            {
               sUI i;
               sU32 chk = 0;

               for(i=0; i<numDWords; i++)
               {
                  sU32 c = s[i];
                  //printf("s[%u] = 0x%08x\n", i, c);
                  chk = (chk ^ c);
               }

               if(chk == reply.data[0].ret)
               {
                  if(LOG_DEBUG_LEVEL(10))
                  {
                     log_printf(LOG_INFO "loc_tc_shm_cache_rand_dsp: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                                chk,
                                reply.data[0].ret
                                );
                  }

               }
               else
               {
                  log_printf(LOG_ERROR "loc_tc_shm_cache_rand_dsp: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                             iter,
                             chk,
                             reply.data[0].ret
                             );
                  ret = S_FALSE;
                  break;
               }
            }
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_shm_cache_rand_dsp: dsp_rpc() failed (iterations=%u).\n", iter);
            ret = S_FALSE;
            break;
         }

      } /* iterate */

      if(NUM_TC_SHM_CACHE_RAND_ITERATIONS == iter)
      {
         sU32 tEnd = osal_milliseconds_get();
         log_printf(LOG_INFO "loc_tc_shm_cache_rand_dsp: OK, %u iterations done in %u millisecs.\n",
                    iter,
                    (tEnd - tStart)
                    );

         /* Succeeded */
         ret = S_TRUE;
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_shm_cache_rand_dsp: FAILED after %u iterations.\n", iter);
         ret = S_FALSE;
      }

      /* Free contiguous memory block (may omit this call, dsp_close() does this) */
      dsp_shm_free(shm);
         
   } /* if dsp_shm_alloc() */

   return ret;
}


/* --------------------------------------------------------------------------- loc_rand() */
static sU32 seed = 0x44894489u;

static sU32 loc_rand_get(void) {
   seed *= 16807u;

   return seed;
}


/* ---------------------------------------------------------------------------- loc_tc_shm_cache_rand_gpp() */
static sBool loc_tc_shm_cache_rand_gpp(dsp_cache_t _type, sBool _bDoWb, sBool _bVirt) {
   sBool ret = S_FALSE;

   /* Allocate contiguous memory block */
   dsp_mem_region_t shm = dsp_shm_alloc(_type, TC_SHM_CACHE_RAND_SIZE);
   
   if(0 != shm.size)
   {
      dsp_msg_t msg;
      dsp_msg_t reply;
      int r;
      sUI numDWords = (shm.size >> 2u);
      sU32 *d = (sU32*)shm.virt_addr;
      sUI iter = 0;
      sU32 tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_RAND_READ,
                   shm.phys_addr, numDWords
                   );

      for(; iter < NUM_TC_SHM_CACHE_RAND_ITERATIONS; iter++)
      {
         /* Generate random data */
         sUI i;
         sU32 chk = 0;
         
         for(i=0; i<numDWords; i++)
         {
            sU32 c = loc_rand_get();
            d[i] = c;
            //printf("s[%u] = 0x%08x\n", i, c);
            chk = (chk ^ c);
         }

         /* Writeback GPP caches so DSP can see modified data */
         if(_bDoWb)
         {
            /* (note) currently never needed since memory is always write-through on GPP side */
            if(_bVirt)
            {
               dsp_cache_wb_virt((void*)shm.virt_addr, shm.size);
            }
            else
            {
               dsp_cache_wb(shm.phys_addr, shm.size);
            }
         }

         r = dsp_rpc(&msg, &reply);

         if(0 == r)
         {
            //dsp_debug_usr_print(0);

            if(chk == reply.data[0].ret)
            {
               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_INFO "loc_tc_shm_cache_rand_gpp: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                             chk,
                             reply.data[0].ret
                             );
               }
            }
            else
            {
               log_printf(LOG_ERROR "loc_tc_shm_cache_rand_gpp: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                          iter,
                          chk,
                          reply.data[0].ret
                          );
               break;
            }
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_shm_cache_rand_gpp: dsp_rpc() failed (iterations=%u).\n", iter);
            break;
         }

      } /* iterate */

      ret = (NUM_TC_SHM_CACHE_RAND_ITERATIONS == iter);

      if(ret)
      {
         sU32 tEnd = osal_milliseconds_get();
         log_printf(LOG_INFO "loc_tc_shm_cache_rand_gpp: OK, %u iterations done in %u millisecs.\n",
                    iter,
                    (tEnd - tStart)
                    );
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_shm_cache_rand_gpp: FAILED after %u iterations.\n", iter);
      }

      /* Free contiguous memory block (may omit this call, dsp_close() does this) */
      dsp_shm_free(shm);
         
   } /* if dsp_shm_alloc() */

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_shm_hugetlb_v2p2v() */
static sBool loc_tc_shm_hugetlb_v2p2v(void) {
   sBool ret = S_FALSE;

   /* Allocate NON-contiguous memory block */
   dsp_mem_region_t shm = dsp_shm_alloc(DSP_CACHE_HUGETLB, TC_SHM_HUGETLB_NUM_PAGES * DSP_HUGETLB_PAGESIZE);
   
   if(0 != shm.size)
   {
      log_printf(LOG_INFO "loc_tc_shm_hugetlb: virt=0x%08x phys=0x%08x size=0x%08x\n",
                 shm.virt_addr,
                 shm.phys_addr,
                 shm.size
                 );

      {
         sUI pageNr;

         for(pageNr=0; pageNr<TC_SHM_HUGETLB_NUM_PAGES; pageNr++)
         {
            void *virt;
            sU32  phys;

            virt = (void*) (shm.virt_addr + (DSP_HUGETLB_PAGESIZE * pageNr + 0x123));
            phys = dsp_virt_to_phys(virt);

            log_printf(LOG_INFO "loc_tc_shm_hugetlb: page %u/%u: dsp_virt_to_phys: virt=0x%08x phys=0x%08x\n",
                       (pageNr + 1), TC_SHM_HUGETLB_NUM_PAGES,
                       (sU32)virt,
                       (sU32)phys
                       );

            virt = dsp_phys_to_virt(phys);

            log_printf(LOG_INFO "loc_tc_shm_hugetlb: page %u/%u: dsp_phys_to_virt: virt=0x%08x phys=0x%08x\n",
                       (pageNr + 1), TC_SHM_HUGETLB_NUM_PAGES,
                       (sU32)virt,
                       (sU32)phys
                       );
         }

         /* Succeeded */
         ret = S_TRUE;
      }

      dsp_shm_free(shm);

   }  /* if dsp_shm_alloc() */

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_l2sram_allocator() */
static sBool loc_tc_l2sram_allocator(void) {
   sBool ret = S_TRUE;
   dsp_mem_region_t mem[8];

   mem[0] = dsp_l2sram_alloc(37);
   dsp_l2sram_debug(1);

   ret = ret && (0 != mem[0].size);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: mem[0] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[0].phys_addr,
              dsp_physgpp_to_physdsp(mem[0].phys_addr),
              mem[0].virt_addr,
              mem[0].size
              );

   /* Return to initial state (one super block @0, all blocks unused) */
   dsp_l2sram_free(mem[0]);
   dsp_l2sram_debug(2);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: freed mem[0]\n");

   /* Allocate blocks 0..2 */
   mem[1] = dsp_l2sram_alloc(750);
   dsp_l2sram_debug(3);

   ret = ret && (0 != mem[1].size);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: mem[1] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[1].phys_addr,
              dsp_physgpp_to_physdsp(mem[1].phys_addr),
              mem[1].virt_addr,
              mem[1].size
              );

   /* Allocate blocks 3..20 */
   mem[2] = dsp_l2sram_alloc(4500);
   dsp_l2sram_debug(4);

   ret = ret && (0 != mem[2].size);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: mem[2] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[2].phys_addr,
              dsp_physgpp_to_physdsp(mem[2].phys_addr),
              mem[2].virt_addr,
              mem[2].size
              );

   /* Create free area at blocks 0..2 */
   dsp_l2sram_free(mem[1]);
   dsp_l2sram_debug(5);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: freed mem[1]\n");

   /* Should pick blocks 0..1 (best fit) and create new empty super block at 2 */
   mem[3] = dsp_l2sram_alloc(300);
   dsp_l2sram_debug(6);

   ret = ret && (0 != mem[3].size);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: mem[3] phys_gpp=0x%08x phys_dsp=0x%08x virt=0x%08x size=0x%08x\n",
              mem[3].phys_addr,
              dsp_physgpp_to_physdsp(mem[3].phys_addr),
              mem[3].virt_addr,
              mem[3].size
              );

   /* Free blocks 3..20 and join with left hand side super block at 2..20 and 
    *  right hand side super block at 21..30
    */
   dsp_l2sram_free(mem[2]);
   dsp_l2sram_debug(7);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: freed mem[2]\n");

   /* Free blocks 0..1. Should join right hand side super block at 2..30 */
   dsp_l2sram_free(mem[3]);
   dsp_l2sram_debug(8);

   log_printf(LOG_DEBUG "loc_tc_l2sram_allocator: freed mem[3]\n");

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_qdma_link1d() */
static sBool loc_tc_qdma_link1d(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;
   dsp_mem_region_t shm;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_NONE, sizeof(test_qdma_cmd_link1d_args_t) + (2u * TC_QDMA_LINK1D_SIZE));
   
   if(0 != shm.size)
   {
      test_qdma_cmd_link1d_args_t *args = (void*) shm.virt_addr;
      sU32 *src = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_link1d_args_t));
      sU32 *dst = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_link1d_args_t) + TC_QDMA_LINK1D_SIZE);

      /* Fill src mem and clear dst mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_LINK1D_SIZE >> 2u); i++)
         {
            src[i] = (i << 24) | (i << 16) | (i << 8) | i;
            dst[i] = 0xCCddCCddu;
         }
      }

      {
         sUI i;
         sU32 s = (sU32) src;
         sU32 d = (sU32) dst;
         
         args->num_links = TEST_QDMA_MAX_LINKS;

         for(i=0; i<args->num_links; i++)
         {
            args->transfers[i].src_phys_addr = dsp_virt_to_phys((void*)s);
            args->transfers[i].dst_phys_addr = dsp_virt_to_phys((void*)d);
            args->transfers[i].num_bytes     = TC_QDMA_LINK1D_SIZE / TEST_QDMA_MAX_LINKS;

            log_printf(LOG_DEBUG "args->transfers[%u].src_phys_addr=0x%08x\n", i, args->transfers[i].src_phys_addr);
            log_printf(LOG_DEBUG "args->transfers[%u].dst_phys_addr=0x%08x\n", i, args->transfers[i].dst_phys_addr);
            log_printf(LOG_DEBUG "args->transfers[%u].num_bytes    =0x%08x\n", i, args->transfers[i].num_bytes);

            s += (TC_QDMA_LINK1D_SIZE / TEST_QDMA_MAX_LINKS);
            d += (TC_QDMA_LINK1D_SIZE / TEST_QDMA_MAX_LINKS);
         }
        
      }
      

      log_printf(LOG_DEBUG "args->num_links=0x%08x\n", args->num_links);

      DSP_MSG_INIT(&msg, compid_test_qdma, TEST_QDMA_CMD_LINK1D,
                   (sU32) dsp_virt_to_phys(args),
                   0
                   );
      
      r = dsp_rpc(&msg, NULL);
      
      if(0 == r)
      {
         sUI i;
         ret = S_TRUE;

         for(i=0; i < (TC_QDMA_LINK1D_SIZE >> 2u); i++)
         {
            log_printf(LOG_INFO "src[%04u] = 0x%08x  dst[%04u] = 0x%08x\n",
                       i, src[i],
                       i, dst[i]
                       );

            ret = ret && (src[i] == dst[i]);
         }

         if(!ret)
         {
            log_printf(LOG_ERROR "src/dst mismatch!\n");
         }
      }      
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_QDMA_CMD_LINK1D] failed (r=%d)\n", r);
      }

      dsp_shm_free(shm);
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_tc_qdma_link2d() */
static sBool loc_tc_qdma_link2d(void) {
   sBool ret = S_FALSE;
   dsp_msg_t msg;
   int r;
   dsp_mem_region_t shm;

   /* Allocate contiguous memory block */
   shm = dsp_shm_alloc(DSP_CACHE_NONE,
                       sizeof(test_qdma_cmd_link2d_args_t) + TC_QDMA_LINK2D_SRC_SIZE + TC_QDMA_LINK2D_DST_SIZE
                       );
   
   if(0 != shm.size)
   {
      test_qdma_cmd_link2d_args_t *args = (void*) shm.virt_addr;
      sU32 *src = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_link2d_args_t));
      sU32 *dst = (void*) (shm.virt_addr + sizeof(test_qdma_cmd_link2d_args_t) + TC_QDMA_LINK2D_SRC_SIZE);

      /* Fill src mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_LINK2D_SRC_SIZE >> 2u); i++)
         {
            src[i] = i;
         }
      }

      /* Clear dst mem */
      {
         sUI i;
         
         for(i=0; i< (TC_QDMA_LINK2D_DST_SIZE >> 2u); i++)
         {
            dst[i] = 0xCCddCCddu;
         }
      }
      
      {
         sUI i;
         sU32 s = (sU32) src;
         sU32 d = (sU32) dst;
         
         args->num_links = TEST_QDMA_MAX_LINKS;

         for(i=0; i<args->num_links; i++)
         {
            args->transfers[i].src_phys_addr = dsp_virt_to_phys((void*)s);
            args->transfers[i].src_pitch     = TC_QDMA_LINK2D_SRC_PITCH;
            args->transfers[i].dst_phys_addr = dsp_virt_to_phys((void*)d);
            args->transfers[i].dst_pitch     = TC_QDMA_LINK2D_DST_PITCH;
            args->transfers[i].num_bytes_x   = TC_QDMA_LINK2D_SRC_PITCH;
            args->transfers[i].num_y         = TC_QDMA_LINK2D_SRC_NUM_Y;

            log_printf(LOG_DEBUG "args->transfers[i].src_phys_addr=0x%08x\n", i, args->transfers[i].src_phys_addr);
            log_printf(LOG_DEBUG "args->transfers[i].src_pitch    =%d\n",     i, args->transfers[i].src_pitch);
            log_printf(LOG_DEBUG "args->transfers[i].dst_phys_addr=0x%08x\n", i, args->transfers[i].dst_phys_addr);
            log_printf(LOG_DEBUG "args->transfers[i].dst_pitch    =%d\n",     i, args->transfers[i].dst_pitch);
            log_printf(LOG_DEBUG "args->transfers[i].num_bytes_x  =%u\n",     i, args->transfers[i].num_bytes_x);
            log_printf(LOG_DEBUG "args->transfers[i].num_y        =%u\n",     i, args->transfers[i].num_y);

            s += (TC_QDMA_LINK2D_SRC_PITCH * TC_QDMA_LINK2D_SRC_NUM_Y);
            d += (TC_QDMA_LINK2D_DST_PITCH * TC_QDMA_LINK2D_DST_NUM_Y);
         }
        
      }
      

      log_printf(LOG_DEBUG "args->num_links=0x%08x\n", args->num_links);

      DSP_MSG_INIT(&msg, compid_test_qdma, TEST_QDMA_CMD_LINK2D,
                   (sU32) dsp_virt_to_phys(args),
                   0
                   );
      
      r = dsp_rpc(&msg, NULL);
      
      if(0 == r)
      {
         sUI i;

         for(i=0; i < (TC_QDMA_LINK2D_DST_SIZE >> 2u); i++)
         {
            log_printf(LOG_INFO "dst[%04u] = 0x%08x\n",
                       i, dst[i]
                       );
         }

         /* Verify */
         ret = 
            (0x00000000u == dst[  0]) &&
            (0x00000001u == dst[  1]) &&
            (0x00000002u == dst[  4]) &&
            (0x00000003u == dst[  5]) &&
            (0x00000004u == dst[ 16]) &&
            (0x00000005u == dst[ 17]) &&
            (0x00000006u == dst[ 20]) &&
            (0x00000007u == dst[ 21]) &&
            (0x00000008u == dst[ 32]) &&
            (0x00000009u == dst[ 33]) &&
            (0x0000000Au == dst[ 36]) &&
            (0x0000000Bu == dst[ 37]) &&
            (0x0000000Cu == dst[ 48]) &&
            (0x0000000Du == dst[ 49]) &&
            (0x0000000Eu == dst[ 52]) &&
            (0x0000000Fu == dst[ 53]) &&
            (0x00000010u == dst[ 64]) &&
            (0x00000011u == dst[ 65]) &&
            (0x00000012u == dst[ 68]) &&
            (0x00000013u == dst[ 69]) &&
            (0x00000014u == dst[ 80]) &&
            (0x00000015u == dst[ 81]) &&
            (0x00000016u == dst[ 84]) &&
            (0x00000017u == dst[ 85]) &&
            (0x00000018u == dst[ 96]) &&
            (0x00000019u == dst[ 97]) &&
            (0x0000001Au == dst[100]) &&
            (0x0000001Bu == dst[101]) &&
            (0x0000001Cu == dst[112]) &&
            (0x0000001Du == dst[113]) &&
            (0x0000001Eu == dst[116]) &&
            (0x0000001Fu == dst[117])  ;
      }      
      else
      {
         log_printf(LOG_ERROR "dsp_rpc_send() [TEST_QDMA_CMD_LINK2D] failed (r=%d)\n", r);
      }

      dsp_shm_free(shm);
   }

   return ret;
}


/* ---------------------------------------------------------------------------- loc_tc_l2sram_rand_chksum_dsp() */
static sBool loc_tc_l2sram_rand_chksum_dsp(void) {
   sBool ret = S_FALSE;

   /* Allocate L2SRAM */
   dsp_mem_region_t l2sram = dsp_l2sram_alloc(TC_L2SRAM_RAND_CHKSUM_SIZE);
   
   if(0 != l2sram.size)
   {
      dsp_msg_t msg;
      dsp_msg_t reply;
      int r;
      sUI numDWords = (l2sram.size >> 2u);
      sU32 *s = (sU32*)l2sram.virt_addr;
      sUI iter = 0;
      sUI tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_RAND_WRITE_NOWB,
                   dsp_physgpp_to_physdsp(l2sram.phys_addr), numDWords
                   );

      for(; iter < NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS; iter++)
      {
         r = dsp_rpc(&msg, &reply);

         if(0 == r)
         {
            /* Calc checksum */
            {
               sUI i;
               sU32 chk = 0;

               for(i=0; i<numDWords; i++)
               {
                  sU32 c = s[i];
                  //printf("s[%u] = 0x%08x\n", i, c);
                  chk = (chk ^ c);
               }

               if(chk == reply.data[0].ret)
               {
                  if(LOG_DEBUG_LEVEL(10))
                  {
                     log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_dsp: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                                chk,
                                reply.data[0].ret
                                );
                  }
               }
               else
               {
                  log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                             iter,
                             chk,
                             reply.data[0].ret
                             );
                  break;
               }
            }
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp: dsp_rpc() failed (iterations=%u).\n", iter);
            break;
         }

      } /* iterate */

      ret = (NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS == iter);

      if(ret)
      {
         sU32 tEnd = osal_milliseconds_get();
         log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_dsp: OK, %u iterations done in %u millisecs.\n",
                    iter,
                    (tEnd - tStart)
                    );
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp: FAILED after %u iterations.\n", iter);
      }

      /* Free L2SRAM */
      dsp_l2sram_free(l2sram);
         
   } /* if dsp_l2sram_alloc() */

   return ret;
}


/* ---------------------------------------------------------------------------- loc_tc_l2sram_rand_chksum_gpp() */
static sBool loc_tc_l2sram_rand_chksum_gpp(void) {
   sBool ret = S_FALSE;

   /* Allocate L2SRAM */
   dsp_mem_region_t l2sram = dsp_l2sram_alloc(TC_L2SRAM_RAND_CHKSUM_SIZE);
   
   if(0 != l2sram.size)
   {
      dsp_msg_t msg;
      dsp_msg_t reply;
      int r;
      sUI numDWords = (l2sram.size >> 2u);
      sU32 *d = (sU32*)l2sram.virt_addr;
      sUI iter = 0;
      sU32 tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_RAND_READ_NOINV,
                   dsp_physgpp_to_physdsp(l2sram.phys_addr), numDWords
                   );

      for(; iter < NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS; iter++)
      {
         /* Generate random data */
         sUI i;
         sU32 chk = 0;
         
         for(i=0; i<numDWords; i++)
         {
            sU32 c = loc_rand_get();
            d[i] = c;
            //printf("s[%u] = 0x%08x\n", i, c);
            chk = (chk ^ c);
         }

         r = dsp_rpc(&msg, &reply);

         if(0 == r)
         {
            if(chk == reply.data[0].ret)
            {
               if(LOG_DEBUG_LEVEL(10))
               {
                  log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_gpp: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                             chk,
                             reply.data[0].ret
                             );
               }
            }
            else
            {
               log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                          iter,
                          chk,
                          reply.data[0].ret
                          );
               break;
            }
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp: dsp_rpc() failed (iterations=%u).\n", iter);
            break;
         }

      } /* iterate */

      ret = (NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS == iter);

      if(ret)
      {
         sU32 tEnd = osal_milliseconds_get();
         log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_gpp: OK, %u iterations done in %u millisecs.\n",
                    iter,
                    (tEnd - tStart)
                    );
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp: FAILED after %u iterations.\n", iter);
      }

      /* Free L2SRAM */
      dsp_l2sram_free(l2sram);
         
   } /* if dsp_l2sram_alloc() */

   return ret;
}


/* ---------------------------------------------------------------------------- loc_tc_l2sram_rand_chksum_dsp_fc() */
static sBool loc_tc_l2sram_rand_chksum_dsp_fc(void) {
   sBool ret = S_FALSE;

   /* Allocate L2SRAM */
   dsp_mem_region_t l2sram = dsp_l2sram_alloc(TC_L2SRAM_RAND_CHKSUM_SIZE);
   
   if(0 != l2sram.size)
   {
      dsp_msg_t msg;
      int r;
      sUI numDWords = (l2sram.size >> 2u);
      sU32 *s = (sU32*)l2sram.virt_addr;
      sUI iter = 0;
      sUI tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_FC_INITIATE,
                   0, 0
                   );
      
      r = dsp_rpc_send(&msg);

      if(0 == r)
      {
         for(; iter < NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS; iter++)
         {
            sU32 r1FC;

            r = dsp_fastcall_rpc(DEMO_CHECKSUM_FC_CMD_RAND_WRITE_NOWB,
                                 dsp_physgpp_to_physdsp(l2sram.phys_addr), numDWords,
                                 &r1FC, NULL
                                 );

            if(0 == r)
            {
               /* Calc checksum */
               {
                  sUI i;
                  sU32 chk = 0;
                  
                  for(i=0; i<numDWords; i++)
                  {
                     sU32 c = s[i];
                     //printf("s[%u] = 0x%08x\n", i, c);
                     chk = (chk ^ c);
                  }
                  
                  if(chk == r1FC)
                  {
                     if(LOG_DEBUG_LEVEL(10))
                     {
                        log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_dsp_fc: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                                   chk,
                                   r1FC
                                   );
                     }
                  }
                  else
                  {
                     log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp_fc: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                                iter,
                                chk,
                                r1FC
                                );
                     break;
                  }
               }
            }
            else
            {
               log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp_fc: dsp_fastcall_rpc() failed (iterations=%u).\n", iter);
               break;
            }
            
         } /* iterate */
         
         ret = (NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS == iter);

         if(ret)
         {
            sU32 tEnd = osal_milliseconds_get();
            log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_dsp_fc: OK, %u iterations done in %u millisecs.\n",
                       iter,
                       (tEnd - tStart)
                       );
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp_fc: FAILED after %u iterations.\n", iter);
         }

         dsp_fastcall_end();
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_dsp_fc: FAILED to initiate fastcall sequence.\n");
      }
         
      /* Free L2SRAM */
      dsp_l2sram_free(l2sram);
         
   } /* if dsp_l2sram_alloc() */

   return ret;
}


/* ---------------------------------------------------------------------------- loc_tc_l2sram_rand_chksum_gpp_fc() */
static sBool loc_tc_l2sram_rand_chksum_gpp_fc(void) {
   sBool ret = S_FALSE;

   /* Allocate L2SRAM */
   dsp_mem_region_t l2sram = dsp_l2sram_alloc(TC_L2SRAM_RAND_CHKSUM_SIZE);
   
   if(0 != l2sram.size)
   {
      dsp_msg_t msg;
      int r;
      sUI numDWords = (l2sram.size >> 2u);
      sU32 *d = (sU32*)l2sram.virt_addr;
      sUI iter = 0;
      sU32 tStart = osal_milliseconds_get();
      
      DSP_MSG_INIT(&msg, compid_demo_checksum, DEMO_CHECKSUM_CMD_FC_INITIATE,
                   0, 0
                   );
      
      r = dsp_rpc_send(&msg);

      if(0 == r)
      {
         for(; iter < NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS; iter++)
         {
            /* Generate random data */
            sUI i;
            sU32 chk = 0;
            sU32 r1FC;
            
            for(i=0; i<numDWords; i++)
            {
               sU32 c = loc_rand_get();
               d[i] = c;
               //printf("s[%u] = 0x%08x\n", i, c);
               chk = (chk ^ c);
            }
            
            r = dsp_fastcall_rpc(DEMO_CHECKSUM_FC_CMD_RAND_READ_NOINV,
                                 dsp_physgpp_to_physdsp(l2sram.phys_addr), numDWords,
                                 &r1FC, NULL
                                 );
            
            if(0 == r)
            {
               if(chk == r1FC)
               {
                  if(LOG_DEBUG_LEVEL(10))
                  {
                     log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_gpp_fc: OK, checksums match (GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                                chk,
                                r1FC
                                );
                  }
               }
               else
               {
                  log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp_fc: checksum MISMATCH (iter=%u, GPP.chk=0x%08x DSP.chk=0x%08x)\n",
                             iter,
                             chk,
                             r1FC
                             );
                  break;
               }
            }
            else
            {
               log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp_fc: dsp_fastcall_rpc() failed (iterations=%u).\n", iter);
               break;
            }
            
         } /* iterate */
         
         ret = (NUM_TC_L2SRAM_RAND_CHKSUM_ITERATIONS == iter);

         if(ret)
         {
            sU32 tEnd = osal_milliseconds_get();
            log_printf(LOG_INFO "loc_tc_l2sram_rand_chksum_gpp_fc: OK, %u iterations done in %u millisecs.\n",
                       iter,
                       (tEnd - tStart)
                       );
         }
         else
         {
            log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp_fc: FAILED after %u iterations.\n", iter);
         }

         dsp_fastcall_end();
      }
      else
      {
         log_printf(LOG_ERROR "loc_tc_l2sram_rand_chksum_gpp_fc: FAILED to initiate fastcall sequence.\n");
      }
      
      /* Free L2SRAM */
      dsp_l2sram_free(l2sram);
         
   } /* if dsp_l2sram_alloc() */

   return ret;
}


/*--------------------------------------------------------------------------- loc_runtest() */
static sBool loc_runtest(sUI _tcIdx) {
   sBool ret = S_FALSE;
   int r;

   if(0 != test_max_tc_idx)
   {
      printf(LOG_INFO "start test_cycle_idx=%u, testcase=%u (\"%s\")\n", test_cycle_idx, _tcIdx, tc_names[_tcIdx]);
   }
   else
   {
      printf(LOG_INFO "selected testcase %u (\"%s\")\n", _tcIdx, tc_names[_tcIdx]);
   }

   /* Open client connection */
   r = dsp_open();

   if(0 == r)
   {
      /* Query L1SRAM scratchbuffer attributes */
      l1sram_scratch = dsp_l1sram_scratchbuffer_get();

      {
         sBool bCompLoadOk = S_TRUE;

         /* Run testcase */
         switch(_tcIdx)
         {
            default:
               break;
                  
            case TC_TESTMSG:
            {
               loc_tc_testmsg();

               ret = S_TRUE;
            }
            break;
               
            case TC_RPC_ADD:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_rpc_add_test();
               }
            }
            break;
            
               
            case TC_RPC_ADD_BENCHMARK:
            {
               /* Benchmark DSP remote procedure call speed */
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_rpc_add_benchmark();
               }
                  
            }
            break;
               
            case TC_IOCTL_TEST:
            {
#ifdef DSP_USE_PRIV
               dsp_ioctl_test();
#else
               log_printf(LOG_WARN "TC_IOCTL_TEST: skipping test, c64_tc was compiled w/o USE_DSP_PRIV\n");
#endif /* USE_DSP_PRIV */

               ret = S_TRUE;
            }
            break;
            
            case TC_SHM_ACCESS:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_access_test();
               }
            }
            break;
               
            case TC_RPC_ADD_FASTCALL_BENCHMARK:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_rpc_add_fastcall_benchmark();
               }
            }
            break;

            case TC_L1SRAM_SPEED:
            {
               ret = loc_tc_l1sram_speed();
            }
            break;

            case TC_PWR_OFF:
            {
#ifdef DSP_USE_PRIV
               if(0 == dsp_suspend())
               {
                  log_printf(LOG_INFO "ok, DSP is powered down now.\n");
               }
#else
               log_printf(LOG_WARN "TC_PWR_OFF: skipping test, c64_tc was compiled w/o USE_DSP_PRIV\n");
#endif /* USE_DSP_PRIV */

               ret = S_TRUE;
            }
            break;
               
            case TC_RESUME:
            {
#ifdef DSP_USE_PRIV
               log_printf(LOG_INFO "resuming DSP program execution..");
                  
               if(0 == dsp_resume())
               {
                  log_printf("OK!\n");
               }
               else
               {
                  log_printf("FAILED!!\n");
               }
#else
               log_printf(LOG_WARN "TC_RESUME: skipping test, c64_tc was compiled w/o USE_DSP_PRIV\n");
#endif /* USE_DSP_PRIV */

               ret = S_TRUE;
            }
            break;

            case TC_IDLE:
            {
               log_printf(LOG_INFO "sleeping for 30 seconds..\n");

               usleep(1000000 * 30);

               ret = S_TRUE;
            }
            break;

            case TC_SHM_CACHE_NONE:
            {
               ret = loc_tc_shm_speedcheck(DSP_CACHE_NONE);
            }
            break;

            case TC_SHM_CACHE_RW:
            {
               ret = loc_tc_shm_speedcheck(DSP_CACHE_RW);
            }
            break;

            case TC_SHM_CACHE_R:
            {
               ret = loc_tc_shm_speedcheck(DSP_CACHE_R);
            }
            break;

            case TC_SHM_CACHE_W:
            {
               ret = loc_tc_shm_speedcheck(DSP_CACHE_W);
            }
            break;

            case TC_L1SRAM_ALLOCATOR:
            {
               ret = loc_tc_l1sram_allocator();
            }
            break;

            case TC_SHM_MSPACE:
            {
               ret = loc_tc_shm_mspace();
            }
            break;

            case TC_CTRLC:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_ctrlc();
               }
            }
            break;

            case TC_FASTCALL_CTRLC:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_fastcall_ctrlc();
               }
            }
            break;

            case TC_LOGBUF:
            {
               bCompLoadOk = loc_lazy_load_components_test_logbuf();

               if(bCompLoadOk)
               {
                  ret = loc_tc_logbuf();
               }
            }
            break;

            case TC_LOGBUF_MANY:
            {
               bCompLoadOk = loc_lazy_load_components_test_logbuf();

               if(bCompLoadOk)
               {
                  ret = loc_tc_logbuf_many();
               }
            }
            break;

            case TC_LOGBUF_SPLITLINE:
            {
               bCompLoadOk = loc_lazy_load_components_test_logbuf();

               if(bCompLoadOk)
               {
                  ret = loc_tc_logbuf_splitline();
               }
            }
            break;

            case TC_LOGBUF_NO_LF:
            {
               bCompLoadOk = loc_lazy_load_components_test_logbuf();

               if(bCompLoadOk)
               {
                  ret = loc_tc_logbuf_no_lf();
               }
            }
            break;

            case TC_QDMA_COPY1D:
            {
               bCompLoadOk = loc_lazy_load_components_test_qdma();

               if(bCompLoadOk)
               {
                  ret = loc_tc_qdma_copy1d();
               }
            }
            break;

            case TC_QDMA_COPY2D:
            {
               bCompLoadOk = loc_lazy_load_components_test_qdma();

               if(bCompLoadOk)
               {
                  ret = loc_tc_qdma_copy2d();
               }
            }
            break;

            case TC_SHM_CACHE_NONE_RAND_DSP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_NONE, S_FALSE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_RW_RAND_DSP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_RW, S_TRUE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_R_RAND_DSP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_R, S_TRUE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_W_RAND_DSP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_W, S_TRUE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_NONE_RAND_GPP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_NONE, S_FALSE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_RW_RAND_GPP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_RW, S_TRUE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_R_RAND_GPP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_R, S_FALSE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_W_RAND_GPP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_W, S_TRUE, S_FALSE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_NONE_RAND_DSP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_NONE, S_FALSE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_RW_RAND_DSP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_RW, S_TRUE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_R_RAND_DSP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_R, S_TRUE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_W_RAND_DSP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_W, S_TRUE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_NONE_RAND_GPP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_NONE, S_FALSE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_RW_RAND_GPP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_RW, S_TRUE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_R_RAND_GPP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_gpp(DSP_CACHE_R, S_FALSE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_W_RAND_GPP_VIRT:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_cache_rand_dsp(DSP_CACHE_W, S_TRUE, S_TRUE/*bVirt*/);
               }
            }
            break;

            case TC_SHM_CACHE_HUGETLB_V2P2V:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_hugetlb_v2p2v();
               }
            }
            break;

            case TC_SHM_CACHE_HUGETLB:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_shm_speedcheck(DSP_CACHE_HUGETLB);
               }
            }
            break;

            case TC_L2SRAM_ALLOCATOR:
            {
               ret = loc_tc_l2sram_allocator();
            }
            break;

            case TC_QDMA_LINK1D:
            {
               bCompLoadOk = loc_lazy_load_components_test_qdma();

               if(bCompLoadOk)
               {
                  ret = loc_tc_qdma_link1d();
               }
            }
            break;

            case TC_QDMA_LINK2D:
            {
               bCompLoadOk = loc_lazy_load_components_test_qdma();

               if(bCompLoadOk)
               {
                  ret = loc_tc_qdma_link2d();
               }
            }
            break;

            case TC_L2SRAM_RAND_CHKSUM_DSP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_l2sram_rand_chksum_dsp();
               }
            }
            break;

            case TC_L2SRAM_RAND_CHKSUM_GPP:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_l2sram_rand_chksum_gpp();
               }
            }
            break;

            case TC_L2SRAM_RAND_CHKSUM_DSP_FC:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_l2sram_rand_chksum_dsp_fc();
               }
            }
            break;

            case TC_L2SRAM_RAND_CHKSUM_GPP_FC:
            {
               bCompLoadOk = loc_lazy_load_components_demo();

               if(bCompLoadOk)
               {
                  ret = loc_tc_l2sram_rand_chksum_gpp_fc();
               }
            }
            break;
               
         } // switch

         if(!bCompLoadOk)
         {
            /* Failed: loc_lazy_load_components() */
            log_printf(LOG_ERROR "failed to load/query required DSP components.\n");
         }
      }
         
      /* Unload components / free shared memory / disconnect client */
      dsp_close();

      /* if dsp_open() */
   }

   //log_puts("\n");

   if(ret)
   {
      log_printf(LOG_INFO "test_cycle_idx=%d, test_case_idx=%d (\"%s\") OK.\n\n",
                 test_cycle_idx,
                 _tcIdx,
                 tc_names[_tcIdx]
                 );
   }
   else
   {
      log_printf(LOG_ERROR "test_cycle_idx=%d, test_case_idx=%d (\"%s\") **FAILED**.\n\n",
                 test_cycle_idx,
                 _tcIdx,
                 tc_names[_tcIdx]
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret = 0;

   test_cycle_idx = 0;
   test_min_tc_idx = TC_NONE;
   test_max_tc_idx = TC_NONE;
   test_num_cycles = 0;
   total_num_tests = 0;

   if(1 == argc)
   {
      usage();

      return 0;
   }

   sscanf(argv[1], "%u", &test_min_tc_idx);
   
   if((0 != test_min_tc_idx) && (test_min_tc_idx < TC_NUM))
   {
      if(argc >= 3)
      {
         sscanf(argv[2], "%u", &test_max_tc_idx);
         
         if((0 != test_max_tc_idx) && (test_max_tc_idx < TC_NUM))
         {
            /* Swap start/end tcIdx if necessary */
            if(test_max_tc_idx < test_min_tc_idx)
            {
               sUI t = test_max_tc_idx;
               test_max_tc_idx = test_min_tc_idx;
               test_min_tc_idx = t;
            }

            /* Determine number of test cycles (0=infinite) */
            if(argc >= 4)
            {
               sscanf(argv[3], "%u", &test_num_cycles);
            }
         }
         else
         {
            printf(LOG_ERROR "invalid testcase (max) idx %u\n", test_max_tc_idx);

            ret = 10;
         }
      }
   }
   else
   {
      printf(LOG_ERROR "invalid testcase idx %u\n", test_min_tc_idx);
      
      ret = 10;
   }

   if(0 == ret)
   {
      if(0 != test_max_tc_idx)
      {
         printf(LOG_INFO "starting stresstest, minTC=%u, maxTC=%u, numCycles=%u.\n",
                test_min_tc_idx,
                test_max_tc_idx,
                test_num_cycles
                );

         while(0 == ret)
         {
            sUI tcIdx;

            for(tcIdx=test_min_tc_idx; tcIdx<=test_max_tc_idx; tcIdx++)
            {
               total_num_tests++;

               if(!loc_runtest(tcIdx))
               {
                  printf(LOG_ERROR "stresstest aborted due to error. test_cycle_idx=%u, tcIdx=%u (\"%s\").\n",
                         test_cycle_idx,
                         tcIdx,
                         tc_names[tcIdx]
                         );

                  /* Failed */
                  ret = 20;
                  break;
               }
            }

            if(0 == ret)
            {
               test_cycle_idx++;

               if(0 != test_num_cycles)
               {
                  if(test_cycle_idx == test_num_cycles)
                  {
                     break;
                  }
               }
            }
         }

         if(0 == ret)
         {
            printf(LOG_INFO "stresstest finished w/o errors :) numTestCycles=%u totalNumTests=%u\n\n",
                   test_cycle_idx,
                   total_num_tests
                   );
         }
         else
         {
            printf(LOG_INFO "stresstest FAILED after numTestCycles=%u totalNumTests=%u\n\n",
                   test_cycle_idx,
                   total_num_tests
                   );
         }
      }
      else
      {
         /* Run single test */
         if(!loc_runtest(test_min_tc_idx))
         {
            printf(LOG_ERROR "testcase %u failed.\n", test_min_tc_idx);

            /* Failed */
            ret = 20;
         }
      }
   }

   return ret;
}
