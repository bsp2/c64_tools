/* ----
 * ---- file   : dsp_c64.c
 * ---- authors: Bastian Spiegel <bs@tkscript.de>
 * ----          DSP power on/off code based on TI's SYSLINK omap3530pwr.c
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 15Sep2013, 28Sep2013, 29Sep2013
 * ----          02Oct2013, 03Oct2013, 04Oct2013, 13Oct2013, 27Oct2013, 29Oct2013, 13Dec2013
 * ----          15Dec2013, 14Jan2014, 16Jan2014, 17Jan2014
 * ----
 * ----
 */

//#define USE_DSP_INFINITE_LOOP defined

#define MAX_WAIT_COUNT 0x100000u

#define __NO_VERSION__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include "../include/types.h"

#define C64_KBUILD defined
#include "../include/dsp_common.h"
#include "../include/dsp_config.h"
#include "../include/msg.h"

#include "../include/arch/dm3730/dm3730.h"

#include "kmod.h"
#include "reg.h"
#include "mlb.h"
#include "dev.h"
#include "dsp_c64.h"


enum {
   ST_OFF     = 0, /* powered off */
   ST_RESET   = 1, /* powered on but in reset */
   ST_RUNNING = 2, /* powered on / running */

   __NUM_STATES__
};


/*--------------------------------------------------------------------------- module vars */
static atomic_t at_dsp_state = ATOMIC_INIT(ST_OFF);

static sU32 dsp_entry_point = 0;

static DEFINE_SPINLOCK(dsp_spinlock);


