/* ----
 * ---- file   : dm3730.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013-2014 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Register defines for OMAP3530/DM3730 platform. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 28Sep2013, 13Oct2013, 20Oct2013, 25Oct2013
 * ----          16Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_DM3730_H__
#define __C64_TOOLS_DM3730_H__

#include "../../../include/cplusplus_begin.h"


/* (note) register addresses are for OMAP3530 / DM3730 */

#define DSP_REG(a) (*(volatile sU32*)(a))


/*--------------------------------------------------------------------------- IC */
#define IVA2_IC_BASE        (0x01800000u)

#define IVA2_IC_EVTFLAG_0   (IVA2_IC_BASE + 0x00000000u)  // R/32   Event Flag Registers
#define IVA2_IC_EVTFLAG_1   (IVA2_IC_BASE + 0x00000004u)  // R/32
#define IVA2_IC_EVTFLAG_2   (IVA2_IC_BASE + 0x00000008u)  // R/32
#define IVA2_IC_EVTFLAG_3   (IVA2_IC_BASE + 0x0000000Cu)  // R/32

#define IVA2_IC_EVTSET_0    (IVA2_IC_BASE + 0x00000020u)  // W/32   Event Set Registers
#define IVA2_IC_EVTSET_1    (IVA2_IC_BASE + 0x00000024u)  // W/32
#define IVA2_IC_EVTSET_2    (IVA2_IC_BASE + 0x00000028u)  // W/32
#define IVA2_IC_EVTSET_3    (IVA2_IC_BASE + 0x0000002Cu)  // W/32

#define IVA2_IC_EVTCLR_0    (IVA2_IC_BASE + 0x00000040u)  // W/32   Event Clear Registers
#define IVA2_IC_EVTCLR_1    (IVA2_IC_BASE + 0x00000044u)  // W/32
#define IVA2_IC_EVTCLR_2    (IVA2_IC_BASE + 0x00000048u)  // W/32
#define IVA2_IC_EVTCLR_3    (IVA2_IC_BASE + 0x0000004Cu)  // W/32

#define IVA2_IC_EVTMASK_0   (IVA2_IC_BASE + 0x00000080u)  // RW/32  Event Mask Registers
#define IVA2_IC_EVTMASK_1   (IVA2_IC_BASE + 0x00000084u)  // RW/32
#define IVA2_IC_EVTMASK_2   (IVA2_IC_BASE + 0x00000088u)  // RW/32
#define IVA2_IC_EVTMASK_3   (IVA2_IC_BASE + 0x0000008Cu)  // RW/32

#define IVA2_IC_MEVTFLAG_0  (IVA2_IC_BASE + 0x000000A0u)  // R/32   Masked Event Flag Registers
#define IVA2_IC_MEVTFLAG_1  (IVA2_IC_BASE + 0x000000A4u)  // R/32
#define IVA2_IC_MEVTFLAG_2  (IVA2_IC_BASE + 0x000000A8u)  // R/32
#define IVA2_IC_MEVTFLAG_3  (IVA2_IC_BASE + 0x000000ACu)  // R/32

#define IVA2_IC_EXPMASK_0   (IVA2_IC_BASE + 0x000000C0u)  // RW/32  Exception Mask Registers
#define IVA2_IC_EXPMASK_1   (IVA2_IC_BASE + 0x000000C4u)  // RW/32
#define IVA2_IC_EXPMASK_2   (IVA2_IC_BASE + 0x000000C8u)  // RW/32
#define IVA2_IC_EXPMASK_3   (IVA2_IC_BASE + 0x000000CCu)  // RW/32

#define IVA2_IC_MEXPFLAG_0  (IVA2_IC_BASE + 0x000000E0u)  // R/32   Masked Exception Flag Registers
#define IVA2_IC_MEXPFLAG_1  (IVA2_IC_BASE + 0x000000E4u)  // R/32
#define IVA2_IC_MEXPFLAG_2  (IVA2_IC_BASE + 0x000000E8u)  // R/32
#define IVA2_IC_MEXPFLAG_3  (IVA2_IC_BASE + 0x000000ECu)  // R/32

