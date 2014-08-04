/* ----
 * ---- file   : kmod.c
 * ---- authors: Bastian Spiegel <bs@tkscript.de>
 * ----          Gražvydas Ignotas <notasas@gmail.com> (platform driver ref. code, kernel patch to remove the need
 * ----                                                   for static DSP image memory reservation at boot time)
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 14Sep2013, 15Sep2013, 20Sep2013
 * ----          02Oct2013, 13Dec2013, 15Dec2013, 14Jan2014, 18Jan2014
 * ----
 * ----
 */

#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE defined

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>

#include "../include/types.h"
#include "kmod.h"

#ifdef USE_PLATFORM_DRIVER
#include <linux/platform_device.h>
#endif /* USE_PLATFORM_DRIVER */

#include "../include/dsp_common.h"
#include "../include/dsp_config.h"
#include "../include/msg.h"

#include "reg.h"
#include "mlb.h"
#include "dsp_c64.h"
#include "dev.h"


/*--------------------------------------------------------------------------- global vars */
sUI c64_debug_level = 0;
sBool c64_pwrlog = S_FALSE;
sBool c64_pwroff = S_TRUE;


/*--------------------------------------------------------------------------- module vars */


/*--------------------------------------------------------------------------- platform driver */
#ifdef USE_PLATFORM_DRIVER
static int __devinit c64_tools_probe(struct platform_device *_pdev);
static int __devexit c64_tools_remove(struct platform_device *_pdev);

static struct platform_driver c64_tools_driver = {
	.probe		= &c64_tools_probe,
	.remove		= &c64_tools_remove,
	.driver		= {
		.name	= "c64_tools",
		.owner	= THIS_MODULE,
	},
};

static dma_addr_t dma_addr_4m;
void *dma_virt_4m;

#endif /* USE_PLATFORM_DRIVER */


/*--------------------------------------------------------------------------- dsp_power_notify__STUB() */
#ifndef USE_DSP_POWER_NOTIFY
volatile int dsp_enabled__STUB;

void dsp_power_notify__STUB(int _bEnabled) {
   dsp_enabled__STUB = _bEnabled;
}
#endif /* USE_DSP_POWER_NOTIFY */


/*--------------------------------------------------------------------------- c64_tools_probe() */
#ifdef USE_PLATFORM_DRIVER
int __devinit c64_tools_probe(struct platform_device *_pdev) {
   int ret;

	dma_virt_4m = dma_alloc_coherent(&_pdev->dev,
                                    DSP_MAX_IMAGE_SIZE, /* must currently be set to exactly 4 MBytes */
                                    &dma_addr_4m,
                                    GFP_KERNEL
                                    );
   
   if(NULL != dma_virt_4m)
   {
      if(C64_DEBUG_LEVEL(1))
      {
         dev_info(&_pdev->dev, "c64::c64_tools_probe: DSP image area available at kvirt_addr=0x%p, phys_addr=0x%p\n",
                  (void*)dma_virt_4m,
                  (void*)dma_addr_4m
                  );
      }

      if(dsp_config.reset_vector.phys_addr == dma_addr_4m)
      {
         /* Succeeded */
         if(C64_DEBUG_LEVEL(1))
         {
            dev_info(&_pdev->dev, "c64::c64_tools_probe: phys location correct, yay!\n");
         }

         ret = 0;
      }
      else
      {
         /* Failed: DSP image area not located at required address */
         dev_err(&_pdev->dev, "c64::c64_tools_probe: 4M buffer for DSP image not at required phys_addr (have=%p, need=0x%08x!\n",
                 (void*)dma_addr_4m,
                 dsp_config.reset_vector.phys_addr
                 );

         dma_free_coherent(&_pdev->dev,
                           DSP_MAX_IMAGE_SIZE,
                           dma_virt_4m,
                           dma_addr_4m
                           );

         ret = -ENOMEM;
      }
   }
   else
   {
      /* Failed: 4M buffer not available */
      dev_err(&_pdev->dev, "c64::c64_tools_probe: no 4M buffer!\n");

      ret = -ENOMEM;
   }

   return ret;
}


/*--------------------------------------------------------------------------- c64_tools_remove() */
int __devexit c64_tools_remove(struct platform_device *_pdev) {
   dma_free_coherent(&_pdev->dev,
                     DSP_MAX_IMAGE_SIZE,
                     dma_virt_4m,
                     dma_addr_4m
                     );

   return 0;
}

#endif /* USE_PLATFORM_DRIVER */


