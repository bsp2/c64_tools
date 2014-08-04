/* ----
 * ---- file   : main.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2014 by Bastian Spiegel.
 * ----          Distributed under terms of the MIT LICENSE (MIT).
 * ----
 * ---- Permission is hereby granted, free of charge, to any person obtaining a copy
 * ---- of this software and associated documentation files (the "Software"), to deal
 * ---- in the Software without restriction, including without limitation the rights
 * ---- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * ---- copies of the Software, and to permit persons to whom the Software is
 * ---- furnished to do so, subject to the following conditions:
 * ----
 * ---- The above copyright notice and this permission notice shall be included in
 * ---- all copies or substantial portions of the Software.
 * ----
 * ---- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * ---- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * ---- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * ---- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * ---- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * ---- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * ---- THE SOFTWARE.
 * ----
 * ---- info   : This is part of the "c64_tools" package examples.
 * ----
 * ---- changed: 15Jan2014
 * ----          
 * ----
 * ----
 */

#define IVA2_XMC_L1DMPAA_BASE  (0x0184AE00u)

#include <std.h>
#include <tsk.h>
#include <bcache.h>
//#include <c64.h>

#include "../../include/types.h"
#include "../../include/arch/dm3730/dm3730.h"
#include "../../include/dsp_common.h"
#include "../../include/dsp_config.h"



#pragma DATA_ALIGN(mlb_sram,  32);
#pragma DATA_SECTION(mlb_sram,  ".sec_mlb_sram");
volatile unsigned char mlb_sram[IVA2_L1DSRAM_SIZE]; /* all of it */


#define IPC_REG(off)  (*(volatile sU32*)(&mlb_sram[(off)]))



/* --------------------------------------------------------------------------- module vars */
static TSK_Attrs mlb_task_attrs;


/* --------------------------------------------------------------------------- loc_its_alive() */
void loc_its_alive(void) {
   /* Write some identity values so that GPP side can verify that DSP is actually running
    *
    *  (note) the first two cachelines (64 bytes) are reserved for GPP<>DSP communication
    *
    */
   sU32 i;
   volatile sU32 *resetBuf = (sU32*) (dsp_config.reset_vector.phys_addr + DSP_RESET_VECTOR_CODESIZE);

   for(i=0; i<16; i++)
   {
      resetBuf[i] = 0;
   }

   for(i=16; i<((DSP_RESET_VECTOR_MINSIZE - DSP_RESET_VECTOR_CODESIZE)/4); i++)
   {
      resetBuf[i] = i;
   }

   /* Signal GPP side that DSP startup is complete */
   IPC_REG(DSP_IPC_BOOTMODE_OFF) = DSP_BOOTFLAG_RUNNING;

   BCACHE_wbAll();
}


/* --------------------------------------------------------------------------- task__entry() */
static void mlb_task__entry(void) {
   sUI irqCount = 0;

#if 1
      {
         sUI i = 0;

         while(i < 1) // (note) crash when writing to all 32 regs (or even more than 2)
         {
            /* Unlock access to L1D memory */
            DSP_REG(IVA2_XMC_L1DMPAA_BASE + (4u * i)) =
                  (1u <<  1) |  // User write access is permitted
                  (1u <<  2) |  // User read access is permitted
                  (1u <<  4) |  // Supervisor write access is permitted
                  (1u <<  5) |  // Supervisor read access is permitted
                  (1u <<  8) |  // Local (DSP) access is permitted
#if 1
                  (1u <<  9) |  // External access is permitted
                  (1u << 10) |  // ID0 has access permission
                  (1u << 11) |  // ID1 has access permission
                  (1u << 12) |  // ID2 has access permission
                  (1u << 13) |  // ID3 has access permission
                  (1u << 14) |  // ID4 has access permission
                  (1u << 15) |  // ID5 has access permission
#endif
                  0          ;

            i++;
         }
      }
#endif

#if 1
   /* Enable power-down during idle */
   DSP_REG(IVA2_SYS_PDCCMD) |= (1u << 16);  /* set GEMPD bit */
#endif

   loc_its_alive();

   for(;;)
   {
      /* ... */
#if 0
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");

            asm(" IDLE\n");

            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
            asm(" NOP\n");
#endif
   }
}


/* --------------------------------------------------------------------------- main() */
void main(void) {

   mlb_task_attrs = TSK_ATTRS;
   mlb_task_attrs.name     = "mlb";
   mlb_task_attrs.priority = TSK_MINPRI;

   (void)TSK_create((Fxn)mlb_task__entry, &mlb_task_attrs);
}