#define IVA2_IC_INTMUX_1    (IVA2_IC_BASE + 0x00000104u)  // RW/32  Interrupt Mux Register
#define IVA2_IC_INTMUX_2    (IVA2_IC_BASE + 0x00000108u)  // RW/32
#define IVA2_IC_INTMUX_3    (IVA2_IC_BASE + 0x0000010Cu)  // RW/32

#define IVA2_IC_INTXSTAT    (IVA2_IC_BASE + 0x00000180u)  // R/32   Interrupt Exception Status Register
#define IVA2_IC_INTXCLR     (IVA2_IC_BASE + 0x00000184u)  // W/32   Interrupt Exception Clear Register
#define IVA2_IC_INTDMASK    (IVA2_IC_BASE + 0x00000188u)  // RW/32  Dropped Interrupt Mask Register

#define IVA2_IC_EVTASRT     (IVA2_IC_BASE + 0x000001C0u)  // W/32   Event Assert Register


/* IVA2 interrupt event sources */
#define IVA2_EVT_MAIL_U1_IRQ        ( 55u)  // Mailbox (IVA2_IRQ[10])
#define IVA2_EVT_DSS_IRQ            ( 58u)  // Display subsystem module (IVA2_IRQ[13])
#define IVA2_EVT_L3_IVA2_ERROR_IRQ  ( 84u)  // L3 interconnect out-of-band error interrupt
#define IVA2_EVT_INTERR             ( 96u)  // Dropped CPU interrupt event
#define IVA2_EVT_PDC_INT            (118u)  // PDC sleep interrupt
#define IVA2_EVT_SYS_CMPA           (119u)  // SYS CPU memory protection fault
#define IVA2_EVT_PMC_CMPA           (120u)  // CPU memory protection fault
#define IVA2_EVT_PMC_DMPA           (121u)  // DMA memory protection fault
#define IVA2_EVT_DMC_CMPA           (122u)  // CPU memory protection fault
#define IVA2_EVT_DMC_DMPA           (123u)  // DMA memory protection fault
#define IVA2_EVT_UMC_CMPA           (124u)  // CPU memory protection fault
#define IVA2_EVT_UMC_DMPA           (125u)  // DMA memory protection fault
#define IVA2_EVT_EMC_CMPA           (126u)  // CPU memory protection fault
#define IVA2_EVT_EMC_BUSERR         (127u)  // BUSERR interrupt
// (todo) add missing event ids


/* IVA2 interrupts */
#define IVA2_INT_4   ( 4u)
#define IVA2_INT_5   ( 5u)
#define IVA2_INT_6   ( 6u)
#define IVA2_INT_7   ( 7u)
#define IVA2_INT_8   ( 8u)
#define IVA2_INT_9   ( 9u)
#define IVA2_INT_10  (10u)
#define IVA2_INT_11  (11u)
#define IVA2_INT_12  (12u)
#define IVA2_INT_13  (13u)
#define IVA2_INT_14  (14u)
#define IVA2_INT_15  (15u)


/*--------------------------------------------------------------------------- WUGEN */
#define IVA2_WUGEN_BASE  (0x01c21000u)

#define IVA2_WUGEN_REVISION     (IVA2_WUGEN_BASE + 0x00000000u)  // R/32
#define IVA2_WUGEN_SYSCONFIG    (IVA2_WUGEN_BASE + 0x00000008u)  // RW/32
#define IVA2_WUGEN_MEVT0        (IVA2_WUGEN_BASE + 0x00000060u)  // R/32
#define IVA2_WUGEN_MEVT1        (IVA2_WUGEN_BASE + 0x00000064u)  // R/32
#define IVA2_WUGEN_MEVT2        (IVA2_WUGEN_BASE + 0x00000068u)  // R/32
#define IVA2_WUGEN_MEVTCLR0     (IVA2_WUGEN_BASE + 0x00000070u)  // W/32
#define IVA2_WUGEN_MEVTCLR1     (IVA2_WUGEN_BASE + 0x00000074u)  // W/32
#define IVA2_WUGEN_MEVTCLR2     (IVA2_WUGEN_BASE + 0x00000078u)  // W/32
#define IVA2_WUGEN_MEVTSET0     (IVA2_WUGEN_BASE + 0x00000080u)  // W/32
#define IVA2_WUGEN_MEVTSET1     (IVA2_WUGEN_BASE + 0x00000084u)  // W/32
#define IVA2_WUGEN_MEVTSET2     (IVA2_WUGEN_BASE + 0x00000088u)  // W/32
#define IVA2_WUGEN_PENDEVT0     (IVA2_WUGEN_BASE + 0x00000090u)  // R/32
#define IVA2_WUGEN_PENDEVT1     (IVA2_WUGEN_BASE + 0x00000094u)  // R/32
#define IVA2_WUGEN_PENDEVT2     (IVA2_WUGEN_BASE + 0x00000098u)  // R/32
#define IVA2_WUGEN_PENDEVTCLR0  (IVA2_WUGEN_BASE + 0x00000100u)  // W/32
#define IVA2_WUGEN_PENDEVTCLR1  (IVA2_WUGEN_BASE + 0x00000104u)  // W/32
#define IVA2_WUGEN_PENDEVTCLR2  (IVA2_WUGEN_BASE + 0x00000108u)  // W/32