/*--------------------------------------------------------------------------- c64_init() */
static int __init c64_init(void) {
   int ret;

   if(C64_DEBUG_LEVEL(1))
   {
      printk(KERN_DEBUG "c64::c64_init: ENTER\n");
   }

#ifdef USE_PLATFORM_DRIVER   
   ret = platform_driver_register(&c64_tools_driver);

   if(0 == ret)
#endif /* USE_PLATFORM_DRIVER */
   {
      /* Register "/dev/c64" char device */
      if(dev_init())
      {
         /* Setup mailbox and interrupt handler */
         if(mlb_init())
         {
            if(dsp_init())
            {
               mlb_set_ready_to_send();

               /* Allow client connections */
               dev_enable();

               /* Succeeded */
               ret = 0;
            }
            else
            {
               /* Failed: dsp_exit() */
               mlb_exit();

               dev_exit();

               ret = -1;
            }
         }
         else
         {
            /* Failed: mlb_init() */
            dev_exit();
            
            ret = -1;
         }
      }
      else
      {
         /* Failed: dev_init() */
         
         ret = -1;
      }
   }
#ifdef USE_PLATFORM_DRIVER
   else
   {
      /* Failed: platform_device_register() */
      printk(KERN_ERR "c64::c64_init: platform_device_register() failed (ret=%d).\n", ret);
   }
#endif /* USE_PLATFORM_DRIVER */

   if(C64_DEBUG_LEVEL(1))
   {
      printk(KERN_DEBUG "c64::c64_init: LEAVE ret=%d\n", ret);
   }
   else
   {
      printk(KERN_INFO "c64: init ok. module build date: %s"
#ifdef USE_PLATFORM_DRIVER
             "  (**%uMB DSP image size**)"
#endif /* PANDORA_VID2_MSG_HACK */
             "\n", __DATE__
#ifdef USE_PLATFORM_DRIVER
             , (DSP_MAX_IMAGE_SIZE >> 20u)
#endif /* PANDORA_VID2_MSG_HACK */
             );
   }

   return ret;
}


/*--------------------------------------------------------------------------- c64_exit() */
static void __exit c64_exit(void) {

   if(C64_DEBUG_LEVEL(1))
   {
      printk(KERN_DEBUG "c64::c64_exit: ENTER\n");
   }

   /* Power off IVA2 subsystem */
   dsp_exit();

   /* Disable mailbox interrupt and unregister handler */
   mlb_exit();

   /* Unregister char device */
   dev_exit();


#ifdef USE_PLATFORM_DRIVER
   platform_driver_unregister(&c64_tools_driver);
#endif /* USE_PLATFORM_DRIVER */

   if(C64_DEBUG_LEVEL(10))
   {
      printk(KERN_DEBUG "c64::c64_exit: LEAVE\n");
   }

   printk(KERN_INFO "c64: exit OK.\n");
}


/*--------------------------------------------------------------------------- c64_param_set_debug_level() */
static int c64_param_set_debug_level(const char *_val, struct kernel_param *_kp) {
   (void)_kp;

   sscanf(_val, "%u", &c64_debug_level);

   return 0;
}


/*--------------------------------------------------------------------------- c64_param_set_pwrlog() */
static int c64_param_set_pwrlog(const char *_val, struct kernel_param *_kp) {
   (void)_kp;

   sscanf(_val, "%d", &c64_pwrlog);

   c64_pwrlog = (0 != c64_pwrlog) ? S_TRUE : S_FALSE;

   return 0;
}


/*--------------------------------------------------------------------------- c64_param_set_pwroff() */
static int c64_param_set_pwroff(const char *_val, struct kernel_param *_kp) {
   (void)_kp;

   sscanf(_val, "%d", &c64_pwroff);

   c64_pwroff = (0 != c64_pwroff) ? S_TRUE : S_FALSE;

   return 0;
}


/*--------------------------------------------------------------------------- module exports */
MODULE_AUTHOR("Bastian Spiegel");
MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("TMS320C6000 init + ARM<>DSP messaging driver");
MODULE_SUPPORTED_DEVICE("omap3530, dm3730 (Open Pandora)");

module_param_call(debug, &c64_param_set_debug_level, NULL, NULL, 0200);
MODULE_PARM_DESC(debug, "debug level (0=no debug output, 20=trace reg access)");

module_param_call(pwrlog, &c64_param_set_pwrlog, NULL, NULL, 0200);
MODULE_PARM_DESC(pwrlog, "power log enable (0=off, 1=log suspend/resume)");

module_param_call(pwroff, &c64_param_set_pwroff, NULL, NULL, 0200);
MODULE_PARM_DESC(pwroff, "power off enable (0=power off only when kmod is unloaded, 1=power off when DSP is not in use (default)");

module_init(c64_init);
module_exit(c64_exit);
