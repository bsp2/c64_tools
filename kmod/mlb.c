/* ----
 * ---- file   : mlb.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 14Sep2013, 15Sep2013, 17Sep2013
 * ----          18Sep2013, 19Sep2013, 15Oct2013, 19Oct2013, 11Dec2013, 15Dec2013, 14Jan2014
 * ----
 * ----
 */

#define __NO_VERSION__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <linux/../../arch/arm/include/asm/cacheflush.h>


#include "../include/types.h"
#include "../include/msg.h"
#include "../include/dsp_common.h"
#define C64_KBUILD defined
#include "../include/dsp_config.h"

#include "../include/arch/dm3730/dm3730.h"

#include "kmod.h"
#include "reg.h"
#include "dev.h"
#include "mlb.h"


/*--------------------------------------------------------------------------- module vars */
static volatile sU32 mlb_irq_count = 0;
static volatile sU32 mlb_bh_count = 0;

static volatile sU32 mlb_num_recvd = 0;
static volatile sU32 mlb_num_sent = 0;  /* == num revd by the DSP */

static sU8 __iomem *mlb_base_virt;  /* also see "dsp_common.h" */

#define MLB_REG(a)  (*(volatile sU32*)( ((a) - MLB_BASE) + mlb_base_virt ))

static volatile sU32 mlb_inbox[MLB_FIFO_SIZE]; /* written by the IRQ handler. actually only one message. */

static sU8 __iomem *ipc_base_virt;

#define IPC_ADDR(a) (((a) - DSP_IPC_BASE_GPP) + ipc_base_virt)
#define IPC_REG(a)  (*(volatile sU32 __iomem*)( IPC_ADDR(a) ))

#define IPC_REG_READ(a) __raw_readl((const volatile void __iomem*)(IPC_ADDR(a)))
#define IPC_REG_WRITE(a, b) __raw_writel(b, (volatile void __iomem*)IPC_ADDR(a))

static sU32 msg_serial;

static atomic_t at_b_ready_to_send = ATOMIC_INIT(S_FALSE);  /* false while waiting for reply from DSP */


/*--------------------------------------------------------------------------- loc_mlb_reset() */
static void loc_mlb_reset(void) {

   /* Disable GPP mailbox interrupts */
   MLB_REG(MLB_IRQENABLE_0) = 0;
   
   /* Disable DSP mailbox interrupts */
   MLB_REG(MLB_IRQENABLE_1) = 0;

   /* Mailbox software reset + set SIDLEMODE to "smart-idle" and autoidle to
    *  automatic interface clocking (based on L4-core interface activity)
    */
   MLB_REG(MLB_SYSCONFIG) = 
      (MLB_REG(MLB_SYSCONFIG) & ~((1u << 3) | (1u << 4))) |
      (2u << 3) |  /* SIDLEMODE */
      (1u << 0) |  /* AUTOIDLE */
      (1u << 1) ;  /* SOFTRESET */

   /* Wait for mailbox software reset */
   while(1 != MLB_REG(MLB_SYSSTATUS))
   {
      /* Wait for RESETDONE bit */
   }
}


/*--------------------------------------------------------------------------- loc_mlb_reset_2() */
static void loc_mlb_reset_2(void) {
   /* GPP-side:
    *  Setup mailbox 0 for sending messages from the GPP to the DSP, and
    *        mailbox 1 for receiving messages from the DSP
    */
   MLB_REG(MLB_IRQENABLE_0) = 
      (
#if 0
         (1u << 1) |  /* Generate IRQ when msg was delivered to the DSP and FIFO slot becomes avail. */
#endif
         (1u << 2)    /* set NEWMSGENABLEUUMB1 --> Generate IRQ when msg was received from the DSP */
       );
   
   /* DSP-side:
    *  Setup mailbox 0 for receiving messages from the DSP to the GPP, and
    *        mailbox 1 for sending messages to the GPP
    */
   MLB_REG(MLB_IRQENABLE_1) = 
      (
#if 0
         (1u << 3) |  /* Generate IRQ when msg was delivered to the GPP and FIFO slot becomes avail. */
#endif
         (1u << 0)    /* set NEWMSGENABLEUUMB0 --> Generate IRQ when msg was received from the GPP */
       );
}