/*--------------------------------------------------------------------------- SCM_INTERFACE */
#define IVA2_SCM_INTERFACE_BASE       (0x48002000u)

/* Control module revision number */
#define IVA2_SCM_INTERFACE_REVISION   (IVA2_SCM_INTERFACE_BASE + 0x00000000u)  // R/32

/* Set various parameters related to the Idle mode of the Control module */
#define IVA2_SCM_INTERFACE_SYSCONFIG  (IVA2_SCM_INTERFACE_BASE + 0x00000010u)  // RW/32


/*--------------------------------------------------------------------------- SCM_GENERAL */
#define IVA2_SCM_GENERAL_BASE      (0x48002270u)


/* Defines the physical address of the IVA2 boot loader.
 *  This is an index to a 4kByte page.
 */
#define IVA2_SCM_GENERAL_BOOTADDR  (IVA2_SCM_GENERAL_BASE + 0x00000190u)  // RW/32

// Defines the IVA2 bootmode
//  0x0: Direct boot: The ROM loader is not executed. Instead, IVA2.2 directly starts executing
//                     the bootstrap at the address contained in the BOOTADDR register.
//  0x1: Idle boot: The boot loader executes the IDLE instruction.
//  0x2: Wait in self-loop boot: THe boot loader puts IVA2.2 in a self-loop.
//  0x3: User-defined bootstrap mode: The boot loader copies the boot strap into internal memory
//                                     and branches to it.
//  0x4: The boot loader executes the default context restore code, which is part of the ROM boot loader.
#define IVA2_SCM_GENERAL_BOOTMOD   (IVA2_SCM_GENERAL_BASE + 0x00000194u)  // RW/32


#define IVA2_SCM_GENERAL_WKUP_BASE  (0x48002A5Cu)


/*--------------------------------------------------------------------------- CM */
#define IVA2_CM_BASE          (0x48004000u)  /* Module region A / Clock manager base address */

#define IVA2_CM_FCLKEN        (IVA2_CM_BASE + 0x00000000u)  // W/RW/32
#define IVA2_CM_CLKEN_PLL     (IVA2_CM_BASE + 0x00000004u)  // W/RW/32
#define IVA2_CM_ICLKEN_PLL    (IVA2_CM_BASE + 0x00000010u)  // <missing in sprugn4r.pdf>
#define IVA2_CM_IDLEST        (IVA2_CM_BASE + 0x00000020u)  // C/R/32
#define IVA2_CM_IDLEST_PLL    (IVA2_CM_BASE + 0x00000024u)  // C/R/32
#define IVA2_CM_AUTOIDLE_PLL  (IVA2_CM_BASE + 0x00000034u)  // W/RW/32
// Provides controls over the IVA2 DPLL
#define IVA2_CM_CLKSEL1_PLL   (IVA2_CM_BASE + 0x00000040u)  // W/RW/32
#define IVA2_CM_CLKSEL2_PLL   (IVA2_CM_BASE + 0x00000044u)  // W/RW/32

// Controls the clock state transition of the IVA2 clock domain (see p.452/Table 3-108.CM_CLKSTCTRL)
#define IVA2_CM_CLKSTCTRL     (IVA2_CM_BASE + 0x00000048u)  // W/RW/32
#define IVA2_CM_CLKSTST       (IVA2_CM_BASE + 0x0000004Cu)  // C/R/32


