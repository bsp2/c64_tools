/* ----
 * ---- file   : int.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel.
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
 * ---- info   : C64+ interrupt controller utility functions. This is part of the "c64_load" utility.
 * ----
 * ---- changed: 12Sep2013, 14Sep2013
 * ----
 * ----
 */

////#include <std.h>
////#include <sys.h>
////#include <hwi.h>

#include "../../../include/types.h"
#include "../../../include/arch/dm3730/dm3730.h"

#include "../include/int.h"


/* --------------------------------------------------------------------------- int_map_event_to_vector() */
void int_map_event_to_vector(sUI _eventId, sUI _vectorNr) {
   sUI bitoff;
   sUI reg;

   /* (note) see p.796/797 of sprugn4r.pdf ("IC Registers") 
    *         and p.169 of spru871k.pdf ("Interrupt Controller Registers")
    */

   if(_vectorNr < 8)
   {
      /* Interrupt vectors 4..7 */
      bitoff = (_vectorNr - 4);
      reg    = IVA2_IC_INTMUX_1;

   }
   else if(_vectorNr < 12)
   {
      /* Interrupt vectors 8..11 */
      bitoff = (_vectorNr - 8);
      reg    = IVA2_IC_INTMUX_2;
   }
   else if(_vectorNr < 16)
   {
      /* Interrupt vectors 12..15 */
      bitoff = (_vectorNr - 12);
      reg    = IVA2_IC_INTMUX_3;
   }
   else
   {
      /* Failed: illegal vectorNr */
      reg = 0;
   }

   if(reg > 0)
   {
      /* Convert field nr. to actual bit offset (each entry uses one byte. the MSB is reserved, though) */
      bitoff = (bitoff << 8);

      /* Map eventId to vectorNr */
      DSP_REG(reg) = 
         (DSP_REG(reg) & ~(255u << bitoff)) |
         ((_eventId & 127u) << bitoff)      ;
   }

}


/* --------------------------------------------------------------------------- int_event_enable() */
void int_event_enable(sUI _eventId) {

   sUI reg   = IVA2_IC_EVTMASK_0 + ((_eventId >> (5 - 2)) & ~3u);
   sUI bitNr = (_eventId & 31u);

   DSP_REG(reg) = (DSP_REG(reg) & ~(1u << bitNr));
}


/* --------------------------------------------------------------------------- int_event_disable() */
void int_event_disable(sUI _eventId) {

   sUI reg   = IVA2_IC_EVTMASK_0 + ((_eventId >> (5 - 2)) & ~3u);
   sUI bitNr = (_eventId & 31u);

   DSP_REG(reg) |= (1u << bitNr);
}


/* --------------------------------------------------------------------------- int_event_clear() */
void int_event_clear(sUI _eventId) {

   sUI reg   = IVA2_IC_EVTCLR_0 + ((_eventId >> (5 - 2)) & ~3u);
   sUI bitNr = (_eventId & 31u);

   DSP_REG(reg) = (1u << bitNr);
}


/* --------------------------------------------------------------------------- int_irq_clear() */
//void int_irq_clear(sUI _intNr) {
//}
