/* ----
 * ---- file   : dsp_access.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Arch-dependent DSP utility functions. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 07Sep2013, 08Sep2013, 18Sep2013, 02Oct2013
 * ----
 * ----
 */

#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../../../include/types.h"
#include "../../../include/log.h"

#include "../../../include/dsp_err.h"
#include "../../../include/dsp_common.h"
#include "../../../include/dsp_config.h"
#include "../../../include/msg.h"
#include "../../../include/dsp.h"

#include "../../../include/arch/dm3730/dm3730.h"


/*--------------------------------------------------------------------------- dsp_reset() */
#if 0
dsp_error_t S_CALL dsp_reset(void) {
   dsp_error_t ret = DSP_ERR_OK;

   // (note) reset/release code is borrowed from Texas Instruments (found in "omap3530_hal_pwr.c")
   
   // Put DSP in reset mode

#if 1
   if(DSP_ERR_OK == ret)
   {
      // Disable GPTIMER5 functional clock (EN_GPT5)
      ret = dsp_reg_bit_clear(IVA2_PER_CM_FCLKEN, 6);
   }
   
   if(DSP_ERR_OK == ret)
   {
      // Disable GPTIMER5 interface clock (EN_GPT5)
      ret = dsp_reg_bit_clear(IVA2_PER_CM_ICLKEN, 6);
   }
#endif
   
   if(DSP_ERR_OK == ret)
   {
      // Disable automatic transition (clear bit 0, bits 1..31 are reserved)
      ret = dsp_reg_write_32(IVA2_CM_CLKSTCTRL, 0);
   }
   
   if(DSP_ERR_OK == ret)
   {
      // Disable IVA2_CLK (clear bit 0, bits 1..31 are reserved)
      ret = dsp_reg_write_32(IVA2_CM_FCLKEN, 0);
   }
   
   if(DSP_ERR_OK == ret)
   {
      // Reset video sequencer (RST3 bit) and DSP (RST1 bit) (don't reset MMU)
      ret = dsp_reg_write_32(IVA2_PRM_RSTCTRL, 5);
      //ret = dsp_reg_write_32(IVA2_PRM_RSTCTRL, 1+2+4);
   }

   if(DSP_ERR_OK == ret)
   {
      // Enable automatic transition (sets reserved bit 1 ??)
      ret = dsp_reg_write_32(IVA2_CM_CLKSTCTRL, 3);
   }
   

#if 0   
   if(DSP_ERR_OK == ret)
   {
      //sU32 val;
     
      
      //ret = dsp_reg_read_32(IVA2_MMU2_SYSCONFIG, &val);
      //ret = dsp_reg_read_32(0x48004a10, &val); // read CM_ICLKEN1_CORE --> 433FFE52
      //ret = dsp_reg_read_32(0x48004000, &val); // read --> 1
      //ret = dsp_reg_read_32(0x48004020, &val); // read CM_IDLEST_IVA2 --> 1 (iva2 standby mode)
      //ret = dsp_reg_read_32(0x48004004, &val); // read CM_CLKEN_PLL_IVA2 --> 17
      //ret = dsp_reg_read_32(0x4800404C, &val); // read CM_CLKSTST_IVA2 --> 
      //ret = dsp_reg_read_32(0x4800404C, &val); // read CM_CLKSTST_IVA2 --> 1
      //ret = dsp_reg_read_32(0x48004D00, &val); // read CM_CLKEN_PLL --> F0371037
      //ret = dsp_reg_read_32(0x48004034, &val); // read CM_AUTOIDLE_PLL --> 1 (auto LP when IVA2 clock is not req)
      
      //printf("xxx val=0x%08x\n", val);
      
      // Disable MMU
      //  (note) for some reason this once worked (07Sep2013 4 AM release) but
      //          now I always get a bus error when trying to access the mmap'd MMU regs
      //          ==> moving this to a kernel module (c64.ko) and using ioremap_nocache() fixed it!
      ////////ret = dsp_reg_bit_clear(IVA2_MMU2_CNTL, 1);
   }
#endif

   return ret;
}
#endif