/*--------------------------------------------------------------------------- CORE_CM */
#define CORE_CM_BASE  (0x48004A00u)

#define CORE_CM_FCLKEN1  (CORE_CM_BASE + 0x00000000u)  // RW/32
#define CORE_CM_FCLKEN3  (CORE_CM_BASE + 0x00000008u)  // RW/32
#define CORE_CM_ICLKEN1  (CORE_CM_BASE + 0x00000010u)  // RW/32
#define CORE_CM_ICLKEN3  (CORE_CM_BASE + 0x00000018u)  // RW/32
#define CORE_CM_IDLEST1  (CORE_CM_BASE + 0x00000020u)  // R/32
// (todo) add remaining regs

#define CORE_CM_IDLEST1_BIT_ST_MAILBOXES  (7u)


/*--------------------------------------------------------------------------- PER_CM */
#define IVA2_PER_CM_BASE       (0x48005000u)

#define IVA2_PER_CM_FCLKEN     (IVA2_PER_CM_BASE + 0x00000000u)  // W/RW/32
#define IVA2_PER_CM_ICLKEN     (IVA2_PER_CM_BASE + 0x00000010u)  // W/RW/32
#define IVA2_PER_CM_CLKSEL     (IVA2_PER_CM_BASE + 0x00000040u)  // W/RW/32
#define IVA2_PER_CM_CLKSTCTRL  (IVA2_PER_CM_BASE + 0x00000048u)  // W/RW/32
// (todo) add remaining PER_CRM regs


/*--------------------------------------------------------------------------- PRM */
#define IVA2_PRM_BASE       (0x48306000u)  // PRM base address

// Controls the release of the IVA sub-system resets
#define IVA2_PRM_RSTCTRL    (IVA2_PRM_BASE + 0x00000050u)  // C/RW/32

// Logs the different reset sources of the IVA2 domain. 
//  Each bit is set upon release of the domain reset signal.
//  Must be cleared by software.
#define IVA2_PRM_RSTST      (IVA2_PRM_BASE + 0x00000058u)  // C/RW/32

#define IVA2_PRM_WKDEP      (IVA2_PRM_BASE + 0x000000C8u)  // W/RW/32
#define IVA2_PRM_PWSTCTRL   (IVA2_PRM_BASE + 0x000000E0u)  // W/RW/32
#define IVA2_PRM_PWSTST     (IVA2_PRM_BASE + 0x000000E4u)  // C/R/32
#define IVA2_PRM_PREPWSTST  (IVA2_PRM_BASE + 0x000000E8u)  // C/RW/32
#define IVA2_PRM_IRQSTATUS  (IVA2_PRM_BASE + 0x000000F8u)  // W/RW/32
#define IVA2_PRM_IRQENABLE  (IVA2_PRM_BASE + 0x000000FCu)  // W/RW/32


/*--------------------------------------------------------------------------- Mailbox */
#define MLB_BASE  (0x48094000u)  // 14.5.1.

#define MLB_REVISION      (MLB_BASE + 0x00000000u)  // R/32
#define MLB_SYSCONFIG     (MLB_BASE + 0x00000010u)  // RW/32
#define MLB_SYSSTATUS     (MLB_BASE + 0x00000014u)  // R/32
#define MLB_MESSAGE_0     (MLB_BASE + 0x00000040u)  // RW/32  (mailbox 0)
#define MLB_MESSAGE_1     (MLB_BASE + 0x00000044u)  // RW/32  (mailbox 1)
#define MLB_FIFOSTATUS_0  (MLB_BASE + 0x00000080u)  // R/32   (mailbox 0)
#define MLB_FIFOSTATUS_1  (MLB_BASE + 0x00000084u)  // R/32   (mailbox 1)
#define MLB_MSGSTATUS_0   (MLB_BASE + 0x000000C0u)  // R/32   (mailbox 0)
#define MLB_MSGSTATUS_1   (MLB_BASE + 0x000000C4u)  // R/32   (mailbox 1)
#define MLB_IRQSTATUS_0   (MLB_BASE + 0x00000100u)  // RW/32  (GPP)
#define MLB_IRQENABLE_0   (MLB_BASE + 0x00000104u)  // RW/32  (GPP)
#define MLB_IRQSTATUS_1   (MLB_BASE + 0x00000108u)  // RW/32  (DSP)
#define MLB_IRQENABLE_1   (MLB_BASE + 0x0000010Cu)  // RW/32  (DSP)


