;/*****************************************************************************
; *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
; *  
; *  Use of this software is controlled by the terms and conditions found in the
; *  license agreement under which this software has been supplied.             
; *****************************************************************************/

;/** @file _csl_intcIntrEnDisRes.asm  (original file)
; *
; *  @date 12th June, 2004
; *  @author Ruchika Kharwar
; */

    .global _int_irq_enable
_int_irq_enable:
    ;input argument is in a4
    ;return value is in a4
    bnop b3, 1
    mvk   1, a5
    mvc ier, b0
    ||shl  a5, a4, a5 ; Or mask obtained
    shru b0, a5, a4 ; return value obtained
    ||or   a5, b0, b0
    mvc  b0, ier
    ||and a4,1,a4
   
    .global _int_irq_disable
_int_irq_disable:
    ;input argument is in a4
    ;return value is in a4
    bnop b3, 1
    mvk   1, a5
    mvc ier, b0
    ||shl  a5, a4, a5 ; Or mask obtained
    shru b0, a4, a4 ; return value obtained
    ||andn  b0, a5, b0
    mvc  b0, ier
    ||and a4,1,a4

    ; borrowed from TI's CSL_intcInterruptClear() code
    .global _int_irq_clear
_int_irq_clear:
    ;input argument is in a4
    bnop b3, 1
    mv   a4,b2
    mvk   1, b1
    shl  b1, b2, b1 ; Or mask obtained
    mvc  b1, icr