/*--------------------------------------------------------------------------- dsp_poweron_and_reset() */
int dsp_poweron_and_reset(void) {
   int ret;
   int dspState;

   dspState = atomic_read(&at_dsp_state);

   if(ST_OFF == dspState)
   {
      unsigned long intFlags;
      sUI timeout;

#ifdef USE_DSP_POWER_NOTIFY
      /* Notify kernel that DSP is about to be powered on */
      dsp_power_notify(S_TRUE/*1*/);
#else
      dsp_power_notify__STUB(S_TRUE/*1*/);
#endif /* USE_DSP_POWER_NOTIFY */

      /* Disable interrupts */
      spin_lock_irqsave(&dsp_spinlock, intFlags);

      /* disable automatic clock gating */
      reg32_write(IVA2_CM_CLKSTCTRL, 0);
      reg32_read(IVA2_CM_CLKSTCTRL);
      
      /* turn off IVA2 domain functional clock */
      reg32_write(IVA2_CM_FCLKEN, 0);
      reg32_read(IVA2_CM_FCLKEN);
      
      /* assert reset on DSP, SEQ */
      reg32_write(IVA2_PRM_RSTCTRL, 5);
      reg32_read(IVA2_PRM_RSTCTRL);
      
      /* assert reset on MMU, interconnect, etc. */
      reg32_write(IVA2_PRM_RSTCTRL,
                  reg32_read(IVA2_PRM_RSTCTRL) | (1 << 1)
                  );
      reg32_read(IVA2_PRM_RSTCTRL);

      /* set next IVA2 power state to ON */
      reg32_write(IVA2_PRM_PWSTCTRL,
                  reg32_read(IVA2_PRM_PWSTCTRL) |
                  (3 << 20) |     /* SharedL2CacheFlatOnState = ON */
                  (3 << 0)        /* PowerState = ON */
                  );
      reg32_read(IVA2_PRM_PWSTCTRL);

      /* start a software supervised wake-up transition */
      reg32_write(IVA2_CM_CLKSTCTRL, 0x2);
      reg32_read(IVA2_CM_CLKSTCTRL);

      /* wait for IVA2 power domain to switch ON */
      for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
      {
         if(0x3 == (reg32_read(IVA2_PRM_PWSTST) & 0x3))
         {
            break;
         }
      }

      if(MAX_WAIT_COUNT != timeout)
      {
         /* clear power-on transition request */
         reg32_write(IVA2_CM_CLKSTCTRL, 0);
         reg32_read(IVA2_CM_CLKSTCTRL);

      /* Allow over/underclocking. */
#ifndef USE_FORCED_LOWPOWER_BYPASS
         // (todo) reg32_write(IVA2_CM_CLKEN_PLL, (1u<<4) | 7u);
         // (todo) reg32_read(IVA2_CM_CLKEN_PLL);
#endif /* USE_FORCED_LOWPOWER_BYPASS */

         /* turn on IVA2 domain functional clock */
         // (todo) dev_iva2_clk_set_enable(S_TRUE);
         reg32_write(IVA2_CM_FCLKEN, 1);
         reg32_read(IVA2_CM_FCLKEN);

         /* ensure the IVA2_SW_RST2 status bit is cleared */
         reg32_write(IVA2_PRM_RSTST,
                     reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                     );
         reg32_read(IVA2_PRM_RSTST);

         /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
         reg32_write(IVA2_PRM_RSTCTRL,
                     reg32_read(IVA2_PRM_RSTCTRL) & ~(1 << 1)
                     );
         reg32_read(IVA2_PRM_RSTCTRL);

         /* wait for IVA2_RST2 signal to be released */
         for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
         {
            if(0 != (reg32_read(IVA2_PRM_RSTST) & (1 << 9)))
            {
               break;
            }
         }

         if(MAX_WAIT_COUNT != timeout)
         {
            /* clear the IVA2_SW_RST2 status bit */
            reg32_write(IVA2_PRM_RSTST,
                        reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                        );
            reg32_read(IVA2_PRM_RSTST);

            /* wait for IVA2 state to become active */
            for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
            {
               if(0 == (reg32_read(IVA2_CM_IDLEST) & 0x1))
               {
                  break;
               }
            }

            if(MAX_WAIT_COUNT != timeout)
            {
               /* enable automatic hardware supervised transitions */
               reg32_write(IVA2_CM_CLKSTCTRL, 0x3);
               reg32_read(IVA2_CM_CLKSTCTRL);

               /* Set EN_MPU and EN_CORE bits => reduces idle power consumption by ~20mA (~80mW/h) */
               // (todo) reg32_write(IVA2_PRM_WKDEP, 3);
               // (todo) reg32_read(IVA2_PRM_WKDEP);
               
               /* Next dsp_start() is a reboot */
               mlb_bootflag_set(DSP_BOOTFLAG_REBOOT);
               
               atomic_set(&at_dsp_state, ST_RESET);

               /* Succeeded */
               ret = 0;
            }
            else
            {
               /* Error: IVA2 did not become active */
               printk(KERN_ERR "c64::dsp_poweron_and_reset: IVA2 did not become active.\n");

               ret = -EBUSY;
            }
            
         }
         else
         {
            /* Error: IVA2_RST2 signal was not released */
            printk(KERN_ERR "c64::dsp_poweron_and_reset: IVA2_RST2 signal was not released.\n");

            ret = -EBUSY;
         }
        
      }
      else
      {
         /* Error: IVA2 power domain failed to switch ON */
         printk(KERN_ERR "c64::dsp_poweron_and_reset: IVA2 power domain failed to switch ON.\n");

         ret = -EBUSY;
      }
      

      /* Enable interrupts */
      spin_unlock_irqrestore(&dsp_spinlock, intFlags);

      /* Print DSP power state */
      if(c64_pwrlog)
      {
         sU32 val = reg32_read(IVA2_PRM_PWSTST); /* 4830 60e4 */

         printk(KERN_DEBUG "c64::dsp_poweron_and_reset: IVA2_PRM_PWSTST=0x%08x => %s\n", 
                val,
                (0 == (val & 3u)) ? "DSP is ON" : "DSP is still off!!!"
                );
      }

   }
   else
   {
      if(ST_RESET != dspState)
      {
         /* Failed: DSP is in unexpected state */
         printk(KERN_ERR "c64::dsp_poweron_and_reset: wrong DSP state, expected %d, have %d.\n",
                ST_OFF,
                dspState
                );
         
         ret = -EACCES;
      }
      else
      {
         /* Succeeded (already reset) */
         ret = 0;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_start() */
int dsp_start(sU32 _entryPoint) {
   int ret;
   int dspState;

   dspState = atomic_read(&at_dsp_state);

   if(ST_RESET == dspState)
   {
      if(C64_DEBUG_LEVEL(1))
      {
         printk(KERN_DEBUG "c64::dsp_start: dsp_config.ram.phys_addr=0x%08x size=0x%08x\n",
                dsp_config.ram.phys_addr,
                dsp_config.ram.size
                );
      }

      if((_entryPoint >= dsp_config.ram.phys_addr) &&
         (_entryPoint <  (dsp_config.ram.phys_addr + dsp_config.ram.size))
         )
      {
         void __iomem *virtAddr;

         /* Generate C64+ startup code (this block is partially borrowed from TI) */
         
         /* Map reset vector to GPP */

         /* (note) Must use kvirtaddr already obtained by dma_alloc_coherent()
          *         Calling ioremap_nocache() causes a kernel crash
          */
#ifdef USE_PLATFORM_DRIVER
         virtAddr = (void __iomem *)dma_virt_4m;
#else
         /* (note) do not have a kvirtual address, yet. */
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_DEBUG "c64::dsp_start: ioremap_nocache(0x%08x, 0x%08x)\n",
                   dsp_config.reset_vector.phys_addr, DSP_RESET_VECTOR_CODESIZE
                   );
         }

         virtAddr = ioremap_nocache(dsp_config.reset_vector.phys_addr, DSP_RESET_VECTOR_CODESIZE);
#endif /* USE_PLATFORM_DRIVER */
         
         if(NULL != virtAddr)
         {
            __iomem sU32 *code  = (__iomem sU32*) virtAddr;
            sU32  entryPtAddrHi = 0x00000000u;
            sU32  entryPtAddrLo = 0x00000000u;
            sU32  startOpCodeLo = 0x0000002Au;
            sU32  startOpCodeHi = 0x0000006Au;
            sU32  noOp          = 0x00400000u;
            sU32  branch        = 0x00000362u;
            
#ifndef USE_DSP_INFINITE_LOOP
            sU32 entryAddr = _entryPoint;
#else
            sU32 entryAddr = dsp_config.reset_vector.phys_addr;
#endif /* USE_DSP_INFINITE_LOOP */

            unsigned long intFlags;
            
            if(C64_DEBUG_LEVEL(1))
            {
               printk(KERN_DEBUG "c64::dsp_start: entryPoint=0x%08x\n", _entryPoint);
            }
            
            /* Write branch to boot.64P in .sysinit segment (_c_int00) */
            entryPtAddrHi  = (entryAddr >> 16);
            entryPtAddrLo  = (entryAddr & 0xFFFFu);
            startOpCodeHi |= (entryPtAddrHi << 7);
            startOpCodeLo |= (entryPtAddrLo << 7);
            *code++ = startOpCodeLo;
            *code++ = startOpCodeHi;
            *code++ = branch;
            
            /* Write 5 no-ops for pipeline flush */
            *code++ = noOp;
            *code++ = noOp;
            *code++ = noOp;
            *code++ = noOp;
            *code++ = noOp;
            
#ifndef USE_PLATFORM_DRIVER
            /* Unmap reset vector */
            iounmap(virtAddr);
#endif /* USE_PLATFORM_DRIVER */

            /* Disable interrupts */
            spin_lock_irqsave(&dsp_spinlock, intFlags);

            /* Set reset vector */
            reg32_write(IVA2_SCM_GENERAL_BOOTADDR, dsp_config.reset_vector.phys_addr);
            reg32_read(IVA2_SCM_GENERAL_BOOTADDR);
            
            /* Set boot mode to "Direct boot" */
            reg32_write(IVA2_SCM_GENERAL_BOOTMOD, 0);
            reg32_read(IVA2_SCM_GENERAL_BOOTMOD);
            
            /* Enable GPTIMER5 functional clock */
            reg32_bit_set(IVA2_PER_CM_FCLKEN, 6);
            reg32_read(IVA2_PER_CM_FCLKEN);
            
            /* Enable GPTIMER5 interface clock (EN_GPT5) */
            reg32_bit_set(IVA2_PER_CM_ICLKEN, 6);
            reg32_read(IVA2_PER_CM_ICLKEN);
            
            /* Set GPTIMER5 clock source to SYS_CLK */
            reg32_bit_set(IVA2_PER_CM_CLKSEL, 3);
            reg32_read(IVA2_PER_CM_CLKSEL);
            
            /* Clear RST1 bit to release DSP from reset */
            reg32_bit_clear(IVA2_PRM_RSTCTRL, 0);
            reg32_read(IVA2_PRM_RSTCTRL);

            /* Enable interrupts */
            spin_unlock_irqrestore(&dsp_spinlock, intFlags);
            
            dsp_entry_point = _entryPoint;

            /* Wait for DSP startup */
#ifndef USE_DSP_INFINITE_LOOP
            {
               sUI i;

               for(i=0; i<100; i++)
               {
                  if(DSP_BOOTFLAG_RUNNING == mlb_bootflag_get())
                  {
                     break;
                  }
                  udelay(100);
               }
            }
#else
            mlb_bootflag_set(DSP_BOOTFLAG_RUNNING);
#endif /* USE_DSP_INFINITE_LOOP */
            
            {
               sU32 bootFlag = mlb_bootflag_get();

               if(DSP_BOOTFLAG_RUNNING == bootFlag)
               {
                  atomic_set(&at_dsp_state, ST_RUNNING);
                  
                  if(c64_pwrlog)
                  {
                     printk(KERN_DEBUG "c64::dsp_start: DSP started @0x%08x\n", dsp_entry_point);
                  }
                  
                  /* Succeeded */
                  ret = 0;
               }
               else
               {
                  /* Failed: DSP is not signaling RUNNING state */
                  printk(KERN_ERR "c64::dsp_start: FAILED to start DSP! (bootflag=0x%08x)\n",
                         bootFlag
                         );

                  ret = -EBUSY;
               }
            }
         }
         else
         {
            /* Failed: ioremap_nocache() */
            printk(KERN_ERR "c64::dsp_start: ioremap_nocache() failed.\n");
            
            ret = -EFAULT;
         }
      }
      else
      {
         /* Failed: DSP entry point outside of DSP image */
         printk(KERN_ERR "c64::dsp_start: dsp_entry_point (0x%08x) outside of DSP image area.\n",
                _entryPoint
                );

         ret = -EINVAL;
      }
   }
   else
   {
      /* Failed: DSP is in unexpected state */
      printk(KERN_ERR "c64::dsp_start: wrong DSP state, expected %d, have %d.\n",
             ST_RESET,
             dspState
             );

      ret = -EACCES;
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_resume() */
int dsp_resume(void) {
   int ret;

   int dspState = atomic_read(&at_dsp_state);

   if(ST_OFF == dspState)
   {
      if(0 != dsp_entry_point)
      {
         if(C64_DEBUG_LEVEL(1))
         {
            printk(KERN_DEBUG "c64::dsp_resume: resuming DSP program execution @0x%08x\n",
                   dsp_entry_point
                   );
         }

         ret = dsp_poweron_and_reset();

         if(0 == ret)
         {
            /* Resume */
            mlb_bootflag_set(DSP_BOOTFLAG_RESUME);

            ret = dsp_start(dsp_entry_point);
         }
      }
      else
      {
         /* Failed: DSP has not been started before */
         printk(KERN_ERR "c64::dsp_resume: dsp_entry_point is NULL.\n");

         ret = -EACCES;
      }
   }
   else
   {
      if(ST_RUNNING != dspState)
      {
         /* Failed: DSP is in unexpected state */
         printk(KERN_ERR "c64::dsp_resume: wrong DSP state, expected %d, have %d.\n",
                ST_OFF,
                dspState
                );
         
         ret = -EACCES;
      }
      else
      {
         /* Succeeded (already running) */
         ret = 0;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- dsp_poweroff() */
int dsp_poweroff(void) {
   int ret;
   unsigned long intFlags;
   int dspState;

   dspState = atomic_read(&at_dsp_state);

   if(ST_OFF != dspState)
   {
      sUI timeout;
#if 0
      //udelay(10);  /* weird: w/o this, sporadic IVA2 inband errors occur during c64_pwrbench !?? */

      /* maybe this will make a difference on OMAP3530 ?
       *
       *  it's kind of a desperate attempt, though.
       *
       */
      {
         sUI delayCnt = 0;
         while(delayCnt++ < 4u)
         {
            udelay(250);
         }
      }
#endif
      
      /* Disable interrupts */
      spin_lock_irqsave(&dsp_spinlock, intFlags);

      /* Disable GPTIMER5 functional clock (EN_GPT5) */
      reg32_bit_clear(IVA2_PER_CM_FCLKEN, 6);
      reg32_read(IVA2_PER_CM_FCLKEN);

      udelay(10); /* definitely not needed on DM3730. maybe improves matters on 3530? */

      /* Disable GPTIMER5 interface clock (EN_GPT5) */
      reg32_bit_clear(IVA2_PER_CM_ICLKEN, 6);
      reg32_read(IVA2_PER_CM_ICLKEN);

      udelay(10); /* definitely not needed on DM3730. maybe improves matters on 3530? */


      /* Enable DPLL low power mode bypass */
      // (todo) reg32_write(IVA2_CM_CLKEN_PLL, (1u<<4) | 5u);
      // (todo) reg32_read(IVA2_CM_CLKEN_PLL);

      /* Remove power domain dependencies */
      // (todo) reg32_write(IVA2_PRM_WKDEP, 0); /* 4830 60c8 */
      // (todo) reg32_read(IVA2_PRM_WKDEP);

      /* Turn on auto-idle enable */
      // (todo) reg32_write(IVA2_CM_AUTOIDLE_PLL, 1);
      // (todo) reg32_read(IVA2_CM_AUTOIDLE_PLL);


      /* MPU controlled ON + ACTIVE */

      /* clear power state change request */
      reg32_write(IVA2_CM_CLKSTCTRL, 0);
      reg32_read(IVA2_CM_CLKSTCTRL);

      /* turn off IVA2 domain functional clock (required to assert reset) */
      // (todo) dev_iva2_clk_set_enable(S_FALSE);
      reg32_write(IVA2_CM_FCLKEN, 0);
      reg32_read(IVA2_CM_FCLKEN);

      /* assert reset on DSP, SEQ */
      reg32_write(IVA2_PRM_RSTCTRL, 0x5);
      reg32_read(IVA2_PRM_RSTCTRL);

      /* assert reset on MMU, interconnect, etc. */
      reg32_write(IVA2_PRM_RSTCTRL,
                  reg32_read(IVA2_PRM_RSTCTRL) | (1 << 1)
                  );
      reg32_read(IVA2_PRM_RSTCTRL);

      /* set next IVA2 power state to ON */
      reg32_write(IVA2_PRM_PWSTCTRL,
                  reg32_read(IVA2_PRM_PWSTCTRL) | (3 << 0)
                  );
      reg32_read(IVA2_PRM_PWSTCTRL);

      /* start a software supervised wake-up transition */
      reg32_write(IVA2_CM_CLKSTCTRL, 0x2);
      reg32_read(IVA2_CM_CLKSTCTRL);

      /* wait for IVA2 power domain to switch ON */
      for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
      {
         if(0x3 == (reg32_read(IVA2_PRM_PWSTST) & 0x3))
         {
            break;
         }
      }

      if(MAX_WAIT_COUNT != timeout)
      {
        /* clear power-on transition request */
         reg32_write(IVA2_CM_CLKSTCTRL, 0);
         reg32_read(IVA2_CM_CLKSTCTRL);

         /* turn on IVA2 domain functional clock */
         // (todo) dev_iva2_clk_set_enable(S_TRUE);
         reg32_write(IVA2_CM_FCLKEN, 1);
         reg32_read(IVA2_CM_FCLKEN);

         /* ensure the IVA2_SW_RST2 status bit is cleared */
         reg32_write(IVA2_PRM_RSTST,
                     reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                     );
         reg32_read(IVA2_PRM_RSTST);

         /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
         reg32_write(IVA2_PRM_RSTCTRL,
                     reg32_read(IVA2_PRM_RSTCTRL) & ~(1 << 1)
                     );
         reg32_read(IVA2_PRM_RSTCTRL);

         /* wait for IVA2_RST2 signal to be released */
         for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
         {
            if(0 != (reg32_read(IVA2_PRM_RSTST) & (1 << 9)))
            {
               break;
            }
         }

         if(MAX_WAIT_COUNT != timeout)
         {
            /* clear the IVA2_SW_RST2 status bit */
            reg32_write(IVA2_PRM_RSTST,
                        reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                        );
            reg32_read(IVA2_PRM_RSTST);

            /* wait for IVA2 state to become active (~2 loops @ 312MHz) */
            for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
            {
               if(0 == (reg32_read(IVA2_CM_IDLEST) & 0x1))
               {
                  break;
               }
            }

            if(MAX_WAIT_COUNT != timeout)
            {
               /* MPU controlled OFF */

               /* turn off IVA2 domain functional clock (required to assert reset) */
               // (todo) dev_iva2_clk_set_enable(S_FALSE);
               reg32_write(IVA2_CM_FCLKEN, 0);
               reg32_read(IVA2_CM_FCLKEN);

               /* assert reset on MMU, interconnect, etc. */
               reg32_write(IVA2_PRM_RSTCTRL,
                           reg32_read(IVA2_PRM_RSTCTRL) | (1 << 1)
                           );
               reg32_read(IVA2_PRM_RSTCTRL);

               /* wait for IVA2 state to become idle */
               for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
               {
                  if(0x1 == (reg32_read(IVA2_CM_IDLEST) & 0x1))
                  {
                     break;
                  }
               }

               if(MAX_WAIT_COUNT != timeout)
               {
                  /* set next IVA2 power state to OFF */
                  reg32_write(IVA2_PRM_PWSTCTRL,
                              reg32_read(IVA2_PRM_PWSTCTRL) & ~(0x3)
                              );
                  reg32_read(IVA2_PRM_PWSTCTRL);

                  /* start a software supervised sleep transition */
                  reg32_write(IVA2_CM_CLKSTCTRL, 0x1);
                  reg32_read(IVA2_CM_CLKSTCTRL);

                  /* wait for IVA2 power domain to switch OFF */
                  for(timeout = 0; timeout < MAX_WAIT_COUNT /*+0x01, WTH?*/; timeout++)
                  {
                     if(0 == (reg32_read(IVA2_PRM_PWSTST) & 0x3))
                     {
                        break;
                     }
                  }

                  if(MAX_WAIT_COUNT != timeout)
                  {
                     /* clear power state change request */
                     reg32_write(IVA2_CM_CLKSTCTRL, 0);
                     reg32_read(IVA2_CM_CLKSTCTRL);

                     /* MPU controlled ON + ACTIVE */

                     /* set next IVA2 power state to ON */
                     reg32_write(IVA2_PRM_PWSTCTRL,
                                 reg32_read(IVA2_PRM_PWSTCTRL) | (3 << 0)
                                 );
                     reg32_read(IVA2_PRM_PWSTCTRL);

                     /* start a software supervised wake-up transition */
                     reg32_write(IVA2_CM_CLKSTCTRL, 0x2);
                     reg32_read(IVA2_CM_CLKSTCTRL);

                     /* wait for IVA2 power domain to switch ON */
                     for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
                     {
                        if(0x3 == (reg32_read(IVA2_PRM_PWSTST) & 0x3))
                        {
                           break;
                        }
                     }

                     if(MAX_WAIT_COUNT != timeout)
                     {
                        /* clear power-on transition request */
                        reg32_write(IVA2_CM_CLKSTCTRL, 0);
                        reg32_read(IVA2_CM_CLKSTCTRL);

                        /* turn on IVA2 domain functional clock */
                        // (todo) dev_iva2_clk_set_enable(S_TRUE);
                        reg32_write(IVA2_CM_FCLKEN, 1);
                        reg32_read(IVA2_CM_FCLKEN);

                        /* ensure the IVA2_SW_RST2 status bit is cleared */
                        reg32_write(IVA2_PRM_RSTST,
                                    reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                                    );
                        reg32_read(IVA2_PRM_RSTST);

                        /* release IVA2_RST2 reset (MMU, SYSC, local interconnect, etc) */
                        reg32_write(IVA2_PRM_RSTCTRL,
                                    reg32_read(IVA2_PRM_RSTCTRL) & ~(1 << 1)
                                    );
                        reg32_read(IVA2_PRM_RSTCTRL);

                        /* wait for IVA2_RST2 signal to be released */
                        for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
                        {
                           if(0 != (reg32_read(IVA2_PRM_RSTST) & (1 << 9)))
                           {
                              break;
                           }
                        }
                        
                        if(MAX_WAIT_COUNT != timeout)
                        {
                           /* clear the IVA2_SW_RST2 status bit */
                           reg32_write(IVA2_PRM_RSTST,
                                       reg32_read(IVA2_PRM_RSTST) | (1 << 9)
                                       );
                           reg32_read(IVA2_PRM_RSTST);

                           /* first wait for IVA2 state to become active */
                           for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
                           {
                              if(0 == (reg32_read(IVA2_CM_IDLEST) & 0x1))
                              {
                                 break;
                              }
                           }

                           if(MAX_WAIT_COUNT != timeout)
                           {
                              /* set DSP boot address and boot mode */
                              reg32_write(IVA2_SCM_GENERAL_BOOTADDR, 0);
                              reg32_read(IVA2_SCM_GENERAL_BOOTADDR);
                              reg32_write(IVA2_SCM_GENERAL_BOOTMOD, 1);
                              reg32_read(IVA2_SCM_GENERAL_BOOTMOD);

                              /* enable automatic clock gating */
                              reg32_write(IVA2_CM_CLKSTCTRL, 3);
                              reg32_read(IVA2_CM_CLKSTCTRL);

                              /* set next IVA2 power state to OFF */
                              reg32_write(IVA2_PRM_PWSTCTRL,
                                          reg32_read(IVA2_PRM_PWSTCTRL) & ~(0x3)
                                          );
                              reg32_read(IVA2_PRM_PWSTCTRL);

                              /* ensure the IVA2_SW_RST1 status bit is cleared */
                              reg32_write(IVA2_PRM_RSTST,
                                          reg32_read(IVA2_PRM_RSTST) | (1 << 8)
                                          );
                              reg32_read(IVA2_PRM_RSTST);
                              
                              /* release DSP from reset */
                              reg32_write(IVA2_PRM_RSTCTRL,
                                          reg32_read(IVA2_PRM_RSTCTRL) & ~(1 << 0)
                                          );
                              reg32_read(IVA2_PRM_RSTCTRL);

                              /* wait for IVA2_RST1 signal to be released */
                              for(timeout = 0; timeout < MAX_WAIT_COUNT; timeout++)
                              {
                                 if(0 != (reg32_read(IVA2_PRM_RSTST) & (1 << 8)))
                                 {
                                    break;
                                 }
                              }
                              
                              if(MAX_WAIT_COUNT != timeout)
                              {
                                 /* clear the IVA2_SW_RST1 status bit */
                                 reg32_write(IVA2_PRM_RSTST,
                                             reg32_read(IVA2_PRM_RSTST) | (1 << 8)
                                             );
                                 reg32_read(IVA2_PRM_RSTST);
                                 
                                 /* wait for IVA2 power domain to switch OFF (~7 loops @ 312MHz) */
                                 for(timeout = 0; timeout < MAX_WAIT_COUNT /*+1, WTH?*/; timeout++)
                                 {
                                    if(0 == (reg32_read(IVA2_PRM_PWSTST) & 0x3))
                                    {
                                       break;
                                    }
                                 }

                                 if(MAX_WAIT_COUNT != timeout)
                                 {
                                    /* assert reset on DSP, SEQ */
                                    reg32_write(IVA2_PRM_RSTCTRL, 0x5);
                                    reg32_read(IVA2_PRM_RSTCTRL);
                                    
                                    /* assert reset on MMU, interconnect, etc. */
                                    reg32_write(IVA2_PRM_RSTCTRL,
                                                reg32_read(IVA2_PRM_RSTCTRL) | (1 << 1)
                                                );
                                    reg32_read(IVA2_PRM_RSTCTRL);

                                    /* disable automatic clock gating */
                                    reg32_write(IVA2_CM_CLKSTCTRL, 0);
                                    reg32_read(IVA2_CM_CLKSTCTRL);

                                    /* turn off IVA2 domain functional clock */
                                    // (todo) dev_iva2_clk_set_enable(S_FALSE);
                                    reg32_write(IVA2_CM_FCLKEN, 0);
                                    reg32_read(IVA2_CM_FCLKEN);

                                    /* Succeeded */
                                    atomic_set(&at_dsp_state, ST_OFF);
                                    
                                    ret = 0;
                                 }
                                 else
                                 {
                                    /* Failed: IVA2 power domain not OFF */
                                    printk(KERN_ERR "c64::dsp_poweroff: IVA2 power domain not OFF.\n");

                                    ret = -EBUSY;
                                 }

                              }
                              else
                              {
                                 /* Failed: IVA2_RST1 signal was not released */
                                 printk(KERN_ERR "c64::dsp_poweroff: IVA2_RST1 signal was not released.\n");

                                 ret = -EBUSY;
                              }
                              
                           }
                           else
                           {
                              /* Failed: IVA2 did not become active */
                              printk(KERN_ERR "c64::dsp_poweroff: IVA2 did not become active.\n");

                              ret = -EBUSY;
                           }
                                                      
                        }
                        else
                        {
                           /* Failed: IVA2_RST2 signal was not released */
                           printk(KERN_ERR "c64::dsp_poweroff: IVA2_RST2 signal was not released.\n");

                           ret = -EBUSY;
                        }

                     }
                     else
                     {
                        /* Failed: IVA2 power domain not ON */
                        printk(KERN_ERR "c64::dsp_poweroff: IVA2 power domain not ON.\n");

                        ret = -EBUSY;
                     }
                     
                  }
                  else
                  {
                     /* Failed: IVA2 power domain not OFF */
                     printk(KERN_ERR "c64::dsp_poweroff: IVA2 power domain not OFF.\n");

                     ret = -EBUSY;
                  }
                  
               }
               else
               {
                  /* Failed: IVA2 did not become idle */
                  printk(KERN_ERR "c64::dsp_poweroff: IVA2 did not become idle.\n");

                  ret = -EBUSY;
               }
               
            }
            else
            {
               /* Failed: IVA2 did not become active */
               printk(KERN_ERR "c64::dsp_poweroff: IVA2 did not become active.\n");

               ret = -EBUSY;
            }
            
         }
         else
         {
            /* Failed: IVA2_RST2 signal was not released */
            printk(KERN_ERR "c64::dsp_poweroff: IVA2_RST2 signal was not released.\n");

            ret = -EBUSY;
         }
         
      }
      else
      {
         /* Failed: IVA2 power domain failed to switch ON */
         printk(KERN_ERR "c64::dsp_poweroff: IVA2 power domain failed to switch ON\n");

         ret = -EBUSY;
      }


      /* Enable interrupts */
      spin_unlock_irqrestore(&dsp_spinlock, intFlags);

      /* Print DSP power state */
      if(c64_pwrlog)
      {
         sU32 val = reg32_read(IVA2_PRM_PWSTST); /* 4830 60e4 */

         printk(KERN_DEBUG "c64::dsp_poweroff: IVA2_PRM_PWSTST=0x%08x => %s\n", 
                val,
                (0 == (val & 3u)) ? "DSP is OFF" : "DSP is still on!!!"
                );
      }

      if(0 == ret)
      {
         /* Notify kernel that DSP is now powered off */
#ifdef USE_DSP_POWER_NOTIFY
         dsp_power_notify(S_FALSE/*0*/);
#else
         dsp_power_notify__STUB(S_FALSE/*0*/);
#endif /* USE_DSP_POWER_NOTIFY */
      }

   }
   else
   {
      /* Succeeded: DSP is already powered off */
      ret = 0;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- dsp_init() */
sBool dsp_init(void) {

   return S_TRUE;
}


/*--------------------------------------------------------------------------- dsp_exit() */
void dsp_exit(void) {

   (void)dsp_poweroff();
}