#define MLB_U0_MPU_IRQ  (26u)  // GPP-side Mailbox user 0 request interrupt

#define MLB_FIFO_SIZE  (4u)


/*--------------------------------------------------------------------------- MMU1 */
#define IVA2_MMU1_BASE  (0x480BD400u) // Camera MMU


/*--------------------------------------------------------------------------- MMU2 */
#define IVA2_MMU2_BASE  (0x5D000000u) // Base address of IVA2.2 MMU (as seen by GPP)

#define IVA2_MMU2_REVISION   (IVA2_MMU2_BASE + 0x00000000u)  // R/32
#define IVA2_MMU2_SYSCONFIG  (IVA2_MMU2_BASE + 0x00000010u)  // RW/32
#define IVA2_MMU2_SYSSTATUS  (IVA2_MMU2_BASE + 0x00000014u)  // R/32
// This register programs the MMU2 features
#define IVA2_MMU2_CNTL       (IVA2_MMU2_BASE + 0x00000044u)


/*--------------------------------------------------------------------------- CCR_CM */
#define CCR_CM_BASE  (0x48004d00u)  // Base address of Clock Control Registers (3.7.1.8)

#define CCR_CM_CLKEN_PLL   (CCR_CM_BASE + 0x00000000u)  // RW/32
#define CCR_CM_CLKEN2_PLL  (CCR_CM_BASE + 0x00000004u)  // RW/32


/*--------------------------------------------------------------------------- L1DSRAM */
#define IVA2_L1DSRAM_BASE_GPP  (0x5cf04000u)

#define IVA2_L1DSRAM_BASE_DSP  (0x10f04000u)

#define IVA2_L1DSRAM_SIZE      (0x0000C000u)  /* 48 kbytes */


/* cache (just for completeness' sake) */
#define IVA2_L1DCSRAM_BASE_DSP  (0x10f10000u)

#define IVA2_L1DCSRAM_SIZE      (0x00004000u)  /* 16 kbytes */


/*--------------------------------------------------------------------------- L2SRAM */
#define IVA2_L2SRAM_BASE_GPP  (0x5c7f8000u)

#define IVA2_L2SRAM_BASE_DSP  (0x107f8000u)

#define IVA2_L2SRAM_SIZE      (0x00018000u)   /* 96 kbytes */


/*--------------------------------------------------------------------------- IVA2 SYS */
#define IVA2_SYS_BASE  (0x01810000u)

#define IVA2_SYS_PDCCMD  (IVA2_SYS_BASE + 0x00000000u)


/*--------------------------------------------------------------------------- IVA2 XMC */
#define IVA2_XMC_BASE  (0x01840000u)

#define IVA2_XMC_L2CFG    (IVA2_XMC_BASE + 0x00000000u)
#define IVA2_XMC_L2MPPA0  (IVA2_XMC_BASE + 0x0000a200u)
#define IVA2_XMC_L2MPPA1  (IVA2_XMC_BASE + 0x0000a204u)


/*--------------------------------------------------------------------------- IVA2 TPCC */
#define IVA2_TPCC_BASE  (0x01C00000u)

