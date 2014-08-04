/* ----
 * ---- file   : reg.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 14Sep2013, 15Sep2013, 16Sep2013
 * ----          15Dec2013
 * ----
 * ----
 */

#define __NO_VERSION__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include "../include/types.h"

#include "kmod.h"
#include "reg.h"


#define REG_TRACE defined


/*--------------------------------------------------------------------------- reg32_write() */
void reg32_write(sU32 _physAddr, sU32 _val) {
   void __iomem *virtAddr = ioremap_nocache(_physAddr, 4);

   if(NULL != virtAddr)
   {
#ifdef REG_TRACE
      if(C64_DEBUG_LEVEL(20))
      {
         printk(KERN_DEBUG "c64::reg32_write: physAddr=0x%08x virtAddr=%08x val=0x%08x\n",
                _physAddr,
                (sU32)virtAddr,
                _val
                );
      }
#endif /* REG_TRACE */

      *((sU32*)virtAddr) = _val;

      iounmap(virtAddr);
   }
  
}


/*--------------------------------------------------------------------------- reg32_read() */
sU32 reg32_read(sU32 _physAddr) {
   sU32 ret;
   void __iomem *virtAddr = ioremap_nocache(_physAddr, 4);

   if(NULL != virtAddr)
   {
      ret = *((sU32*)virtAddr);

#ifdef REG_TRACE
      if(C64_DEBUG_LEVEL(20))
      {
         printk(KERN_DEBUG "c64::reg32_read: physAddr=0x%08x val=0x%08x\n",
                _physAddr,
                ret
                );
      }
#endif /* REG_TRACE */

      iounmap(virtAddr);
   }
   else
   {
      ret = 0xCCccCCccu;
   }

   return ret;
}


/*--------------------------------------------------------------------------- reg32_bit_clear() */
void reg32_bit_clear(sU32 _physAddr, sU32 _bit) {

   void __iomem *virtAddr = ioremap_nocache(_physAddr, 4);

   if(NULL != virtAddr)
   {
      *((sU32*)virtAddr) &= ~(1 << _bit);

      iounmap(virtAddr);
   }
  
}


/*--------------------------------------------------------------------------- reg32_bit_set() */
void reg32_bit_set(sU32 _physAddr, sU32 _bit) {
   void __iomem *virtAddr = ioremap_nocache(_physAddr, 4);

   if(NULL != virtAddr)
   {
#ifdef REG_TRACE
      if(C64_DEBUG_LEVEL(20))
      {
         printk(KERN_DEBUG "c64::reg32_bit_set:   addr=0x%08x bit=%u oldVal=0x%08x\n",
                _physAddr,
                _bit,
                *((sU32*)virtAddr)
                );
      }
#endif /* REG_TRACE */
             
      
      *((sU32*)virtAddr) |= (1 << _bit);

#ifdef REG_TRACE
      if(C64_DEBUG_LEVEL(20))
      {
         printk(KERN_DEBUG "c64::reg32_bit_set: 2 addr=0x%08x bit=%u newVal=0x%08x\n",
                _physAddr,
                _bit,
                *((sU32*)virtAddr)
                );
      }
#endif /* REG_TRACE */

      iounmap(virtAddr);
   }
  
}
