/* ----
 * ---- file   : reg.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 15Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_REG_H__
#define __C64_TOOLS_KMOD_REG_H__


/*
 * Write to 32bit register
 *
 */
void reg32_write (sU32 _physAddr, sU32 _val);


/*
 * Read from 32bit register
 *
 */
sU32 reg32_read (sU32 _physAddr);


/*
 * Clear bit in 32bit register
 *
 */
void reg32_bit_clear (sU32 _physAddr, sU32 _bit);


/*
 * Set bit in 32bit register
 *
 */
void reg32_bit_set (sU32 _physAddr, sU32 _bit);


#endif /* __C64_TOOLS_KMOD_REG_H__ */