#define IVA2_TPCC_PID       (IVA2_TPCC_BASE + 0x00000000u)  // R/32
#define IVA2_TPCC_CCCFG     (IVA2_TPCC_BASE + 0x00000004u)  // R/32
#define IVA2_TPCC_DHCMAP0   (IVA2_TPCC_BASE + 0x00000100u)  // RW/32  (64 registers, +4, +8, ..)
#define IVA2_TPCC_QCHMAP0   (IVA2_TPCC_BASE + 0x00000200u)  // RW/32  (8 registers)
#define IVA2_TPCC_DMAQNUM0  (IVA2_TPCC_BASE + 0x00000240u)  // RW/32  (8 registers)
#define IVA2_TPCC_QDMAQNUM  (IVA2_TPCC_BASE + 0x00000260u)  // RW/32
#define IVA2_TPCC_QUETCMAP  (IVA2_TPCC_BASE + 0x00000280u)  // RW/32
#define IVA2_TPCC_QUEPRI    (IVA2_TPCC_BASE + 0x00000284u)  // RW/32
#define IVA2_TPCC_EMR       (IVA2_TPCC_BASE + 0x00000300u)  // R/32
#define IVA2_TPCC_EMRH      (IVA2_TPCC_BASE + 0x00000304u)  // R/32
#define IVA2_TPCC_EMCR      (IVA2_TPCC_BASE + 0x00000308u)  // W/32
#define IVA2_TPCC_EMCRH     (IVA2_TPCC_BASE + 0x0000030Cu)  // W/32
#define IVA2_TPCC_QEMR      (IVA2_TPCC_BASE + 0x00000310u)  // R/32
#define IVA2_TPCC_QEMCR     (IVA2_TPCC_BASE + 0x00000314u)  // W/32
#define IVA2_TPCC_CCERR     (IVA2_TPCC_BASE + 0x00000318u)  // R/32
#define IVA2_TPCC_CCERRCLR  (IVA2_TPCC_BASE + 0x0000031Cu)  // W/32
#define IVA2_TPCC_EEVAL     (IVA2_TPCC_BASE + 0x00000320u)  // W/32
#define IVA2_TPCC_DRAE0     (IVA2_TPCC_BASE + 0x00000340u)  // RW/32  (8 registers, +8, +16, ..)
#define IVA2_TPCC_DRAEH0    (IVA2_TPCC_BASE + 0x00000344u)  // RW/32  (8 registers, +8, +16, ..)
#define IVA2_TPCC_QRAE0     (IVA2_TPCC_BASE + 0x00000380u)  // RW/32  (8 registers, +4, +8, ..)
#define IVA2_TPCC_Q0E0      (IVA2_TPCC_BASE + 0x00000400u)  // R/32  (16 registers, +4, +8, ..)
#define IVA2_TPCC_Q1E0      (IVA2_TPCC_BASE + 0x00000440u)  // R/32  (16 registers, +4, +8, ..)
#define IVA2_TPCC_QSTAT0    (IVA2_TPCC_BASE + 0x00000600u)  // R/32  (2 registers, +4, +8, ..)
#define IVA2_TPCC_QWMTHRA   (IVA2_TPCC_BASE + 0x00000620u)  // RW/32
#define IVA2_TPCC_QWMTHRB   (IVA2_TPCC_BASE + 0x00000624u)  // RW/32
#define IVA2_TPCC_CCSTAT    (IVA2_TPCC_BASE + 0x00000640u)  // R/32
#define IVA2_TPCC_MPFAR     (IVA2_TPCC_BASE + 0x00000800u)  // R/32
#define IVA2_TPCC_MPFSR     (IVA2_TPCC_BASE + 0x00000804u)  // R/32
#define IVA2_TPCC_MPFCR     (IVA2_TPCC_BASE + 0x00000808u)  // W/32
#define IVA2_TPCC_MPPAG     (IVA2_TPCC_BASE + 0x0000080Cu)  // RW/32
#define IVA2_TPCC_MPPA0     (IVA2_TPCC_BASE + 0x00000810u)  // RW/32  (8 registers, +4, +8, ..)
#define IVA2_TPCC_ER        (IVA2_TPCC_BASE + 0x00001000u)  // R/32
#define IVA2_TPCC_ECR       (IVA2_TPCC_BASE + 0x00001008u)  // W/32
#define IVA2_TPCC_ECRH      (IVA2_TPCC_BASE + 0x0000100Cu)  // W/32
#define IVA2_TPCC_ESR       (IVA2_TPCC_BASE + 0x00001010u)  // W/32
#define IVA2_TPCC_ESRH      (IVA2_TPCC_BASE + 0x00001014u)  // W/32
#define IVA2_TPCC_CER       (IVA2_TPCC_BASE + 0x00001018u)  // R/32
#define IVA2_TPCC_CERH      (IVA2_TPCC_BASE + 0x0000101Cu)  // R/32
#define IVA2_TPCC_EER       (IVA2_TPCC_BASE + 0x00001020u)  // R/32
#define IVA2_TPCC_EECR      (IVA2_TPCC_BASE + 0x00001028u)  // W/32
#define IVA2_TPCC_EESR      (IVA2_TPCC_BASE + 0x00001030u)  // W/32
#define IVA2_TPCC_SER       (IVA2_TPCC_BASE + 0x00001038u)  // R/32
#define IVA2_TPCC_SERH      (IVA2_TPCC_BASE + 0x0000103Cu)  // R/32
#define IVA2_TPCC_SECR      (IVA2_TPCC_BASE + 0x00001040u)  // W/32
#define IVA2_TPCC_SECRH     (IVA2_TPCC_BASE + 0x00001044u)  // W/32
#define IVA2_TPCC_IER       (IVA2_TPCC_BASE + 0x00001050u)  // R/32
#define IVA2_TPCC_IERH      (IVA2_TPCC_BASE + 0x00001054u)  // R/32
#define IVA2_TPCC_IECR      (IVA2_TPCC_BASE + 0x00001058u)  // W/32
#define IVA2_TPCC_IECRH     (IVA2_TPCC_BASE + 0x0000105Cu)  // W/32
#define IVA2_TPCC_IESR      (IVA2_TPCC_BASE + 0x00001060u)  // W/32
#define IVA2_TPCC_IESRH     (IVA2_TPCC_BASE + 0x00001064u)  // W/32
#define IVA2_TPCC_IPR       (IVA2_TPCC_BASE + 0x00001068u)  // R/32
#define IVA2_TPCC_IPRH      (IVA2_TPCC_BASE + 0x0000106Cu)  // R/32
#define IVA2_TPCC_ICR       (IVA2_TPCC_BASE + 0x00001070u)  // W/32
#define IVA2_TPCC_ICRH      (IVA2_TPCC_BASE + 0x00001074u)  // W/32
#define IVA2_TPCC_IEVAL     (IVA2_TPCC_BASE + 0x00001078u)  // W/32
#define IVA2_TPCC_QER       (IVA2_TPCC_BASE + 0x00001080u)  // R/32
#define IVA2_TPCC_QEER      (IVA2_TPCC_BASE + 0x00001084u)  // R/32
#define IVA2_TPCC_QEECR     (IVA2_TPCC_BASE + 0x00001088u)  // W/32
#define IVA2_TPCC_QEESR     (IVA2_TPCC_BASE + 0x0000108Cu)  // W/32
#define IVA2_TPCC_QSER      (IVA2_TPCC_BASE + 0x00001090u)  // R/32
#define IVA2_TPCC_QSECR     (IVA2_TPCC_BASE + 0x00001094u)  // W/32
#define IVA2_TPCC_ER_R0     (IVA2_TPCC_BASE + 0x00002000u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ECR_R0    (IVA2_TPCC_BASE + 0x00002008u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ECRH_R0   (IVA2_TPCC_BASE + 0x0000200Cu)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ESR_R0    (IVA2_TPCC_BASE + 0x00002010u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ESRH_R0   (IVA2_TPCC_BASE + 0x00002014u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_CER_R0    (IVA2_TPCC_BASE + 0x00002018u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_CERH_R0   (IVA2_TPCC_BASE + 0x0000201Cu)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_EER_R0    (IVA2_TPCC_BASE + 0x00002020u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_EECR_R0   (IVA2_TPCC_BASE + 0x00002028u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_EESR_R0   (IVA2_TPCC_BASE + 0x00002030u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_SER_R0    (IVA2_TPCC_BASE + 0x00002038u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_SERH_R0   (IVA2_TPCC_BASE + 0x0000203Cu)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_SECR_R0   (IVA2_TPCC_BASE + 0x00002040u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_SECRH_R0  (IVA2_TPCC_BASE + 0x00002044u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IER_R0    (IVA2_TPCC_BASE + 0x00002050u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IERH_R0   (IVA2_TPCC_BASE + 0x00002054u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IECR_R0   (IVA2_TPCC_BASE + 0x00002058u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IECRH_R0  (IVA2_TPCC_BASE + 0x0000205Cu)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IESR_R0   (IVA2_TPCC_BASE + 0x00002060u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IESRH_R0  (IVA2_TPCC_BASE + 0x00002064u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IPR_R0    (IVA2_TPCC_BASE + 0x00002068u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IPRH_R0   (IVA2_TPCC_BASE + 0x0000206Cu)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ICR_R0    (IVA2_TPCC_BASE + 0x00002070u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_ICRH_R0   (IVA2_TPCC_BASE + 0x00002074u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_IEVAL_R0  (IVA2_TPCC_BASE + 0x00002078u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QER_R0    (IVA2_TPCC_BASE + 0x00002080u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QEER_R0   (IVA2_TPCC_BASE + 0x00002084u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QEECR_R0  (IVA2_TPCC_BASE + 0x00002088u)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QEESR_R0  (IVA2_TPCC_BASE + 0x0000208Cu)  // W/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QSER_R0   (IVA2_TPCC_BASE + 0x00002090u)  // R/32  (8 registers, +0x200, +0x400, ..)
#define IVA2_TPCC_QSECR_R0  (IVA2_TPCC_BASE + 0x00002094u)  // W/32  (8 registers, +0x200, +0x400, ..)