/*--------------------------------------------------------------------------- dsp_start() */
#if 0
static int xxx = 0;
static void pxxx(void) {
/*    log_printf("xxx=%d\n", xxx++); */
/*    usleep(10000); */
}

dsp_error_t dsp_start(sU32 _bootAddr) {
   dsp_error_t ret = DSP_ERR_OK;

   /* Activate IVA2, generate boot code, and release DSP from reset */

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Set reset vector
      ret = dsp_reg_write_32(IVA2_SCM_GENERAL_BOOTADDR, dsp_config.reset_vector.phys_addr);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Set boot mode to "Direct boot"
      ret = dsp_reg_write_32(IVA2_SCM_GENERAL_BOOTMOD, 0);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Generate C64+ startup code (this block is partially borrowed from TI)
      sUI virtAddr;

      // Map reset vector to GPP
      ret = dsp_mmap(dsp_config.reset_vector.phys_addr, DSP_RESET_VECTOR_CODESIZE, &virtAddr);

      pxxx();

      if(DSP_ERR_OK == ret)
      {
         sU32 *code          = (sU32*) virtAddr;
         sU32  entryPtAddrHi = 0x00000000u;
         sU32  entryPtAddrLo = 0x00000000u;
         sU32  startOpCodeLo = 0x0000002Au;
         sU32  startOpCodeHi = 0x0000006Au;
         sU32  noOp          = 0x00400000u;
         sU32  branch        = 0x00000362u;

         if(log_debug_level > 0)
         {
            log_printf(LOG_DEBUG "dsp_start: bootAddr=0x%08x\n", _bootAddr);
         }
            
         // Write branch to boot.64P in .sysinit segment (_c_int00)
         entryPtAddrHi  = (_bootAddr >> 16);
         entryPtAddrLo  = (_bootAddr & 0xFFFFu);
         startOpCodeHi |= (entryPtAddrHi << 7);
         startOpCodeLo |= (entryPtAddrLo << 7);
         *code++ = startOpCodeLo;
         *code++ = startOpCodeHi;
         *code++ = branch;

         // Write 5 no-ops for pipeline flush
         *code++ = noOp;
         *code++ = noOp;
         *code++ = noOp;
         *code++ = noOp;
         *code++ = noOp;

         pxxx();

         // Unmap reset vector
         ret = dsp_munmap(virtAddr, DSP_RESET_VECTOR_CODESIZE);

         pxxx();
      }
   }

#if 1

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Enable GPTIMER5 functional clock
      ret = dsp_reg_bit_set(IVA2_PER_CM_FCLKEN, 6);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Enable GPTIMER5 interface clock (EN_GPT5)
      ret = dsp_reg_bit_set(IVA2_PER_CM_ICLKEN, 6);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Set GPTIMER5 clock source to SYS_CLK
      ret = dsp_reg_bit_set(IVA2_PER_CM_CLKSEL, 3);
   }
#endif 

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Disable automatic transition (clear bit 0, bits 1..31 are reserved)
      ret = dsp_reg_write_32(IVA2_CM_CLKSTCTRL, 0);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Enable IVA2_CLK (set bit 1, bits 1..31 are reserved)
      ret = dsp_reg_write_32(IVA2_CM_FCLKEN, 1);
   }

   pxxx();
   
   if(DSP_ERR_OK == ret)
   {
      // Enable automatic transition (sets reserved bit 1 ??)
      ret = dsp_reg_write_32(IVA2_CM_CLKSTCTRL, 3);
   }

   pxxx();

   if(DSP_ERR_OK == ret)
   {
      // Clear RST1 bit to release DSP from reset
      ret = dsp_reg_bit_clear(IVA2_PRM_RSTCTRL, 0);
   }

   pxxx();

   return ret;
}
#endif