/*--------------------------------------------------------------------------- irq_handler__mlb */
static irqreturn_t irq_handler__mlb(int _irq,
                                    void * _devId /* priv data ptr (NULL) */
                                    ) {
   /* Top half IRQ handler (actually does the entire IRQ handling ATM) */
   sUI numMsg = 0;

   mlb_irq_count++;

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "irq_handler_mlb\n");
   }

   /* Read messages */
   {
      numMsg = MLB_REG(MLB_MSGSTATUS_1) & 7u; /* mask NBOFMSGMB field */

      if(numMsg > 0)
      {
         if(numMsg <= MLB_FIFO_SIZE)  /* sanity check */
         {
            sUI fifoIdx = 0;

            while(fifoIdx < numMsg)
            {
               mlb_inbox[fifoIdx] = MLB_REG(MLB_MESSAGE_1);

               {
                  mlb_num_recvd++;

                  if(C64_DEBUG_LEVEL(16))
                  {
                     printk(KERN_DEBUG "c64: mlb_inbox[%u/%u] = 0x%08x\n",
                            (fifoIdx + 1),
                            numMsg,
                            mlb_inbox[fifoIdx]
                            );
                  }
               }
               
               /* Next message */
               fifoIdx++;
            }
         }
         /* else: numMsg exceeds FIFO size */
      }
      /* else: no messages to read */
   }

   /* Acknowledge interrupt */
#ifndef DSP_MAILBOX_RESET
   if(numMsg > 0)
#endif
   {
      MLB_REG(MLB_IRQSTATUS_0) = (1u << 2);  /* clear NEWMSGSTATUSUUMB1 */
   
#ifdef DSP_MAILBOX_RESET
      MLB_REG(MLB_IRQENABLE_0) = 0;
#endif 

      /* Copy DSP reply to client structure */
      dev_handle_reply(IPC_REG(DSP_IPC_MSG_RET1_GPP),
                       IPC_REG(DSP_IPC_MSG_RET2_GPP)
                       );
      
      /* Ready to send the next message */
      mlb_set_ready_to_send();
   }

   return IRQ_HANDLED;
}