#define IVA2_TPCC_PARAM_BASE  (IVA2_TPCC_BASE + 0x00004000u)  /* 32 bytes per PaRAM entry */

#define IVA2_TPCC_OPT0      (IVA2_TPCC_BASE + 0x00004000u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_SRC0      (IVA2_TPCC_BASE + 0x00004004u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_ABCNT0    (IVA2_TPCC_BASE + 0x00004008u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_DST0      (IVA2_TPCC_BASE + 0x0000400Cu)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_BIDX0     (IVA2_TPCC_BASE + 0x00004010u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_LNK0      (IVA2_TPCC_BASE + 0x00004014u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_CIDX0     (IVA2_TPCC_BASE + 0x00004018u)  // RW/32  (128 registers, +0x20, +0x40, ..)
#define IVA2_TPCC_CCNT0     (IVA2_TPCC_BASE + 0x0000401Cu)  // RW/32  (128 registers, +0x20, +0x40, ..)

#define IVA2_TPCC_PARAM_OPT    (0u)
#define IVA2_TPCC_PARAM_SRC    (1u)
#define IVA2_TPCC_PARAM_ABCNT  (2u)
#define IVA2_TPCC_PARAM_DST    (3u)
#define IVA2_TPCC_PARAM_BIDX   (4u)
#define IVA2_TPCC_PARAM_LNK    (5u)
#define IVA2_TPCC_PARAM_CIDX   (6u)
#define IVA2_TPCC_PARAM_CCNT   (7u)


