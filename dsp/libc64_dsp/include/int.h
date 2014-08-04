/* ----
 * ---- file   : int.h
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
 * ---- info   : This is part of the "c64_load" utility.
 * ----
 * ---- changed: 12Sep2013
 * ----
 * ----
 */

#ifndef __INT_H__
#define __INT_H__


/*
 * Map event source to interrupt vector
 *
 */
void int_map_event_to_vector (sUI _eventId, sUI _intVectorNr);


/*
 * Enable event / interrupt source
 *
 */
void int_event_enable (sUI _eventId);


/*
 * Clear event bit
 *
 */
void int_event_clear (sUI _eventId);


/*
 * Enable interrupt
 *
 */
sU32 int_irq_enable (sUI _intNr);


/*
 * Disable interrupt
 *
 */
sU32 int_irq_disable (sUI _intNr);


/*
 * Clear interrupt request bit
 *
 */
void int_irq_clear (sUI _intNr);


#endif /* __INT_H__ */
