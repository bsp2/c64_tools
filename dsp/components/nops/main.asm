;******************************************************************************
;* TMS320C6x C/C++ Codegen                                        Unix v7.4.5 *
;* Date/Time created: Fri Jan 17 15:11:50 2014                                *
;******************************************************************************
	.compiler_opts --abi=coffabi --c64p_l1d_workaround=default --endian=little --hll_source=on --long_precision_bits=40 --mem_model:code=near --mem_model:const=far --mem_model:data=far --object_format=coff --silicon_version=6500 --symdebug:none 

;******************************************************************************
;* GLOBAL FILE PARAMETERS                                                     *
;*                                                                            *
;*   Architecture      : TMS320C64x+                                          *
;*   Optimization      : Enabled at level 3                                   *
;*   Optimizing for    : Speed                                                *
;*                       Based on options: -o3, no -ms                        *
;*   Endian            : Little                                               *
;*   Interrupt Thrshld : Disabled                                             *
;*   Data Access Model : Far                                                  *
;*   Pipelining        : Enabled                                              *
;*   Speculate Loads   : Enabled with threshold = 0                           *
;*   Memory Aliases    : Presume are aliases (pessimistic)                    *
;*   Debug Info        : No Debug Info                                        *
;*                                                                            *
;******************************************************************************

	.asg	A15, FP
	.asg	B14, DP
	.asg	B15, SP
	.global	$bss

;*****************************************************************************
;* CINIT RECORDS                                                             *
;*****************************************************************************
	.sect	".cinit"
	.align	8
	.field  	$C$IR_1,32
	.field  	_component_nops+0,32
	.bits	0,32			; _component_nops._fxns._init @ 0
	.bits	_loc_exec,32		; _component_nops._fxns._exec @ 32
	.bits	0,32			; _component_nops._fxns._exec_fc @ 64
	.bits	0,32			; _component_nops._fxns._exit @ 96
	.bits	110,8			; _component_nops._name[0] @ 128
	.bits	111,8			; _component_nops._name[1] @ 136
	.bits	112,8			; _component_nops._name[2] @ 144
	.bits	115,8			; _component_nops._name[3] @ 152
	.bits	0,8			; _component_nops._name[4] @ 160
$C$IR_1:	.set	21

	.global	_syscalls
_syscalls:	.usect	".sec_mlb_syscalls",216,4
	.global	_component_nops
_component_nops:	.usect	".sec_com",52,4
;	opt6x /tmp/13997HVqDeC /tmp/139979W7Nmw 
	.sect	".text"
	.clink
	.global	_main

;******************************************************************************
;* FUNCTION NAME: main                                                        *
;*                                                                            *
;*   Regs Modified     :                                                      *
;*   Regs Used         : B3                                                   *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_main:
;** --------------------------------------------------------------------------*
;**  	-----------------------    return;
           RETNOP  .S2     B3,5              ; |84| 
           ; BRANCH OCCURS {B3}              ; |84| 
	.sect	".text"
	.clink

;******************************************************************************
;* FUNCTION NAME: loc_exec                                                    *
;*                                                                            *
;*   Regs Modified     : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,B0,B1,B2,B3,B4,*
;*                           B5,B6,B7,B8,B9,B10,B11,SP,A16,A17,A18,A19,A20,   *
;*                           A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31,B16, *
;*                           B17,B18,B19,B20,B21,B22,B23,B24,B25,B26,B27,B28, *
;*                           B29,B30,B31                                      *
;*   Regs Used         : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,B0,B1,B2,B3,B4,*
;*                           B5,B6,B7,B8,B9,B10,B11,DP,SP,A16,A17,A18,A19,A20,*
;*                           A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31,B16, *
;*                           B17,B18,B19,B20,B21,B22,B23,B24,B25,B26,B27,B28, *
;*                           B29,B30,B31                                      *
;*   Local Frame Size  : 0 Args + 0 Auto + 24 Save = 24 byte                  *
;******************************************************************************
_loc_exec:
;** --------------------------------------------------------------------------*
;** 48	-----------------------    if ( _cmd == 1 ) goto g3;
;** 64	-----------------------    return 1u;

           STW     .D2T1   A11,*SP--(8)      ; |45| 
||         CMPEQ   .L1     A4,1,A0           ; |48| 
||         MV      .S1X    B3,A11            ; |45| 

           STDW    .D2T2   B11:B10,*SP--     ; |45| 
|| [!A0]   B       .S1     $C$L1             ; |64| 
||         MV      .L2     B6,B10            ; |45| 
||         ZERO    .S2     B11               ; |59| 

   [ A0]   CALL    .S1     _nops             ; |57| 
||         STW     .D2T1   A10,*SP--(8)      ; |45| 
||         MVK     .L1     0x1,A10           ; |64| 
|| [!A0]   MV      .L2X    A11,B3            ; |65| 

   [!A0]   MV      .L1     A10,A4            ; |64| 
|| [!A0]   LDW     .D2T1   *++SP(8),A10      ; |65| 

   [!A0]   RET     .S2     B3                ; |65| 
|| [!A0]   LDDW    .D2T2   *++SP,B11:B10     ; |65| 

   [!A0]   LDW     .D2T1   *++SP(8),A11      ; |65| 
           NOP             1
           ; BRANCHCC OCCURS {$C$L1}         ; |64| 
;** --------------------------------------------------------------------------*
;**	-----------------------g3:
;** 57	-----------------------    nops();
;** 59	-----------------------    *_ret1 = 0u;
;** 64	-----------------------    return 1u;
           ADDKPC  .S2     $C$RL0,B3,0       ; |57| 
$C$RL0:    ; CALL OCCURS {_nops} {0}         ; |57| 
;** --------------------------------------------------------------------------*
           STW     .D2T2   B11,*B10          ; |59| 

           MV      .L1     A10,A4            ; |64| 
||         LDW     .D2T1   *++SP(8),A10      ; |65| 
||         MV      .L2X    A11,B3            ; |65| 

           LDDW    .D2T2   *++SP,B11:B10     ; |65| 
||         RET     .S2     B3                ; |65| 

           LDW     .D2T1   *++SP(8),A11      ; |65| 
           NOP             1
;** --------------------------------------------------------------------------*
$C$L1:    
           NOP             3
           ; BRANCH OCCURS {B3}              ; |65| 
;*****************************************************************************
;* UNDEFINED EXTERNAL REFERENCES                                             *
;*****************************************************************************
	.global	_nops

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_needed(0)
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_preserved(0)
	.battr "TI", Tag_File, 1, Tag_Tramps_Use_SOC(1)