/*--------------------------------------------------------------------------- IVA2 TPTC */
#define IVA2_TPTC0_BASE  (0x01C10000u)
#define IVA2_TPTC1_BASE  (0x01C10400u)

#define IVA2_TPTC0_RDRATE  (IVA2_TPTC0_BASE + 0x00000140u)
#define IVA2_TPTC1_RDRATE  (IVA2_TPTC1_BASE + 0x00000140u)


/*--------------------------------------------------------------------------- IVA2 video sequencer */
#define IVA2_SEQ_BASE_L3  (0x5e000000u)

#define IVA2_SEQ_SL2MEM      (0x00000000u)   // 32k L2SRAM?
#define IVA2_SEQ_EDMA3_BASE  (0x00020000u)
#define IVA2_SEQ_SEQCFG_BASE (0x00090000u)
#define IVA2_SEQ_DMEM        (0x00094000u)   // data memory (4k)
#define IVA2_SEQ_IMEM        (0x00098000u)   // instruction memory (8k)
#define IVA2_SEQ_VIDCFG_BASE (0x0009C000u)
#define IVA2_SEQ_IMECFG_BASE (0x000A0000u)   // motion estimation engine regs
#define IVA2_SEQ_ILFCFG_BASE (0x000A1000u)   // loop filter engine regs
#define IVA2_SEQ_SRC0_BASE   (0x000F8000u)   // videoseq interconnect cfg regs? (16k?)

#define IVA2_SEQ_IMEM_SIZE  (8 * 1024u)
#define IVA2_SEQ_DMEM_SIZE  (4 * 1024u)


#include "../../../include/cplusplus_end.h"

#endif // __C64_TOOLS_TYPES_H__