/*--------------------------------------------------------------------------- mlb_send_to_dsp() */
int mlb_send_to_dsp(void *_client, const dsp_msg_t *_msg) {
   int ret;
   sUI numMsg = 0;

   if(S_TRUE == atomic_read(&at_b_ready_to_send) || (NULL == _client)/*force testmsg*/)
   {
      atomic_set(&at_b_ready_to_send, S_FALSE);

      if(C64_DEBUG_LEVEL(10))
      {
         numMsg = MLB_REG(MLB_MSGSTATUS_1) & 7; /* mask NBOFMSGMB field */
         
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: %u unread msg on GPP-side.\n", numMsg);
      }
      
      /* Check FIFOSTATUS_0 and return EAGAIN if FIFO is still full */
      if(C64_DEBUG_LEVEL(10))
      {
         numMsg = MLB_REG(MLB_MSGSTATUS_0);
         
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: %u msg already in FIFO.\n", numMsg);
      }
      
      /* Will be written to DSP_IPC_MSG_SERIAL_GPP */
      msg_serial++;
      
#ifdef DSP_MAILBOX_RESET
      {
         loc_mlb_reset();
         loc_mlb_reset_2();
      }
#endif
      
      if(C64_DEBUG_LEVEL(20))
      {
         sU32 val;

         val = IPC_REG_READ(DSP_IPC_MSG_SERIAL_GPP);
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: last msgSerial=0x%08x (msg_serial=0x%08x)\n",
                val,
                msg_serial
                );
         
         val = IPC_REG_READ(DSP_IPC_ERRNO_GPP);
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: last errno=0x%08x\n", val);

         val = IPC_REG_READ(DSP_IPC_DEBUG_DSPITERCOUNT_GPP);
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: last dspIterCount=0x%08x\n", val);
         
         val = IPC_REG_READ(DSP_IPC_DEBUG_DSPIRQCOUNT_GPP);
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: last dspIrqCount=0x%08x\n", val);
         
         val = IPC_REG_READ(DSP_IPC_DEBUG_INTERNAL_GPP);
         printk(KERN_DEBUG "c64::mlb_send_to_dsp: last debugInternal=0x%08x\n", val);
      }
      
      
      if(0 == (MLB_REG(MLB_FIFOSTATUS_0) & 1))
      {
         /* Store message serial so the DSP can distinguish between interrupt spam
          *  (on my HW, it always receives at least two interrupts, even with the workarounds)
          *  and "real" messages.
          */
         if(C64_DEBUG_LEVEL(20))
         {
            printk(KERN_DEBUG "c64::mlb::mlb_send_to_dsp: writing new msg_serial=0x%08x to 0x%08x\n",
                   msg_serial,
                   DSP_IPC_MSG_SERIAL_GPP
                   );
         }

         IPC_REG_WRITE(DSP_IPC_MSG_SERIAL_GPP, msg_serial);
            
         if(NULL != _client)
         {
            /* Copy message into GPP<>DSP shared memory area and send to DSP */
            
            if(C64_DEBUG_LEVEL(20))
            {
               printk(KERN_DEBUG "c64::mlb::mlb_send_to_dsp: sending msg. client=0x%08x dspCompId=0x%08x dspCmdId=0x%08x arg1=0x%08x arg2=0x%08x\n", 
                      (sU32)_client,
                      _msg->req.fields.dsp_component_id,
                      _msg->req.fields.dsp_cmd_id,
                      _msg->data[0].arg,
                      _msg->data[1].arg
                      );
            }
#if 0
            if(C64_DEBUG_LEVEL(20))
            {
               printk(KERN_DEBUG "c64::mlb::mlb_send_to_dsp: writing msg serial 0x%08x\n", msg_serial);
            }
#endif
            
           
            IPC_REG_WRITE(DSP_IPC_MSG_COMP_GPP, _msg->req.comp);
            
            IPC_REG_WRITE(DSP_IPC_MSG_ARG1_GPP, _msg->data[0].arg);
            
            IPC_REG_WRITE(DSP_IPC_MSG_ARG2_GPP, _msg->data[1].arg);
         }
         else
         {
            if(C64_DEBUG_LEVEL(20))
            {
               printk(KERN_DEBUG "c64::mlb::mlb_send_to_dsp: sending testmsg=0x%08x\n", 
                      (sU32)_msg
                      );
            }
         }
         
#if 0
         flush_cache_all();
#endif
         
         /* Send message to DSP (triggers interrupt on DSP-side) */
         MLB_REG(MLB_MESSAGE_0) = (sU32) _msg;
      
         
         /* Succeeded */
         ret = 0;
      }
      else
      {
         /* FIFO is still full, must retry later */
         
         mlb_set_ready_to_send();
         
         ret = -EAGAIN;
      }
   }
   else
   {
      /* Failed: still waiting for reply from DSP */
      
      ret = -EAGAIN;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- mlb_is_ready_to_send() */
sBool mlb_is_ready_to_send(void) {

   return (sBool) atomic_read(&at_b_ready_to_send);
}


/*--------------------------------------------------------------------------- mlb_set_ready_to_send() */
void mlb_set_ready_to_send(void) {
   atomic_set(&at_b_ready_to_send, S_TRUE);
}


/*--------------------------------------------------------------------------- mlb_debug_read() */
sU32 mlb_debug_read(void) {
   sU32 ret;

   ret = IPC_REG_READ(DSP_IPC_DEBUG_INTERNAL_GPP);

   if(C64_DEBUG_LEVEL(10))
   {
      printk(KERN_DEBUG "c64::mlb_debug_read: ret=0x%08x\n", ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- mlb_debug_usr_read() */
sU32 mlb_debug_usr_read (sU32 _idx) {
   sU32 ret;

   ret = IPC_REG_READ(DSP_IPC_DEBUG_USR0_GPP + (4u * (_idx & 3u)));

   if(C64_DEBUG_LEVEL(10))
   {
      printk(KERN_DEBUG "c64::mlb_debug_usr_read: idx=%u ret=0x%08x\n", _idx, ret);
   }

   return ret;
}


/*--------------------------------------------------------------------------- mlb_bootflag_set() */
void mlb_bootflag_set(sU32 _mode) {
   IPC_REG_WRITE(DSP_IPC_BOOTMODE_GPP, _mode);
}


/*--------------------------------------------------------------------------- mlb_bootflag_get() */
sU32 mlb_bootflag_get(void) {
   return IPC_REG_READ(DSP_IPC_BOOTMODE_GPP);
}


/*--------------------------------------------------------------------------- mlb_dsp_errno() */
sU32 mlb_dsp_errno(void) {
   return IPC_REG_READ(DSP_IPC_ERRNO_GPP);
}


/*--------------------------------------------------------------------------- mlb_init() */
sBool mlb_init(void) {
   sBool ret;
   sU32 coreCmIdleSt;

   atomic_set(&at_b_ready_to_send, S_FALSE);

   /* Power up mailbox registers */
   reg32_bit_set(CORE_CM_ICLKEN1, 7);  /* set EN_MAILBOXES bit */

   /* Wait for mailbox */
   {
      sUI i=0;

      for(i=0; i<1; i++)
      {
         udelay(1000);
      }
   }

   /* First msg serial sent to DSP will be 1 (inc'd in mlb_send_to_dsp()) */
   msg_serial = 0;

   /* Check whether mailbox regs can now be accessed */
   coreCmIdleSt = reg32_read(CORE_CM_IDLEST1);

   if(C64_DEBUG_LEVEL(20))
   {
      printk(KERN_DEBUG "c64::mlb_init: CM_IDLEST1_CORE=0x%08x\n",
             coreCmIdleSt
             );
   }

   /* Check ST_MAILBOXES bit */
   if(0 == (coreCmIdleSt & 0x80u))
   {
      /* Map the entire L1DSRAM */
      ipc_base_virt =
         (sU8 __iomem *)
         ioremap_nocache(DSP_IPC_BASE_GPP,
                         IVA2_L1DSRAM_SIZE /* DSP_IPC_SIZE */
                         );

      if(NULL != ipc_base_virt)
      {
         mlb_base_virt = (sU8 __iomem *) ioremap_nocache(MLB_BASE, 4096);
         
         if(NULL != mlb_base_virt)
         {
            {
               sU32 mlbRev;
               
               mlbRev = MLB_REG(MLB_REVISION);

               if(C64_DEBUG_LEVEL(10))
               {
                  printk(KERN_DEBUG "c64::mlb_init: mlb revision=0x%08x\n", mlbRev); 
               }
               
               /* Disable mailbox interrupts and reset mailbox */
               loc_mlb_reset();
               
               
               /* Install interrupt handler for GPP mailbox interrupt */
               {
                  int r;
                  
                  r = request_irq(MLB_U0_MPU_IRQ,
                                  &irq_handler__mlb,
                                  0,                  /* irq flags */
                                  "c64-mlb",          /* dev-name (for /proc/interrupts) */
                                  NULL                /* dev-id (ptr. to priv. data) */
                                  );
                  
                  if(0 == r)
                  {
                     /* Enable mailbox interrupts */
                     loc_mlb_reset_2();
                     
                     /* Succeeded */
                     ret = S_TRUE;
                  }
                  else
                  {
                     /* Failed: request_irq() */
                     printk(KERN_ERR "c64::mlb_init: request_irq() failed.\n");
                     
                     iounmap(mlb_base_virt);
                     mlb_base_virt = NULL;
                    
                     iounmap(ipc_base_virt);
                     ipc_base_virt = NULL;
                     
                     ret = S_FALSE;
                  }
               }
            }
         }
         else
         {
            /* Failed: ioremap_nocache() [mlb] */
            printk(KERN_ERR "c64::mlb_init: ioremap_nocache() [mlb] failed.\n");
            
            iounmap(ipc_base_virt);
            ipc_base_virt = NULL;
            
            ret = S_FALSE;
         }
      }
      else
      {
         /* Failed: ioremap_nocache() [msg] */
         printk(KERN_ERR "c64::mlb_init: ioremap_nocache() [msg] failed.\n");
         
         ret = S_FALSE;
      }
   }
   else
   {
      /* Failed to enable access to mailbox registers */
      printk(KERN_ERR "c64::mlb_init: mailbox regs cannot be accessed.\n");

      ret = S_FALSE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- mlb_exit() */
void mlb_fastcall_end(void) {
   sU32 fcCtl;

   /* Wait until DSP has acknowledged last fastcall */
   fcCtl = IPC_REG_READ(DSP_IPC_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF + DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);

   if(DSP_FSHM_FASTCALL_CTL_DONE != fcCtl) /* fastcall in progress ? */
   {
      sU32 timeout = 0;

      printk(KERN_WARNING "c64::dev::release: force-terminating fastcall sequence (fcCtl=0x%08x)\n",
             fcCtl
             );

      /* Waiting for ack? ? */
      if(DSP_FSHM_FASTCALL_CTL_ACK == fcCtl)
      {
         IPC_REG_WRITE(DSP_IPC_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF + DSP_FSHM_MSG_TODSP_CTL_GPP_OFF,
                       DSP_FSHM_FASTCALL_CTL_IDLE
                       );
      }


      while((DSP_FSHM_FASTCALL_CTL_IDLE != fcCtl) &&
            (timeout < DSP_FASTCALL_TIMEOUT)
            )
         
      {
         fcCtl = IPC_REG_READ(DSP_IPC_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF + DSP_FSHM_MSG_TOGPP_CTL_GPP_OFF);
         timeout++;
         
         asm("\n\t nop"
             "\n\t nop"
             "\n\t nop"
             "\n\t nop"
             "\n\t nop"
             "\n\t nop"
             "\n\t nop"
             "\n\t nop"
             );

      }
      
      if(DSP_FASTCALL_TIMEOUT == timeout)
      {
         printk(KERN_ERR "c64::mlb_fastcall_end: timeout while waiting for DSP to ack. last request. fcCtl=0x%08x\n",
                fcCtl
                );
      }
      
      IPC_REG_WRITE(DSP_IPC_BASE_GPP + DSP_L1SRAM_FSHM_BASE_OFF + DSP_FSHM_MSG_TODSP_CTL_GPP_OFF,
                    DSP_FSHM_FASTCALL_CTL_DONE
                    );
   }
}


/*--------------------------------------------------------------------------- mlb_exit() */
void mlb_exit(void) {

   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::mlb_exit: ENTER  (%u irqs handled so far)\n", mlb_irq_count);
   }

   atomic_set(&at_b_ready_to_send, S_FALSE);

   /* Disable GPP mailbox interrupts */
   MLB_REG(MLB_IRQENABLE_0) = 0;

   /* Disable DSP mailbox interrupts */
   MLB_REG(MLB_IRQENABLE_1) = 0;

   /* Uninstall interrupt handler */
   free_irq(MLB_U0_MPU_IRQ,
            NULL             /* dev-id (ptr. to priv. data) */
            );

   /* Unmap mailbox registers */
   iounmap(mlb_base_virt);
   mlb_base_virt = NULL;

   /* Unmap DSP<>GPP shared memory area */
   iounmap(ipc_base_virt);
   ipc_base_virt = NULL;

   if(C64_DEBUG_LEVEL(5))
   {
      printk(KERN_DEBUG "c64::mlb_exit: LEAVE\n");
   }
}
