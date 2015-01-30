;******************************************************************************
;* TMS320C6x C/C++ Codegen                                        Unix v7.4.5 *
;* Date/Time created: Fri Jan 30 12:46:34 2015                                *
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
	.field  	_component_sramtest+0,32
	.bits	0,32			; _component_sramtest._fxns._init @ 0
	.bits	_loc_exec,32		; _component_sramtest._fxns._exec @ 32
	.bits	0,32			; _component_sramtest._fxns._exec_fc @ 64
	.bits	0,32			; _component_sramtest._fxns._exit @ 96
	.bits	115,8			; _component_sramtest._name[0] @ 128
	.bits	114,8			; _component_sramtest._name[1] @ 136
	.bits	97,8			; _component_sramtest._name[2] @ 144
	.bits	109,8			; _component_sramtest._name[3] @ 152
	.bits	116,8			; _component_sramtest._name[4] @ 160
	.bits	101,8			; _component_sramtest._name[5] @ 168
	.bits	115,8			; _component_sramtest._name[6] @ 176
	.bits	116,8			; _component_sramtest._name[7] @ 184
	.bits	0,8			; _component_sramtest._name[8] @ 192
$C$IR_1:	.set	25

	.global	_syscalls
_syscalls:	.usect	".sec_mlb_syscalls",216,4
	.global	_component_sramtest
_component_sramtest:	.usect	".sec_com",52,4
;	opt6x /tmp/03558c3oygt /tmp/03558M7Nxt5 
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
           RETNOP  .S2     B3,5              ; |598| 
           ; BRANCH OCCURS {B3}              ; |598| 
	.sect	".text"
	.clink

;******************************************************************************
;* FUNCTION NAME: loc_exec                                                    *
;*                                                                            *
;*   Regs Modified     : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,B0,*
;*                           B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,SP,   *
;*                           A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27, *
;*                           A28,A29,A30,A31,B16,B17,B18,B19,B20,B21,B22,B23, *
;*                           B24,B25,B26,B27,B28,B29,B30,B31                  *
;*   Regs Used         : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,B0,*
;*                           B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,DP,SP,*
;*                           A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27, *
;*                           A28,A29,A30,A31,B16,B17,B18,B19,B20,B21,B22,B23, *
;*                           B24,B25,B26,B27,B28,B29,B30,B31                  *
;*   Local Frame Size  : 0 Args + 0 Auto + 48 Save = 48 byte                  *
;******************************************************************************
_loc_exec:
;** --------------------------------------------------------------------------*
;** 465	-----------------------    v$1 = syscalls.qdma_init;
;** 465	-----------------------    switch ( _cmd ) {case 1: goto g78;, case 2: goto g72;, case 3: goto g66;, case 4: goto g60;, case 5: goto g54;, case 6: goto g48;, case 7: goto g42;, case 8: goto g33;, case 9: goto g27;, case 10: goto g21;, case 11: goto g15;, case 12: goto g9;, case 13: goto g3;, DEFAULT goto g2};

           SUB     .L1     A4,1,A3           ; |465| 
||         STW     .D2T1   A14,*SP--(8)      ; |462| 

           CMPGTU  .L1     A3,12,A0          ; |465| 
||         STDW    .D2T2   B13:B12,*SP--     ; |462| 

   [ A0]   B       .S2     $C$L1             ; |465| 
||         STDW    .D2T2   B11:B10,*SP--     ; |462| 
||         MV      .L2     B6,B10            ; |462| 
|| [!A0]   SHL     .S1     A4,2,A3           ; |465| 

           STDW    .D2T1   A13:A12,*SP--     ; |462| 
||         MVKL    .S1     _syscalls+192,A13
||         MV      .L1     A6,A12            ; |462| 
|| [!A0]   SUB     .D1     A3,4,A3           ; |465| 

           STDW    .D2T1   A11:A10,*SP--     ; |462| 
||         MVKH    .S1     _syscalls+192,A13
||         MV      .L1X    B4,A10            ; |462| 
|| [!A0]   MVKL    .S2     $C$SW1,B4

           STW     .D2T2   B3,*SP--(8)       ; |462| 
|| [!A0]   MVKH    .S2     $C$SW1,B4

           LDW     .D1T1   *A13,A5           ; |465| 
   [!A0]   ADD     .L1X    B4,A3,A3          ; |465| 
           ; BRANCHCC OCCURS {$C$L1} {-14}   ; |465| 
;** --------------------------------------------------------------------------*
           LDW     .D1T1   *A3,A3            ; |465| 
           NOP             4
           BNOP    .S2X    A3,5              ; |465| 
           ; BRANCH OCCURS {A3}              ; |465| 
	.sect	".switch:_loc_exec"
	.clink
$C$SW1:	.word	$C$L76	; 1
	.word	$C$L70	; 2
	.word	$C$L64	; 3
	.word	$C$L58	; 4
	.word	$C$L52	; 5
	.word	$C$L47	; 6
	.word	$C$L42	; 7
	.word	$C$L32	; 8
	.word	$C$L26	; 9
	.word	$C$L20	; 10
	.word	$C$L14	; 11
	.word	$C$L8	; 12
	.word	$C$L2	; 13
	.sect	".text"
;** --------------------------------------------------------------------------*
$C$L1:    
;**	-----------------------g2:
;** 578	-----------------------    return 1u;
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           MVK     .L1     0x1,A4            ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L2:    
;**	-----------------------g3:
;** 413	-----------------------    dRAM = (unsigned *)_arg2;  // [12]
;** 415	-----------------------    (*v$1)();  // [12]
;** 417	-----------------------    if ( !_arg1 ) goto g8;  // [12]
           CALL    .S2X    A5                ; |415| 
           ADDKPC  .S2     $C$RL0,B3,4       ; |415| 
$C$RL0:    ; CALL OCCURS {A5} {0}            ; |415| 
;** --------------------------------------------------------------------------*
;**  	-----------------------    K$38 = 12256u;
;** 420	-----------------------    L$1 = _arg1;  // [12]
;** 409	-----------------------    srcCur = (volatile unsigned *)0x10f04040u;  // [12]
;** 410	-----------------------    srcNext = (volatile unsigned *)0x10f07020u;  // [12]
;** 411	-----------------------    dstCur = (volatile unsigned *)0x107f8000u;  // [12]
;** 412	-----------------------    dstNext = (volatile unsigned *)0x107fafe0u;  // [12]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(5120u)

           MV      .L1     A10,A0            ; |409| 
||         MVKL    .S2     0x107fafe0,B4
||         MV      .S1     A10,A11           ; |409| 

           MVKH    .S2     0x107fafe0,B4

           MVKL    .S2     0x10f04040,B13
|| [!A0]   B       .S1     $C$L7             ; |417| 

           MVKL    .S2     0x10f07020,B12
||         MV      .L1X    B4,A14            ; |412| 

           MVKL    .S2     0x107f8000,B11
           MVKH    .S2     0x10f04040,B13
           MVKH    .S2     0x10f07020,B12
           MVKH    .S2     0x107f8000,B11
           ; BRANCHCC OCCURS {$C$L7}         ; |417| 
;** --------------------------------------------------------------------------*

           MVK     .S2     0xbf8,B7          ; |422| 
||         LDW     .D2T2   *B12,B5           ; |420| 

           SUB     .L2     B7,1,B4
||         MVC     .S2     B7,RILC

;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L3
;** --------------------------------------------------------------------------*
$C$L3:    
;**	-----------------------g5:
;** 420	-----------------------    k = *srcNext;  // [12]
;**  	-----------------------    U$30 = srcCur;
;**  	-----------------------    U$31 = dstCur;
;** 422	-----------------------    L$2 = 3064;  // [12]
;**  	-----------------------    #pragma MUST_ITERATE(3064, 3064, 3064)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g6:
;** 424	-----------------------    *U$31++ = *U$30+++k;  // [12]
;** 424	-----------------------    ++k;  // [12]
;** 422	-----------------------    if ( L$2 = L$2-1 ) goto g6;  // [12]
;** 427	-----------------------    (*syscalls.qdma_wait)(0u);  // [12]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 422
;*      Loop opening brace source line   : 423
;*      Loop closing brace source line   : 425
;*      Known Minimum Trip Count         : 3064                    
;*      Known Maximum Trip Count         : 3064                    
;*      Known Max Trip Count Factor      : 3064
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          2        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L4:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1X    B13,A4
||         MVC     .S2     B4,ILC

;** --------------------------------------------------------------------------*
$C$L5:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |424| (P) <0,0>  ^ 
           NOP             4

           SPMASK          S2
||         MV      .S2     B11,B6
||         ADD     .L2X    B5,A3,B4          ; |424| (P) <0,5>  ^ 

           ADD     .L2     1,B5,B5           ; |424| (P) <0,6> 
||         STW     .D2T2   B4,*B6++          ; |424| (P) <0,6>  ^ 

           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L6:    ; PIPED LOOP EPILOG
;** 429	-----------------------    (*syscalls.qdma_copy2d)(0u, dstCur, 0, dRAM, 0, K$38, 1u);  // [12]
;** 441	-----------------------    vt = srcCur;  // [12]
;** 442	-----------------------    srcCur = srcNext;  // [12]
;** 443	-----------------------    srcNext = vt;  // [12]
;** 445	-----------------------    vt = dstCur;  // [12]
;** 446	-----------------------    dstCur = dstNext;  // [12]
;** 447	-----------------------    dstNext = vt;  // [12]
;** 417	-----------------------    if ( L$1 = L$1-1 ) goto g5;  // [12]

           ZERO    .L1     A4                ; |427| 
||         ADD     .S1     4,A13,A10

;** --------------------------------------------------------------------------*
           LDW     .D1T1   *A10,A3           ; |427| 
           NOP             4
           CALL    .S2X    A3                ; |427| 
           ADDKPC  .S2     $C$RL1,B3,4       ; |427| 
$C$RL1:    ; CALL OCCURS {A3} {0}            ; |427| 
;** --------------------------------------------------------------------------*
           ADDAD   .D1     A10,1,A3
           LDW     .D1T1   *A3,A3            ; |429| 
           ZERO    .L1     A4                ; |429| 
           MVK     .S2     0x2fe0,B8
           MV      .L2     B11,B4            ; |429| 
           ZERO    .L1     A6                ; |429| 
           CALL    .S2X    A3                ; |429| 
           MV      .L2X    A12,B6            ; |429| 
           ZERO    .L1     A8                ; |429| 
           MVK     .L1     0x1,A10           ; |429| 
           ADDKPC  .S2     $C$RL2,B3,1       ; |429| 
$C$RL2:    ; CALL OCCURS {A3} {0}            ; |429| 
;** --------------------------------------------------------------------------*

           SUB     .L1     A11,1,A0          ; |417| 
||         MV      .S1X    B13,A4            ; |441| 
||         MV      .L2     B12,B13           ; |442| 
||         SUB     .D1     A11,1,A11         ; |417| 

   [ A0]   B       .S1     $C$L3             ; |417| 
|| [ A0]   MVK     .S2     0xbf8,B7          ; |422| 
||         MV      .L1X    B11,A3            ; |445| 
||         MV      .L2X    A14,B11           ; |446| 

           MV      .L1     A3,A14            ; |447| 
   [ A0]   SUB     .L2     B7,1,B4
           MV      .L2X    A4,B12            ; |443| 
   [ A0]   LDW     .D2T2   *B12,B5           ; |420| 
   [ A0]   MVC     .S2     B7,RILC
           ; BRANCHCC OCCURS {$C$L3}         ; |417| 
;** --------------------------------------------------------------------------*
$C$L7:    
;**	-----------------------g8:
;** 452	-----------------------    (*syscalls.qdma_wait)(0u);  // [12]
;** 570	-----------------------    *_ret1 = 0u;
;** 578	-----------------------    return 0u;
           ADD     .L1     4,A13,A3
           LDW     .D1T1   *A3,A3            ; |452| 
           ZERO    .L1     A4                ; |452| 
           NOP             3
           CALL    .S2X    A3                ; |452| 
           ADDKPC  .S2     $C$RL3,B3,4       ; |452| 
$C$RL3:    ; CALL OCCURS {A3} {0}            ; |452| 
;** --------------------------------------------------------------------------*
           ZERO    .L2     B4                ; |570| 
           STW     .D2T2   B4,*B10           ; |570| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |570| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L8:    
;**	-----------------------g9:
;** 382	-----------------------    ret = 0u;  // [11]
;** 386	-----------------------    if ( !_arg1 ) goto g14;  // [11]
;**  	-----------------------    U$66 = _extu(_arg1, 19u, 19u)*4+(d = (volatile unsigned *)0x107f8000u);
;** 390	-----------------------    L$3 = _arg1;  // [11]
;** 383	-----------------------    s = (volatile unsigned *)0x10f04000u;  // [11]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MVKL    .S2     0x107f8000,B8
||         MVKL    .S1     0x10f04000,A6
||         MV      .L2X    A10,B0
||         ZERO    .D2     B4                ; |382| 

   [!A0]   B       .S1     $C$L13            ; |386| 
||         MVKH    .S2     0x107f8000,B8

   [ A0]   MVK     .S2     0x3000,B7         ; |390| 
||         EXTU    .S1     A0,19,19,A4

   [ A0]   MVC     .S2     B7,RILC
||         MVKH    .S1     0x10f04000,A6

   [ A0]   SUB     .L2     B7,1,B6
           MV      .L1X    B8,A3             ; Define a twin register
           ADDAW   .D1     A3,A4,A5
           ; BRANCHCC OCCURS {$C$L13}        ; |386| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L9
;** --------------------------------------------------------------------------*
$C$L9:    
;**	-----------------------g11:
;**  	-----------------------    U$55 = s;
;**  	-----------------------    U$58 = d;
;** 390	-----------------------    L$4 = 12288;  // [11]
;**  	-----------------------    #pragma MUST_ITERATE(12288, 12288, 12288)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g12:
;** 392	-----------------------    c = *U$55++;  // [11]
;** 393	-----------------------    ret += c;  // [11]
;** 394	-----------------------    *U$58++ = ret;  // [11]
;** 390	-----------------------    if ( L$4 = L$4-1 ) goto g12;  // [11]
;** 397	-----------------------    *s = *U$66;  // [11]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 390
;*      Loop opening brace source line   : 391
;*      Loop closing brace source line   : 395
;*      Known Minimum Trip Count         : 12288                    
;*      Known Maximum Trip Count         : 12288                    
;*      Known Max Trip Count Factor      : 12288
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          1        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L10:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1     A6,A4
||         MVC     .S2     B6,ILC

;** --------------------------------------------------------------------------*
$C$L11:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |392| (P) <0,0>  ^ 
           NOP             4

           SPMASK          S2
||         MV      .S2     B8,B5
||         ADD     .L2X    A3,B4,B4          ; |393| (P) <0,5>  ^ 

           STW     .D2T2   B4,*B5++          ; |394| (P) <0,6>  ^ 
           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L12:    ; PIPED LOOP EPILOG
;** 386	-----------------------    if ( L$3 = L$3-1 ) goto g11;  // [11]

           LDW     .D1T1   *A5,A3            ; |397| 
||         SUB     .L2     B0,1,B0           ; |386| 

;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L9,2           ; |386| 
|| [ B0]   MVK     .S2     0x3000,B7         ; |390| 

   [ B0]   SUB     .L2     B7,1,B6
           STW     .D1T1   A3,*A6            ; |397| 
   [ B0]   MVC     .S2     B7,RILC
           ; BRANCHCC OCCURS {$C$L9}         ; |386| 
;** --------------------------------------------------------------------------*
$C$L13:    
;**	-----------------------g14:
;** 562	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B4,*B10           ; |562| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L14:    
;**	-----------------------g15:
;** 356	-----------------------    ret = 0u;  // [10]
;** 360	-----------------------    if ( !_arg1 ) goto g20;  // [10]
;**  	-----------------------    U$86 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x107f8000u);
;** 364	-----------------------    L$5 = _arg1;  // [10]
;** 358	-----------------------    d = (volatile unsigned *)0x10804000u;  // [10]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x107f8000,A6
||         MVKL    .S2     0x10804000,B8

   [!A0]   B       .S1     $C$L19            ; |360| 
||         EXTU    .S2     B0,19,19,B4

   [ A0]   MVK     .S2     0x6000,B7         ; |364| 
||         MVKH    .S1     0x107f8000,A6

   [ A0]   SUB     .L2     B7,1,B6
||         MVKH    .S2     0x10804000,B8
||         MV      .L1X    B4,A3             ; Define a twin register

           ZERO    .L2     B4                ; |356| 
   [ A0]   MVC     .S2     B7,RILC
           ADDAW   .D1     A6,A3,A5
           ; BRANCHCC OCCURS {$C$L19}        ; |360| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L15
;** --------------------------------------------------------------------------*
$C$L15:    
;**	-----------------------g17:
;**  	-----------------------    U$78 = s;
;**  	-----------------------    U$81 = d;
;** 364	-----------------------    L$6 = 24576;  // [10]
;**  	-----------------------    #pragma MUST_ITERATE(24576, 24576, 24576)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g18:
;** 366	-----------------------    c = *U$78++;  // [10]
;** 367	-----------------------    ret += c;  // [10]
;** 368	-----------------------    *U$81++ = ret;  // [10]
;** 364	-----------------------    if ( L$6 = L$6-1 ) goto g18;  // [10]
;** 371	-----------------------    *s = *U$86;  // [10]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 364
;*      Loop opening brace source line   : 365
;*      Loop closing brace source line   : 369
;*      Known Minimum Trip Count         : 24576                    
;*      Known Maximum Trip Count         : 24576                    
;*      Known Max Trip Count Factor      : 24576
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          1        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L16:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1     A6,A4
||         MVC     .S2     B6,ILC

;** --------------------------------------------------------------------------*
$C$L17:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |366| (P) <0,0>  ^ 
           NOP             4

           SPMASK          S2
||         MV      .S2     B8,B5
||         ADD     .L2X    A3,B4,B4          ; |367| (P) <0,5>  ^ 

           STW     .D2T2   B4,*B5++          ; |368| (P) <0,6>  ^ 
           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L18:    ; PIPED LOOP EPILOG
;** 360	-----------------------    if ( L$5 = L$5-1 ) goto g17;  // [10]

           LDW     .D1T1   *A5,A3            ; |371| 
||         SUB     .L2     B0,1,B0           ; |360| 

;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L15,2          ; |360| 
|| [ B0]   MVK     .S2     0x6000,B7         ; |364| 

   [ B0]   SUB     .L2     B7,1,B6
           STW     .D1T1   A3,*A6            ; |371| 
   [ B0]   MVC     .S2     B7,RILC
           ; BRANCHCC OCCURS {$C$L15}        ; |360| 
;** --------------------------------------------------------------------------*
$C$L19:    
;**	-----------------------g20:
;** 554	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B4,*B10           ; |554| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L20:    
;**	-----------------------g21:
;** 333	-----------------------    ret = 0u;  // [9]
;** 336	-----------------------    if ( !_arg1 ) goto g26;  // [9]
;**  	-----------------------    U$100 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x107f8000u);
;** 340	-----------------------    L$7 = _arg1;  // [9]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
           MV      .L1     A10,A0
   [!A0]   B       .S1     $C$L25            ; |336| 
           MV      .L2X    A10,B0
           EXTU    .S2     B0,19,19,B4
           MVKL    .S1     0x107f8000,A7

           MV      .L1X    B4,A3             ; Define a twin register
||         MVKH    .S1     0x107f8000,A7

           ADDAW   .D1     A7,A3,A6
||         ZERO    .L1     A4                ; |333| 

           ; BRANCHCC OCCURS {$C$L25}        ; |336| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L21
;** --------------------------------------------------------------------------*
$C$L21:    
;**	-----------------------g23:
;**  	-----------------------    U$95 = s;
;** 340	-----------------------    L$8 = 24576;  // [9]
;**  	-----------------------    #pragma MUST_ITERATE(24576, 24576, 24576)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g24:
;** 342	-----------------------    a$3 = *U$95+++ret;  // [9]
;** 342	-----------------------    ret = a$3;  // [9]
;** 340	-----------------------    if ( L$8 = L$8-1 ) goto g24;  // [9]
;** 345	-----------------------    *s = *U$100;  // [9]
           MVK     .S2     0x6000,B5         ; |340| 

           SUB     .L2     B5,4,B4
||         MVC     .S2     B5,RILC

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 340
;*      Loop opening brace source line   : 341
;*      Loop closing brace source line   : 343
;*      Known Minimum Trip Count         : 24576                    
;*      Known Maximum Trip Count         : 24576                    
;*      Known Max Trip Count Factor      : 24576
;*      Loop Carried Dependency Bound(^) : 0
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       0     
;*      .M units                     0        0     
;*      .X cross paths               0        0     
;*      .T address paths             1*       0     
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          1        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       0     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 1  Schedule found with 6 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L22:    ; PIPED LOOP PROLOG

           SPLOOPD 1       ;6                ; (P) 
||         MV      .L1     A7,A5
||         MVC     .S2     B4,ILC

;** --------------------------------------------------------------------------*
$C$L23:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A5++,A3          ; |342| (P) <0,0> 
           NOP             3
           NOP             1

           SPKERNEL 0,0
||         ADD     .L1     A4,A3,A4          ; |342| <0,5> 

;** --------------------------------------------------------------------------*
$C$L24:    ; PIPED LOOP EPILOG
;** 336	-----------------------    if ( L$7 = L$7-1 ) goto g23;  // [9]

           LDW     .D1T1   *A6,A5            ; |345| 
||         SUB     .L2     B0,1,B0           ; |336| 

   [ B0]   BNOP    .S2     $C$L21,3          ; |336| 
           STW     .D1T1   A5,*A7            ; |345| 
           NOP             1
           ; BRANCHCC OCCURS {$C$L21}        ; |336| 
;** --------------------------------------------------------------------------*
$C$L25:    
;**	-----------------------g26:
;** 546	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T1   A4,*B10           ; |546| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L26:    
;**	-----------------------g27:
;** 284	-----------------------    dRAM = (unsigned *)_arg2;  // [8]
;** 286	-----------------------    (*v$1)();  // [8]
;** 288	-----------------------    if ( !_arg1 ) goto g32;  // [8]
           CALL    .S2X    A5                ; |286| 
           ADDKPC  .S2     $C$RL4,B3,4       ; |286| 
$C$RL4:    ; CALL OCCURS {A5} {0}            ; |286| 
;** --------------------------------------------------------------------------*
;**  	-----------------------    K$124 = 6128u;
;** 291	-----------------------    L$9 = _arg1;  // [8]
;** 280	-----------------------    srcCur = (volatile unsigned *)0x10f04040u;  // [8]
;** 281	-----------------------    srcNext = (volatile unsigned *)0x10f05830u;  // [8]
;** 282	-----------------------    dstCur = (volatile unsigned *)0x10f07020u;  // [8]
;** 283	-----------------------    dstNext = (volatile unsigned *)0x10f08810u;  // [8]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(5120u)

           MV      .L1     A10,A0            ; |280| 
||         MVKL    .S2     0x10f08810,B4
||         MV      .S1     A10,A14           ; |280| 

           MVKH    .S2     0x10f08810,B4

           MVKL    .S2     0x10f04040,B13
|| [!A0]   B       .S1     $C$L31            ; |288| 

           MVKL    .S2     0x10f05830,B12
||         MV      .L1X    B4,A11            ; |283| 

           MVKL    .S2     0x10f07020,B11
           MVKH    .S2     0x10f04040,B13
           MVKH    .S2     0x10f05830,B12
           MVKH    .S2     0x10f07020,B11
           ; BRANCHCC OCCURS {$C$L31}        ; |288| 
;** --------------------------------------------------------------------------*

           MVK     .S2     0x5fc,B7          ; |293| 
||         LDW     .D2T2   *B12,B5           ; |291| 

           SUB     .L2     B7,1,B4
||         MVC     .S2     B7,RILC

;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L27
;** --------------------------------------------------------------------------*
$C$L27:    
;**	-----------------------g29:
;** 291	-----------------------    k = *srcNext;  // [8]
;**  	-----------------------    U$117 = srcCur;
;**  	-----------------------    U$118 = dstCur;
;** 293	-----------------------    L$10 = 1532;  // [8]
;**  	-----------------------    #pragma MUST_ITERATE(1532, 1532, 1532)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g30:
;** 295	-----------------------    *U$118++ = *U$117+++k;  // [8]
;** 295	-----------------------    ++k;  // [8]
;** 293	-----------------------    if ( L$10 = L$10-1 ) goto g30;  // [8]
;** 298	-----------------------    (*syscalls.qdma_wait)(0u);  // [8]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 293
;*      Loop opening brace source line   : 294
;*      Loop closing brace source line   : 296
;*      Known Minimum Trip Count         : 1532                    
;*      Known Maximum Trip Count         : 1532                    
;*      Known Max Trip Count Factor      : 1532
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          2        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L28:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1X    B13,A4
||         MVC     .S2     B4,ILC

;** --------------------------------------------------------------------------*
$C$L29:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |295| (P) <0,0>  ^ 
           NOP             4

           SPMASK          S2
||         MV      .S2     B11,B6
||         ADD     .L2X    B5,A3,B4          ; |295| (P) <0,5>  ^ 

           ADD     .L2     1,B5,B5           ; |295| (P) <0,6> 
||         STW     .D2T2   B4,*B6++          ; |295| (P) <0,6>  ^ 

           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L30:    ; PIPED LOOP EPILOG
;** 300	-----------------------    (*syscalls.qdma_copy2d)(0u, dstCur, 0, dRAM, 0, K$124, 1u);  // [8]
;** 312	-----------------------    vt = srcCur;  // [8]
;** 313	-----------------------    srcCur = srcNext;  // [8]
;** 314	-----------------------    srcNext = vt;  // [8]
;** 316	-----------------------    vt = dstCur;  // [8]
;** 317	-----------------------    dstCur = dstNext;  // [8]
;** 318	-----------------------    dstNext = vt;  // [8]
;** 288	-----------------------    if ( L$9 = L$9-1 ) goto g29;  // [8]

           ZERO    .L1     A4                ; |298| 
||         ADD     .S1     4,A13,A10

;** --------------------------------------------------------------------------*
           LDW     .D1T1   *A10,A3           ; |298| 
           NOP             4
           CALL    .S2X    A3                ; |298| 
           ADDKPC  .S2     $C$RL5,B3,4       ; |298| 
$C$RL5:    ; CALL OCCURS {A3} {0}            ; |298| 
;** --------------------------------------------------------------------------*
           ADDAD   .D1     A10,1,A3
           LDW     .D1T1   *A3,A3            ; |300| 
           ZERO    .L1     A4                ; |300| 
           MVK     .S2     0x17f0,B8
           MV      .L2     B11,B4            ; |300| 
           ZERO    .L1     A6                ; |300| 
           CALL    .S2X    A3                ; |300| 
           MV      .L2X    A12,B6            ; |300| 
           ZERO    .L1     A8                ; |300| 
           MVK     .L1     0x1,A10           ; |300| 
           ADDKPC  .S2     $C$RL6,B3,1       ; |300| 
$C$RL6:    ; CALL OCCURS {A3} {0}            ; |300| 
;** --------------------------------------------------------------------------*

           SUB     .L1     A14,1,A0          ; |288| 
||         MV      .S1X    B13,A3            ; |312| 
||         MV      .L2     B12,B13           ; |313| 
||         SUB     .D1     A14,1,A14         ; |288| 

   [ A0]   B       .S1     $C$L27            ; |288| 
|| [ A0]   MVK     .S2     0x5fc,B7          ; |293| 
||         MV      .L1X    B11,A4            ; |316| 
||         MV      .L2X    A11,B11           ; |317| 

           MV      .L1     A4,A11            ; |318| 
   [ A0]   SUB     .L2     B7,1,B4
           MV      .L2X    A3,B12            ; |314| 
   [ A0]   LDW     .D2T2   *B12,B5           ; |291| 
   [ A0]   MVC     .S2     B7,RILC
           ; BRANCHCC OCCURS {$C$L27}        ; |288| 
;** --------------------------------------------------------------------------*
$C$L31:    
;**	-----------------------g32:
;** 323	-----------------------    (*syscalls.qdma_wait)(0u);  // [8]
;** 538	-----------------------    *_ret1 = 0u;
;** 578	-----------------------    return 0u;
           ADD     .L1     4,A13,A3
           LDW     .D1T1   *A3,A3            ; |323| 
           ZERO    .L1     A4                ; |323| 
           NOP             3
           CALL    .S2X    A3                ; |323| 
           ADDKPC  .S2     $C$RL7,B3,4       ; |323| 
$C$RL7:    ; CALL OCCURS {A3} {0}            ; |323| 
;** --------------------------------------------------------------------------*
           ZERO    .L2     B4                ; |538| 
           STW     .D2T2   B4,*B10           ; |538| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |538| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L32:    
;**	-----------------------g33:
;** 257	-----------------------    d = (unsigned *)_arg2;  // [7]
;** 259	-----------------------    if ( !_arg1 ) goto g41;  // [7]
;** 256	-----------------------    s = (volatile unsigned *)0x10f04040u;  // [7]
;**  	-----------------------    U$140 = (d-s >= 24512)|(s-d >= 24512);
;** 262	-----------------------    L$11 = _arg1;  // [7]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MVKL    .S1     0x10f04040,A7
||         MVK     .S2     24512,B5
||         MV      .L2X    A12,B4

   [!A0]   B       .S2     $C$L41            ; |259| 
||         MVKH    .S1     0x10f04040,A7
||         MV      .L2X    A10,B1

           SUB     .L1     A12,A7,A3
||         MVK     .S2     24512,B6
||         MV      .L2X    A12,B7

           CMPLT   .L1X    A3,B5,A3
||         SUB     .L2X    A7,B4,B5
|| [ A0]   LDW     .D2T2   *B7,B8            ; |262| 

           CMPLT   .L2     B5,B6,B5
           AND     .L2X    A3,B5,B5
           XOR     .L2     1,B5,B2
           ; BRANCHCC OCCURS {$C$L41}        ; |259| 
;** --------------------------------------------------------------------------*

   [ B2]   B       .S1     $C$L37            ; |264| 
|| [!B2]   MVK     .S2     0x17f0,B5
|| [ B2]   ADD     .L1     4,A7,A6
|| [ B2]   MV      .L2X    A7,B6
|| [ B2]   MV      .D2     B7,B5
|| [ B2]   ADD     .D1X    4,B7,A5

   [!B2]   SUB     .L2     B5,1,B5
   [ B2]   MV      .L1X    B8,A4
   [ B2]   MVK     .S2     0xbf8,B4
   [ B2]   SUB     .L2     B4,1,B0
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L33
;** --------------------------------------------------------------------------*
$C$L33:    
;**	-----------------------g35:
;** 262	-----------------------    k = *d;  // [7]
;** 264	-----------------------    if ( U$140 ) goto g38;  // [7]
           NOP             1
           ; BRANCHCC OCCURS {$C$L37}        ; |264| 
;** --------------------------------------------------------------------------*
;**  	-----------------------    U$145 = s;
;**  	-----------------------    U$146 = d;
;**  	-----------------------    L$12 = 6128;
;**  	-----------------------    #pragma MUST_ITERATE(6128, 6128, 6128)
;**  	-----------------------    #pragma LOOP_FLAGS(4104u)
;**	-----------------------g37:
;** 266	-----------------------    *U$146++ = *U$145+++k;  // [7]
;** 266	-----------------------    ++k;  // [7]
;** 264	-----------------------    if ( L$12 = L$12-1 ) goto g37;  // [7]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 264
;*      Loop opening brace source line   : 265
;*      Loop closing brace source line   : 267
;*      Known Minimum Trip Count         : 6128                    
;*      Known Maximum Trip Count         : 6128                    
;*      Known Max Trip Count Factor      : 6128
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          2        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L34:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1     A7,A4
||         MVC     .S2     B5,ILC

;** --------------------------------------------------------------------------*
$C$L35:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |266| (P) <0,0>  ^ 
           NOP             3

           SPMASK          L2
||         MV      .L2     B8,B6

           SPMASK          S2
||         MV      .S2     B7,B5
||         ADD     .L2X    B6,A3,B4          ; |266| (P) <0,5>  ^ 

           ADD     .L2     1,B6,B6           ; |266| (P) <0,6> 
||         STW     .D2T2   B4,*B5++          ; |266| (P) <0,6>  ^ 

           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L36:    ; PIPED LOOP EPILOG
;** 264	-----------------------    goto g40;  // [7]
           BNOP    .S2     $C$L40,5          ; |264| 
           ; BRANCH OCCURS {$C$L40}          ; |264| 
;** --------------------------------------------------------------------------*
;**	-----------------------g38:
;**  	-----------------------    U$145 = s;
;**  	-----------------------    U$146 = d;
;**  	-----------------------    U$153 = &U$145[1];
;**  	-----------------------    U$154 = &U$146[1];
;**  	-----------------------    L$13 = 3064;
;**  	-----------------------    #pragma[60] (s != d;)
;**  	-----------------------    #pragma MUST_ITERATE(3064, 3064, 3064)
;**  	-----------------------    // LOOP BELOW UNROLLED BY FACTOR(2)
;**  	-----------------------    #pragma LOOP_FLAGS(4162u)
;**	-----------------------g39:
;** 266	-----------------------    *U$146++{2} = *U$145++{2}+k;  // [7]
;** 266	-----------------------    *U$154++{2} = *U$153++{2}+k+1u;  // [7]
;** 266	-----------------------    k += 2u;  // [7]
;** 264	-----------------------    if ( L$13 = L$13-1 ) goto g39;  // [7]

           ADD     .L1X    4,B7,A5
||         MVK     .S2     0xbf8,B4

           SUB     .L2     B4,1,B0
||         MV      .L1X    B8,A4
||         ADD     .S1     4,A7,A6
||         MV      .S2     B7,B5
||         MV      .D2X    A7,B6

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 264
;*      Loop opening brace source line   : 265
;*      Loop closing brace source line   : 267
;*      Loop Unroll Multiple             : 2x
;*      Known Minimum Trip Count         : 3064                    
;*      Known Maximum Trip Count         : 3064                    
;*      Known Max Trip Count Factor      : 3064
;*      Loop Carried Dependency Bound(^) : 15
;*      Unpartitioned Resource Bound     : 2
;*      Partitioned Resource Bound(*)    : 2
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     1        0     
;*      .D units                     2*       2*    
;*      .M units                     0        0     
;*      .X cross paths               0        1     
;*      .T address paths             2*       2*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          3        1     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             1        0     
;*      Bound(.L .S .D .LS .LSD)     2*       1     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 15 Schedule found with 1 iterations in parallel
;*      Done
;*
;*      Loop is interruptible
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*
;*      Minimum safe trip count       : 1 (after unrolling)
;*----------------------------------------------------------------------------*
$C$L37:    ; PIPED LOOP PROLOG
;** --------------------------------------------------------------------------*
$C$L38:    ; PIPED LOOP KERNEL
           LDW     .D2T2   *B6++(8),B4       ; |266| <0,0>  ^ 
           NOP             4
           ADD     .L2X    A4,B4,B4          ; |266| <0,5>  ^ 
           STW     .D2T2   B4,*B5++(8)       ; |266| <0,6>  ^ 
           LDW     .D1T1   *A6++(8),A3       ; |266| <0,7>  ^ 
           NOP             1
   [ B0]   BDEC    .S2     $C$L38,B0         ; |264| <0,9> 
           NOP             2
           ADD     .L1     A4,A3,A3          ; |266| <0,12>  ^ 
           ADD     .L1     1,A3,A3           ; |266| <0,13>  ^ 

           ADD     .L1     2,A4,A4           ; |266| <0,14> 
||         STW     .D1T1   A3,*A5++(8)       ; |266| <0,14>  ^ 

;** --------------------------------------------------------------------------*
$C$L39:    ; PIPED LOOP EPILOG
;** --------------------------------------------------------------------------*
$C$L40:    
;**	-----------------------g40:
;** 259	-----------------------    if ( L$11 = L$11-1 ) goto g35;  // [7]
           SUB     .L2     B1,1,B1           ; |259| 

   [ B1]   LDW     .D2T2   *B7,B8            ; |262| 
|| [ B1]   B       .S1     $C$L33            ; |259| 
|| [!B1]   ZERO    .L2     B2                ; nullify predicate

   [ B2]   B       .S1     $C$L37            ; |264| 
|| [ B2]   MVK     .S2     0xbf8,B4
|| [ B2]   ADD     .L1X    4,B7,A5
|| [ B2]   MV      .L2X    A7,B6
|| [ B2]   ADD     .D1     4,A7,A6
|| [ B2]   MV      .D2     B7,B5

   [ B2]   SUB     .L2     B4,1,B0
   [!B2]   MVK     .S2     0x17f0,B5
   [!B2]   SUB     .L2     B5,1,B5
   [ B2]   MV      .L1X    B8,A4
           ; BRANCHCC OCCURS {$C$L33}        ; |259| 
;** --------------------------------------------------------------------------*
$C$L41:    
;**	-----------------------g41:
;** 530	-----------------------    *_ret1 = 0u;
;** 578	-----------------------    return 0u;
           ZERO    .L2     B4                ; |530| 
           STW     .D2T2   B4,*B10           ; |530| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |530| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L42:    
;**	-----------------------g42:
;** 215	-----------------------    ret = 0u;  // [6]
;** 219	-----------------------    if ( !_arg1 ) goto g47;  // [6]
;**  	-----------------------    U$216 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 223	-----------------------    L$14 = _arg1;  // [6]
;** 217	-----------------------    d = (volatile unsigned *)0x10f07040u;  // [6]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x10f04040,A17
||         MVKL    .S2     0x10f07040,B26
||         ZERO    .D2     B16               ; |215| 

   [!A0]   B       .S1     $C$L46            ; |219| 
||         EXTU    .S2     B0,19,19,B4

           MVKH    .S1     0x10f04040,A17
||         MVKH    .S2     0x10f07040,B26

           MV      .L1X    B4,A3             ; Define a twin register
|| [ A0]   MV      .L2     B26,B24

           ADDAW   .D1     A17,A3,A16
|| [ A0]   MV      .L2X    A17,B6
||         MV      .L1     A17,A3            ; Define a twin register
|| [ A0]   ADD     .D2     B24,28,B17

   [ A0]   ADD     .D2     B6,16,B9
|| [ A0]   ADD     .L2X    8,A17,B5

   [ A0]   ADD     .D2     B6,24,B8
           ; BRANCHCC OCCURS {$C$L46}        ; |219| 
;** --------------------------------------------------------------------------*
;**	-----------------------g44:
;**  	-----------------------    U$170 = s;
;**  	-----------------------    U$174 = &U$170[1];
;**  	-----------------------    U$178 = &U$170[2];
;**  	-----------------------    U$183 = &U$170[3];
;**  	-----------------------    U$187 = &U$170[4];
;**  	-----------------------    U$192 = &U$170[5];
;**  	-----------------------    U$197 = &U$170[6];
;**  	-----------------------    U$202 = &U$170[7];
;**  	-----------------------    U$204 = d;
;**  	-----------------------    U$205 = &U$204[1];
;**  	-----------------------    U$206 = &U$204[2];
;**  	-----------------------    U$207 = &U$204[3];
;**  	-----------------------    U$208 = &U$204[4];
;**  	-----------------------    U$209 = &U$204[5];
;**  	-----------------------    U$210 = &U$204[6];
;**  	-----------------------    U$211 = &U$204[7];
;** 223	-----------------------    L$15 = 383;  // [6]
;**  	-----------------------    #pragma MUST_ITERATE(383, 383, 383)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g45:
;** 225	-----------------------    a$4 = *U$170++{8}+ret;  // [6]
;** 226	-----------------------    a$5 = *U$174++{8}+a$4;  // [6]
;** 227	-----------------------    a$6 = *U$178++{8}+a$5;  // [6]
;** 228	-----------------------    a$7 = *U$183++{8}+a$6;  // [6]
;** 229	-----------------------    a$8 = *U$187++{8}+a$7;  // [6]
;** 230	-----------------------    a$9 = *U$192++{8}+a$8;  // [6]
;** 231	-----------------------    a$10 = *U$197++{8}+a$9;  // [6]
;** 232	-----------------------    a$11 = *U$202++{8}+a$10;  // [6]
;** 232	-----------------------    ret = a$11;  // [6]
;** 234	-----------------------    *U$204++{8} = ret;  // [6]
;** 235	-----------------------    *U$205++{8} = ret;  // [6]
;** 236	-----------------------    *U$206++{8} = ret;  // [6]
;** 237	-----------------------    *U$207++{8} = ret;  // [6]
;** 238	-----------------------    *U$208++{8} = ret;  // [6]
;** 239	-----------------------    *U$209++{8} = ret;  // [6]
;** 240	-----------------------    *U$210++{8} = ret;  // [6]
;** 241	-----------------------    *U$211++{8} = ret;  // [6]
;** 223	-----------------------    if ( L$15 = L$15-1 ) goto g45;  // [6]
;** 244	-----------------------    *s = *U$216;  // [6]

           MVK     .S1     0x17f,A0          ; |223| 
||         ADD     .D2     B24,24,B18
||         ADD     .D1     A3,20,A7
||         ADD     .L1     4,A17,A9
||         ADD     .L2     4,B26,B23
||         ADD     .S2     12,B26,B21

           SUB     .L1     A0,1,A0
||         ADD     .D2     B24,20,B19
||         ADD     .D1     A3,28,A6
||         ADD     .S1     12,A17,A8
||         ADD     .L2     8,B26,B22

           ADD     .D2     B24,16,B20
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 223
;*      Loop opening brace source line   : 224
;*      Loop closing brace source line   : 242
;*      Known Minimum Trip Count         : 383                    
;*      Known Maximum Trip Count         : 383                    
;*      Known Max Trip Count Factor      : 383
;*      Loop Carried Dependency Bound(^) : 21
;*      Unpartitioned Resource Bound     : 8
;*      Partitioned Resource Bound(*)    : 12
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     1        0     
;*      .D units                     4       12*    
;*      .M units                     0        0     
;*      .X cross paths               0        4     
;*      .T address paths             4       12*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          0        8     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             1        0     
;*      Bound(.L .S .D .LS .LSD)     2        7     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 21 Schedule found with 1 iterations in parallel
;*      Done
;*
;*      Loop is interruptible
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L43:    ; PIPED LOOP PROLOG
;** --------------------------------------------------------------------------*
$C$L44:    ; PIPED LOOP KERNEL
           LDW     .D2T2   *B6++(32),B4      ; |225| <0,0>  ^ 
           LDW     .D1T1   *A9++(32),A5      ; |226| <0,1>  ^ 
           LDW     .D2T2   *B5++(32),B25     ; |227| <0,2> 
           LDW     .D1T1   *A8++(32),A4      ; |228| <0,3> 
           LDW     .D2T2   *B9++(32),B7      ; |229| <0,4> 

           LDW     .D2T2   *B8++(32),B4      ; |231| <0,5> 
||         LDW     .D1T1   *A7++(32),A3      ; |230| <0,5> 
||         ADD     .L2     B16,B4,B16        ; |225| <0,5>  ^ 

           LDW     .D1T1   *A6++(32),A5      ; |232| <0,6> 
||         ADD     .L2X    B16,A5,B16        ; |226| <0,6>  ^ 

           ADD     .L2     B16,B25,B16       ; |227| <0,7>  ^ 
           ADD     .L2X    B16,A4,B16        ; |228| <0,8>  ^ 
           ADD     .L2     B16,B7,B7         ; |229| <0,9>  ^ 
           ADD     .L2X    B7,A3,B7          ; |230| <0,10>  ^ 
           ADD     .L2     B7,B4,B4          ; |231| <0,11>  ^ 
           ADD     .L2X    B4,A5,B16         ; |232| <0,12>  ^ 
           STW     .D2T2   B16,*B24++(32)    ; |234| <0,13>  ^ 
           STW     .D2T2   B16,*B23++(32)    ; |235| <0,14>  ^ 

   [ A0]   BDEC    .S1     $C$L44,A0         ; |223| <0,15> 
||         STW     .D2T2   B16,*B22++(32)    ; |236| <0,15>  ^ 

           STW     .D2T2   B16,*B21++(32)    ; |237| <0,16>  ^ 
           STW     .D2T2   B16,*B20++(32)    ; |238| <0,17>  ^ 
           STW     .D2T2   B16,*B19++(32)    ; |239| <0,18>  ^ 
           STW     .D2T2   B16,*B18++(32)    ; |240| <0,19>  ^ 
           STW     .D2T2   B16,*B17++(32)    ; |241| <0,20>  ^ 
;** --------------------------------------------------------------------------*
$C$L45:    ; PIPED LOOP EPILOG
;** --------------------------------------------------------------------------*
;** 219	-----------------------    if ( L$14 = L$14-1 ) goto g44;  // [6]
;**	-----------------------g44:
;**  	-----------------------    U$170 = s;
;**  	-----------------------    U$174 = &U$170[1];
;**  	-----------------------    U$178 = &U$170[2];
;**  	-----------------------    U$183 = &U$170[3];
;**  	-----------------------    U$187 = &U$170[4];
;**  	-----------------------    U$192 = &U$170[5];
;**  	-----------------------    U$197 = &U$170[6];
;**  	-----------------------    U$202 = &U$170[7];
;**  	-----------------------    U$204 = d;
;**  	-----------------------    U$205 = &U$204[1];
;**  	-----------------------    U$206 = &U$204[2];
;**  	-----------------------    U$207 = &U$204[3];
;**  	-----------------------    U$208 = &U$204[4];
;**  	-----------------------    U$209 = &U$204[5];
;**  	-----------------------    U$210 = &U$204[6];
;**  	-----------------------    U$211 = &U$204[7];
;** 223	-----------------------    L$15 = 383;  // [6]
;**  	-----------------------    #pragma MUST_ITERATE(383, 383, 383)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g45:
;** 225	-----------------------    a$4 = *U$170++{8}+ret;  // [6]
;** 226	-----------------------    a$5 = *U$174++{8}+a$4;  // [6]
;** 227	-----------------------    a$6 = *U$178++{8}+a$5;  // [6]
;** 228	-----------------------    a$7 = *U$183++{8}+a$6;  // [6]
;** 229	-----------------------    a$8 = *U$187++{8}+a$7;  // [6]
;** 230	-----------------------    a$9 = *U$192++{8}+a$8;  // [6]
;** 231	-----------------------    a$10 = *U$197++{8}+a$9;  // [6]
;** 232	-----------------------    a$11 = *U$202++{8}+a$10;  // [6]
;** 232	-----------------------    ret = a$11;  // [6]
;** 234	-----------------------    *U$204++{8} = ret;  // [6]
;** 235	-----------------------    *U$205++{8} = ret;  // [6]
;** 236	-----------------------    *U$206++{8} = ret;  // [6]
;** 237	-----------------------    *U$207++{8} = ret;  // [6]
;** 238	-----------------------    *U$208++{8} = ret;  // [6]
;** 239	-----------------------    *U$209++{8} = ret;  // [6]
;** 240	-----------------------    *U$210++{8} = ret;  // [6]
;** 241	-----------------------    *U$211++{8} = ret;  // [6]
;** 223	-----------------------    if ( L$15 = L$15-1 ) goto g45;  // [6]
;** 244	-----------------------    *s = *U$216;  // [6]

           SUB     .L2     B0,1,B0           ; |219| 
||         MV      .S2     B26,B24

   [ B0]   B       .S1     $C$L43            ; |219| 
||         ADD     .D2     B24,28,B17
||         LDW     .D1T1   *A16,A3           ; |244| 

           ADD     .D2     B24,16,B20

           ADD     .D2     B24,20,B19
||         MV      .L1     A17,A4            ; Define a twin register

           ADD     .D2     B24,24,B18
||         ADD     .D1     A4,20,A7
||         MV      .L2X    A17,B6

           ADD     .D1     A4,28,A6
||         ADD     .D2     B6,16,B9
||         ADD     .L1     4,A17,A9
||         ADD     .L2     4,B26,B23
||         ADD     .S2     8,B26,B22
||         MVK     .S1     0x17f,A5          ; |223| 

           STW     .D1T1   A3,*A17           ; |244| 
||         ADD     .D2     B6,24,B8
||         SUB     .L1     A5,1,A0
||         ADD     .L2X    8,A17,B5
||         ADD     .S1     12,A17,A8
||         ADD     .S2     12,B26,B21

           ; BRANCHCC OCCURS {$C$L43}        ; |219| 
;** --------------------------------------------------------------------------*
$C$L46:    
;**	-----------------------g47:
;** 522	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B16,*B10          ; |522| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L47:    
;**	-----------------------g48:
;** 179	-----------------------    ret = 0u;  // [5]
;** 183	-----------------------    if ( !_arg1 ) goto g53;  // [5]
;**  	-----------------------    U$257 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 187	-----------------------    L$16 = _arg1;  // [5]
;** 181	-----------------------    d = (volatile unsigned *)0x10f07040u;  // [5]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x10f04040,A16
||         MVKL    .S2     0x10f07040,B23
||         ZERO    .D2     B16               ; |179| 

   [!A0]   B       .S1     $C$L51            ; |183| 
||         EXTU    .S2     B0,19,19,B4

           MVKH    .S1     0x10f04040,A16
||         MVKH    .S2     0x10f07040,B23

           MV      .L1X    B4,A3             ; Define a twin register
|| [ A0]   MV      .L2     B23,B22
|| [ A0]   ADD     .S2     8,B23,B20
|| [ A0]   ADD     .S1     12,A16,A7

           ADDAW   .D1     A16,A3,A9
||         MV      .L1     A16,A3            ; Define a twin register
|| [ A0]   MV      .L2X    A16,B4
|| [ A0]   ADD     .D2     B22,20,B17

           ADD     .D1     A3,20,A6
|| [ A0]   ADD     .D2     B4,16,B6
|| [ A0]   ADD     .L2X    8,A16,B7

   [ A0]   ADD     .D2     B22,16,B18
||         MVK     .S1     0x1ff,A0          ; |187| 

           ; BRANCHCC OCCURS {$C$L51}        ; |183| 
;** --------------------------------------------------------------------------*
;**	-----------------------g50:
;**  	-----------------------    U$226 = s;
;**  	-----------------------    U$230 = &U$226[1];
;**  	-----------------------    U$234 = &U$226[2];
;**  	-----------------------    U$238 = &U$226[3];
;**  	-----------------------    U$242 = &U$226[4];
;**  	-----------------------    U$246 = &U$226[5];
;**  	-----------------------    U$248 = d;
;**  	-----------------------    U$249 = &U$248[1];
;**  	-----------------------    U$250 = &U$248[2];
;**  	-----------------------    U$251 = &U$248[3];
;**  	-----------------------    U$252 = &U$248[4];
;**  	-----------------------    U$253 = &U$248[5];
;** 187	-----------------------    L$17 = 511;  // [5]
;**  	-----------------------    #pragma MUST_ITERATE(511, 511, 511)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g51:
;** 189	-----------------------    a$12 = *U$226++{6}+ret;  // [5]
;** 190	-----------------------    a$13 = *U$230++{6}+a$12;  // [5]
;** 191	-----------------------    a$14 = *U$234++{6}+a$13;  // [5]
;** 192	-----------------------    a$15 = *U$238++{6}+a$14;  // [5]
;** 193	-----------------------    a$16 = *U$242++{6}+a$15;  // [5]
;** 194	-----------------------    a$17 = *U$246++{6}+a$16;  // [5]
;** 194	-----------------------    ret = a$17;  // [5]
;** 196	-----------------------    *U$248++{6} = ret;  // [5]
;** 197	-----------------------    *U$249++{6} = ret;  // [5]
;** 198	-----------------------    *U$250++{6} = ret;  // [5]
;** 199	-----------------------    *U$251++{6} = ret;  // [5]
;** 200	-----------------------    *U$252++{6} = ret;  // [5]
;** 201	-----------------------    *U$253++{6} = ret;  // [5]
;** 187	-----------------------    if ( L$17 = L$17-1 ) goto g51;  // [5]
;** 204	-----------------------    *s = *U$257;  // [5]

           ADD     .L1     4,A16,A8
||         ADD     .L2     4,B23,B21
||         ADD     .S2     12,B23,B19
||         SUB     .S1     A0,1,A0

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 187
;*      Loop opening brace source line   : 188
;*      Loop closing brace source line   : 202
;*      Known Minimum Trip Count         : 511                    
;*      Known Maximum Trip Count         : 511                    
;*      Known Max Trip Count Factor      : 511
;*      Loop Carried Dependency Bound(^) : 17
;*      Unpartitioned Resource Bound     : 6
;*      Partitioned Resource Bound(*)    : 9
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     1        0     
;*      .D units                     3        9*    
;*      .M units                     0        0     
;*      .X cross paths               0        3     
;*      .T address paths             3        9*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          0        6     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             1        0     
;*      Bound(.L .S .D .LS .LSD)     2        5     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 17 Schedule found with 1 iterations in parallel
;*      Done
;*
;*      Loop is interruptible
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L48:    ; PIPED LOOP PROLOG
;** --------------------------------------------------------------------------*
$C$L49:    ; PIPED LOOP KERNEL
           LDW     .D2T2   *B4++(24),B9      ; |189| <0,0>  ^ 
           LDW     .D1T1   *A8++(24),A5      ; |190| <0,1>  ^ 
           LDW     .D2T2   *B7++(24),B8      ; |191| <0,2> 

           LDW     .D2T2   *B6++(24),B5      ; |193| <0,3> 
||         LDW     .D1T1   *A7++(24),A4      ; |192| <0,3> 

           LDW     .D1T1   *A6++(24),A3      ; |194| <0,4> 
           ADD     .L2     B16,B9,B9         ; |189| <0,5>  ^ 
           ADD     .L2X    B9,A5,B9          ; |190| <0,6>  ^ 
           ADD     .L2     B9,B8,B8          ; |191| <0,7>  ^ 
           ADD     .L2X    B8,A4,B8          ; |192| <0,8>  ^ 
           ADD     .L2     B8,B5,B5          ; |193| <0,9>  ^ 
           ADD     .L2X    B5,A3,B16         ; |194| <0,10>  ^ 

   [ A0]   BDEC    .S1     $C$L49,A0         ; |187| <0,11> 
||         STW     .D2T2   B16,*B22++(24)    ; |196| <0,11>  ^ 

           STW     .D2T2   B16,*B21++(24)    ; |197| <0,12>  ^ 
           STW     .D2T2   B16,*B20++(24)    ; |198| <0,13>  ^ 
           STW     .D2T2   B16,*B19++(24)    ; |199| <0,14>  ^ 
           STW     .D2T2   B16,*B18++(24)    ; |200| <0,15>  ^ 
           STW     .D2T2   B16,*B17++(24)    ; |201| <0,16>  ^ 
;** --------------------------------------------------------------------------*
$C$L50:    ; PIPED LOOP EPILOG
;** --------------------------------------------------------------------------*
;** 183	-----------------------    if ( L$16 = L$16-1 ) goto g50;  // [5]
;**	-----------------------g50:
;**  	-----------------------    U$226 = s;
;**  	-----------------------    U$230 = &U$226[1];
;**  	-----------------------    U$234 = &U$226[2];
;**  	-----------------------    U$238 = &U$226[3];
;**  	-----------------------    U$242 = &U$226[4];
;**  	-----------------------    U$246 = &U$226[5];
;**  	-----------------------    U$248 = d;
;**  	-----------------------    U$249 = &U$248[1];
;**  	-----------------------    U$250 = &U$248[2];
;**  	-----------------------    U$251 = &U$248[3];
;**  	-----------------------    U$252 = &U$248[4];
;**  	-----------------------    U$253 = &U$248[5];
;** 187	-----------------------    L$17 = 511;  // [5]
;**  	-----------------------    #pragma MUST_ITERATE(511, 511, 511)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g51:
;** 189	-----------------------    a$12 = *U$226++{6}+ret;  // [5]
;** 190	-----------------------    a$13 = *U$230++{6}+a$12;  // [5]
;** 191	-----------------------    a$14 = *U$234++{6}+a$13;  // [5]
;** 192	-----------------------    a$15 = *U$238++{6}+a$14;  // [5]
;** 193	-----------------------    a$16 = *U$242++{6}+a$15;  // [5]
;** 194	-----------------------    a$17 = *U$246++{6}+a$16;  // [5]
;** 194	-----------------------    ret = a$17;  // [5]
;** 196	-----------------------    *U$248++{6} = ret;  // [5]
;** 197	-----------------------    *U$249++{6} = ret;  // [5]
;** 198	-----------------------    *U$250++{6} = ret;  // [5]
;** 199	-----------------------    *U$251++{6} = ret;  // [5]
;** 200	-----------------------    *U$252++{6} = ret;  // [5]
;** 201	-----------------------    *U$253++{6} = ret;  // [5]
;** 187	-----------------------    if ( L$17 = L$17-1 ) goto g51;  // [5]
;** 204	-----------------------    *s = *U$257;  // [5]
           SUB     .L2     B0,1,B0           ; |183| 

   [ B0]   BNOP    .S1     $C$L48,1          ; |183| 
||         LDW     .D1T1   *A9,A3            ; |204| 

           MV      .L2     B23,B22

           MV      .L1     A16,A4            ; Define a twin register
||         MV      .L2X    A16,B4
||         ADD     .D2     B22,20,B17

           MVK     .S1     0x1ff,A5          ; |187| 
||         ADD     .D1     A4,20,A6
||         ADD     .D2     B22,16,B18
||         ADD     .L2     8,B23,B20
||         ADD     .S2     12,B23,B19
||         ADD     .L1     12,A16,A7

           STW     .D1T1   A3,*A16           ; |204| 
||         SUB     .L1     A5,1,A0
||         ADD     .D2     B4,16,B6
||         ADD     .L2     4,B23,B21
||         ADD     .S1     4,A16,A8
||         ADD     .S2X    8,A16,B7

           ; BRANCHCC OCCURS {$C$L48}        ; |183| 
;** --------------------------------------------------------------------------*
$C$L51:    
;**	-----------------------g53:
;** 514	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B16,*B10          ; |514| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L52:    
;**	-----------------------g54:
;** 147	-----------------------    ret = 0u;  // [4]
;** 151	-----------------------    if ( !_arg1 ) goto g59;  // [4]
;**  	-----------------------    U$288 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 155	-----------------------    L$18 = _arg1;  // [4]
;** 149	-----------------------    d = (volatile unsigned *)0x10f07040u;  // [4]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x10f04040,A16
||         ZERO    .D1     A4                ; |147| 

   [!A0]   B       .S1     $C$L57            ; |151| 
||         EXTU    .S2     B0,19,19,B4

   [ A0]   MVK     .S2     0x2fe,B5          ; |155| 
||         MVKH    .S1     0x10f04040,A16

           MVKL    .S1     0x10f07040,A17
|| [ A0]   SUB     .L2     B5,1,B9
|| [ A0]   MVC     .S2     B5,RILC

           MV      .L1X    B4,A3             ; Define a twin register
           MVKH    .S1     0x10f07040,A17
           ADDAW   .D1     A16,A3,A9
           ; BRANCHCC OCCURS {$C$L57}        ; |151| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L53
;** --------------------------------------------------------------------------*
$C$L53:    
;**	-----------------------g56:
;**  	-----------------------    U$267 = s;
;**  	-----------------------    U$271 = &U$267[1];
;**  	-----------------------    U$275 = &U$267[2];
;**  	-----------------------    U$279 = &U$267[3];
;**  	-----------------------    U$281 = d;
;**  	-----------------------    U$282 = &U$281[1];
;**  	-----------------------    U$283 = &U$281[2];
;**  	-----------------------    U$284 = &U$281[3];
;** 155	-----------------------    L$19 = 766;  // [4]
;**  	-----------------------    #pragma MUST_ITERATE(766, 766, 766)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g57:
;** 157	-----------------------    a$18 = *U$267++{4}+ret;  // [4]
;** 158	-----------------------    a$19 = *U$271++{4}+a$18;  // [4]
;** 159	-----------------------    a$20 = *U$275++{4}+a$19;  // [4]
;** 160	-----------------------    a$21 = *U$279++{4}+a$20;  // [4]
;** 160	-----------------------    ret = a$21;  // [4]
;** 162	-----------------------    *U$281++{4} = ret;  // [4]
;** 163	-----------------------    *U$282++{4} = ret;  // [4]
;** 164	-----------------------    *U$283++{4} = ret;  // [4]
;** 165	-----------------------    *U$284++{4} = ret;  // [4]
;** 155	-----------------------    if ( L$19 = L$19-1 ) goto g57;  // [4]
;** 168	-----------------------    *s = *U$288;  // [4]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 155
;*      Loop opening brace source line   : 156
;*      Loop closing brace source line   : 166
;*      Known Minimum Trip Count         : 766                    
;*      Known Maximum Trip Count         : 766                    
;*      Known Max Trip Count Factor      : 766
;*      Loop Carried Dependency Bound(^) : 12
;*      Unpartitioned Resource Bound     : 4
;*      Partitioned Resource Bound(*)    : 4
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     4*       4*    
;*      .M units                     0        0     
;*      .X cross paths               2        1     
;*      .T address paths             4*       4*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          4        1     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     3        2     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 12 Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L54:    ; PIPED LOOP PROLOG

           SPLOOPD 12      ;24               ; (P) 
||         MV      .L1     A16,A8
||         MVC     .S2     B9,ILC

;** --------------------------------------------------------------------------*
$C$L55:    ; PIPED LOOP KERNEL

           SPMASK          L2
||         ADD     .L2X    4,A16,B6
||         LDW     .D1T1   *A8++(16),A3      ; |157| (P) <0,0>  ^ 

           SPMASK          L2
||         ADD     .L2X    8,A16,B5
||         LDW     .D2T1   *B6++(16),A3      ; |158| (P) <0,1>  ^ 

           SPMASK          L1
||         ADD     .L1     12,A16,A7
||         LDW     .D2T2   *B5++(16),B4      ; |159| (P) <0,2> 

           LDW     .D1T2   *A7++(16),B4      ; |160| (P) <0,3> 
           NOP             1
           ADD     .L1     A4,A3,A4          ; |157| (P) <0,5>  ^ 
           ADD     .L1     A4,A3,A3          ; |158| (P) <0,6>  ^ 
           ADD     .L1X    A3,B4,A3          ; |159| (P) <0,7>  ^ 

           SPMASK          L2
||         MV      .L2X    A17,B8
||         ADD     .L1X    A3,B4,A4          ; |160| (P) <0,8> 

           SPMASK          L1
||         ADD     .L1     4,A17,A6
||         ADD     .L2X    A3,B4,B4          ; |160| (P) <0,9>  ^ Define a twin register

           SPMASK          L1,L2
||         ADD     .L1     12,A17,A5
||         ADD     .L2X    8,A17,B7
||         STW     .D2T1   A4,*B8++(16)      ; |162| (P) <0,10> 
||         STW     .D1T2   B4,*A6++(16)      ; |163| (P) <0,10>  ^ 

           STW     .D2T1   A4,*B7++(16)      ; |164| (P) <0,11>  ^ 
||         STW     .D1T2   B4,*A5++(16)      ; |165| (P) <0,11>  ^ 

           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L56:    ; PIPED LOOP EPILOG
;** 151	-----------------------    if ( L$18 = L$18-1 ) goto g56;  // [4]

           LDW     .D1T1   *A9,A3            ; |168| 
||         SUB     .L2     B0,1,B0           ; |151| 

;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L53,2          ; |151| 
|| [ B0]   MVK     .S2     0x2fe,B5          ; |155| 

   [ B0]   SUB     .L2     B5,1,B9
           STW     .D1T1   A3,*A16           ; |168| 
   [ B0]   MVC     .S2     B5,RILC
           ; BRANCHCC OCCURS {$C$L53}        ; |151| 
;** --------------------------------------------------------------------------*
$C$L57:    
;**	-----------------------g59:
;** 506	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T1   A4,*B10           ; |506| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L58:    
;**	-----------------------g60:
;** 119	-----------------------    ret = 0u;  // [3]
;** 123	-----------------------    if ( !_arg1 ) goto g65;  // [3]
;**  	-----------------------    U$309 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 127	-----------------------    L$20 = _arg1;  // [3]
;** 121	-----------------------    d = (volatile unsigned *)0x10f07040u;  // [3]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x10f04040,A6
||         MVKL    .S2     0x10f07040,B16
||         ZERO    .D2     B6                ; |119| 

   [!A0]   B       .S1     $C$L63            ; |123| 
||         EXTU    .S2     B0,19,19,B4

   [ A0]   MVK     .S2     0x5fc,B9          ; |127| 
||         MVKH    .S1     0x10f04040,A6

           MVKH    .S2     0x10f07040,B16
||         MV      .L1X    B4,A3             ; Define a twin register

   [ A0]   SUB     .L2     B9,1,B4
   [ A0]   MVC     .S2     B9,RILC
           ADDAW   .D1     A6,A3,A5
           ; BRANCHCC OCCURS {$C$L63}        ; |123| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L59
;** --------------------------------------------------------------------------*
$C$L59:    
;**	-----------------------g62:
;**  	-----------------------    U$298 = s;
;**  	-----------------------    U$302 = &U$298[1];
;**  	-----------------------    U$304 = d;
;**  	-----------------------    U$305 = &U$304[1];
;** 127	-----------------------    L$21 = 1532;  // [3]
;**  	-----------------------    #pragma MUST_ITERATE(1532, 1532, 1532)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g63:
;** 129	-----------------------    a$22 = *U$298++{2}+ret;  // [3]
;** 130	-----------------------    a$23 = *U$302++{2}+a$22;  // [3]
;** 130	-----------------------    ret = a$23;  // [3]
;** 132	-----------------------    *U$304++{2} = ret;  // [3]
;** 133	-----------------------    *U$305++{2} = ret;  // [3]
;** 127	-----------------------    if ( L$21 = L$21-1 ) goto g63;  // [3]
;** 136	-----------------------    *s = *U$309;  // [3]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 127
;*      Loop opening brace source line   : 128
;*      Loop closing brace source line   : 134
;*      Known Minimum Trip Count         : 1532                    
;*      Known Maximum Trip Count         : 1532                    
;*      Known Max Trip Count Factor      : 1532
;*      Loop Carried Dependency Bound(^) : 9
;*      Unpartitioned Resource Bound     : 2
;*      Partitioned Resource Bound(*)    : 3
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1        3*    
;*      .M units                     0        0     
;*      .X cross paths               0        1     
;*      .T address paths             1        3*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          0        2     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1        2     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 9  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L60:    ; PIPED LOOP PROLOG

           SPLOOPD 9       ;18               ; (P) 
||         MV      .L2X    A6,B4
||         MVC     .S2     B4,ILC

;** --------------------------------------------------------------------------*
$C$L61:    ; PIPED LOOP KERNEL

           SPMASK          L1
||         ADD     .L1     4,A6,A4
||         LDW     .D2T2   *B4++(8),B5       ; |129| (P) <0,0>  ^ 

           LDW     .D1T1   *A4++(8),A3       ; |130| (P) <0,1>  ^ 
           NOP             3
           ADD     .L2     B6,B5,B5          ; |129| (P) <0,5>  ^ 

           SPMASK          S2
||         MV      .S2     B16,B8
||         ADD     .L2X    B5,A3,B6          ; |130| (P) <0,6>  ^ 

           SPMASK          L2
||         ADD     .L2     4,B16,B7
||         STW     .D2T2   B6,*B8++(8)       ; |132| (P) <0,7>  ^ 

           STW     .D2T2   B6,*B7++(8)       ; |133| (P) <0,8>  ^ 
           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L62:    ; PIPED LOOP EPILOG
;** 123	-----------------------    if ( L$20 = L$20-1 ) goto g62;  // [3]

           LDW     .D1T1   *A5,A3            ; |136| 
||         SUB     .L2     B0,1,B0           ; |123| 

;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L59,2          ; |123| 
|| [ B0]   MVK     .S2     0x5fc,B9          ; |127| 

   [ B0]   SUB     .L2     B9,1,B4
           STW     .D1T1   A3,*A6            ; |136| 
   [ B0]   MVC     .S2     B9,RILC
           ; BRANCHCC OCCURS {$C$L59}        ; |123| 
;** --------------------------------------------------------------------------*
$C$L63:    
;**	-----------------------g65:
;** 498	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B6,*B10           ; |498| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L64:    
;**	-----------------------g66:
;** 90	-----------------------    ret = 0u;  // [2]
;** 97	-----------------------    if ( !_arg1 ) goto g71;  // [2]
;**  	-----------------------    U$326 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 101	-----------------------    L$22 = _arg1;  // [2]
;** 92	-----------------------    d = (volatile unsigned *)0x10f07040u;  // [2]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MV      .L1     A10,A0
||         MV      .L2X    A10,B0
||         MVKL    .S1     0x10f04040,A6
||         MVKL    .S2     0x10f07040,B8

   [!A0]   B       .S1     $C$L69            ; |97| 
||         EXTU    .S2     B0,19,19,B4

   [ A0]   MVK     .S2     0xbf8,B7          ; |101| 
||         MVKH    .S1     0x10f04040,A6

   [ A0]   SUB     .L2     B7,1,B6
||         MVKH    .S2     0x10f07040,B8
||         MV      .L1X    B4,A3             ; Define a twin register

           ZERO    .L2     B4                ; |90| 
   [ A0]   MVC     .S2     B7,RILC
           ADDAW   .D1     A6,A3,A5
           ; BRANCHCC OCCURS {$C$L69}        ; |97| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L65
;** --------------------------------------------------------------------------*
$C$L65:    
;**	-----------------------g68:
;**  	-----------------------    U$319 = s;
;**  	-----------------------    U$322 = d;
;** 101	-----------------------    L$23 = 3064;  // [2]
;**  	-----------------------    #pragma MUST_ITERATE(3064, 3064, 3064)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g69:
;** 103	-----------------------    c = *U$319++;  // [2]
;** 104	-----------------------    ret += c;  // [2]
;** 105	-----------------------    *U$322++ = ret;  // [2]
;** 101	-----------------------    if ( L$23 = L$23-1 ) goto g69;  // [2]
;** 108	-----------------------    *s = *U$326;  // [2]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 101
;*      Loop opening brace source line   : 102
;*      Loop closing brace source line   : 106
;*      Known Minimum Trip Count         : 3064                    
;*      Known Maximum Trip Count         : 3064                    
;*      Known Max Trip Count Factor      : 3064
;*      Loop Carried Dependency Bound(^) : 7
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               1*       0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          1        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 7  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L66:    ; PIPED LOOP PROLOG

           SPLOOPD 7       ;14               ; (P) 
||         MV      .L1     A6,A4
||         MVC     .S2     B6,ILC

;** --------------------------------------------------------------------------*
$C$L67:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A4++,A3          ; |103| (P) <0,0>  ^ 
           NOP             4

           SPMASK          S2
||         MV      .S2     B8,B5
||         ADD     .L2X    A3,B4,B4          ; |104| (P) <0,5>  ^ 

           STW     .D2T2   B4,*B5++          ; |105| (P) <0,6>  ^ 
           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L68:    ; PIPED LOOP EPILOG
;** 97	-----------------------    if ( L$22 = L$22-1 ) goto g68;  // [2]

           LDW     .D1T1   *A5,A3            ; |108| 
||         SUB     .L2     B0,1,B0           ; |97| 

;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L65,2          ; |97| 
|| [ B0]   MVK     .S2     0xbf8,B7          ; |101| 

   [ B0]   SUB     .L2     B7,1,B6
           STW     .D1T1   A3,*A6            ; |108| 
   [ B0]   MVC     .S2     B7,RILC
           ; BRANCHCC OCCURS {$C$L65}        ; |97| 
;** --------------------------------------------------------------------------*
$C$L69:    
;**	-----------------------g71:
;** 490	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T2   B4,*B10           ; |490| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L70:    
;**	-----------------------g72:
;** 68	-----------------------    if ( !_arg1 ) goto g77;  // [1]
;** 71	-----------------------    L$24 = _arg1;  // [1]
;** 66	-----------------------    d = (volatile unsigned *)0x10f04040u;  // [1]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)

           MVKL    .S1     0x10f04040,A6
||         MV      .L1     A10,A0            ; |66| 
||         MV      .L2X    A10,B0            ; |66| 

           MVKH    .S1     0x10f04040,A6
|| [!A0]   B       .S2     $C$L75            ; |68| 

   [ A0]   LDW     .D1T2   *A6,B4            ; |71| 
   [ A0]   MVK     .S2     0xbf8,B7          ; |73| 
   [ A0]   SUB     .L2     B7,2,B6
   [ A0]   MVC     .S2     B7,RILC
   [ A0]   MVC     .S2     B6,ILC
           ; BRANCHCC OCCURS {$C$L75}        ; |68| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L71
;** --------------------------------------------------------------------------*
$C$L71:    
;**	-----------------------g74:
;** 71	-----------------------    k = *d;  // [1]
;**  	-----------------------    U$335 = d;
;**  	-----------------------    U$339 = &U$335[1];
;** 73	-----------------------    L$25 = 3064;  // [1]
;**  	-----------------------    #pragma MUST_ITERATE(3064, 3064, 3064)
;**  	-----------------------    // LOOP BELOW UNROLLED BY FACTOR(2)
;**  	-----------------------    #pragma LOOP_FLAGS(4098u)
;**	-----------------------g75:
;** 75	-----------------------    *U$335++{2} = k;  // [1]
;** 75	-----------------------    *U$339++{2} = k+1u;  // [1]
;** 75	-----------------------    k += 2u;  // [1]
;** 73	-----------------------    if ( L$25 = L$25-1 ) goto g75;  // [1]
;** 68	-----------------------    if ( L$24 = L$24-1 ) goto g74;  // [1]
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 73
;*      Loop opening brace source line   : 74
;*      Loop closing brace source line   : 76
;*      Loop Unroll Multiple             : 2x
;*      Known Minimum Trip Count         : 3064                    
;*      Known Maximum Trip Count         : 3064                    
;*      Known Max Trip Count Factor      : 3064
;*      Loop Carried Dependency Bound(^) : 2
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       1*    
;*      .M units                     0        0     
;*      .X cross paths               0        0     
;*      .T address paths             1*       1*    
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          2        1     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       1*    
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 2  Schedule found with 2 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1 (after unrolling)
;*----------------------------------------------------------------------------*
$C$L72:    ; PIPED LOOP PROLOG

           SPLOOPD 2       ;4                ; (P) 
||         MV      .L2X    A6,B5
||         MV      .L1X    B4,A3

;** --------------------------------------------------------------------------*
$C$L73:    ; PIPED LOOP KERNEL

           SPMASK          S1
||         ADD     .S1     4,A6,A5
||         ADD     .L1     1,A3,A4           ; |75| (P) <0,0> 
||         STW     .D2T2   B4,*B5++(8)       ; |75| (P) <0,0>  ^ 

           ADD     .L1     2,A3,A3           ; |75| (P) <0,1> 
||         ADD     .L2     2,B4,B4           ; |75| (P) <0,1> Define a twin register
||         STW     .D1T1   A4,*A5++(8)       ; |75| (P) <0,1>  ^ 

           SPKERNEL 0,0
;** --------------------------------------------------------------------------*
$C$L74:    ; PIPED LOOP EPILOG
           SUB     .L2     B0,1,B0           ; |68| 
;** --------------------------------------------------------------------------*

   [ B0]   BNOP    .S1     $C$L71,2          ; |68| 
|| [ B0]   LDW     .D1T2   *A6,B4            ; |71| 
|| [ B0]   MVK     .S2     0xbf8,B7          ; |73| 

   [ B0]   MVC     .S2     B7,RILC
   [ B0]   SUB     .L2     B7,2,B6
   [ B0]   MVC     .S2     B6,ILC
           ; BRANCHCC OCCURS {$C$L71}        ; |68| 
;** --------------------------------------------------------------------------*
$C$L75:    
;**	-----------------------g77:
;** 482	-----------------------    *_ret1 = 0u;
;** 578	-----------------------    return 0u;
           ZERO    .L2     B4                ; |482| 
           STW     .D2T2   B4,*B10           ; |482| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |482| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;** --------------------------------------------------------------------------*
$C$L76:    
;**	-----------------------g78:
;** 42	-----------------------    ret = 0u;  // [0]
;** 45	-----------------------    if ( !_arg1 ) goto g83;  // [0]
;**  	-----------------------    U$357 = _extu(_arg1, 19u, 19u)*4+(s = (volatile unsigned *)0x10f04040u);
;** 49	-----------------------    L$26 = _arg1;  // [0]
;**  	-----------------------    #pragma MUST_ITERATE(1, 2147483647, 1)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
           MV      .L1     A10,A0
   [!A0]   B       .S1     $C$L81            ; |45| 
           MV      .L2X    A10,B0
           EXTU    .S2     B0,19,19,B4
           MVKL    .S1     0x10f04040,A7

           MV      .L1X    B4,A3             ; Define a twin register
||         MVKH    .S1     0x10f04040,A7

           ADDAW   .D1     A7,A3,A6
||         ZERO    .L1     A4                ; |42| 

           ; BRANCHCC OCCURS {$C$L81}        ; |45| 
;** --------------------------------------------------------------------------*
;**   BEGIN LOOP $C$L77
;** --------------------------------------------------------------------------*
$C$L77:    
;**	-----------------------g80:
;**  	-----------------------    U$352 = s;
;** 49	-----------------------    L$27 = 6128;  // [0]
;**  	-----------------------    #pragma MUST_ITERATE(6128, 6128, 6128)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g81:
;** 51	-----------------------    a$25 = *U$352+++ret;  // [0]
;** 51	-----------------------    ret = a$25;  // [0]
;** 49	-----------------------    if ( L$27 = L$27-1 ) goto g81;  // [0]
;** 54	-----------------------    *s = *U$357;  // [0]
           MVK     .S2     0x17f0,B5         ; |49| 

           SUB     .L2     B5,4,B4
||         MVC     .S2     B5,RILC

;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*
;*      Loop found in file               : sramtest.c
;*      Loop source line                 : 49
;*      Loop opening brace source line   : 50
;*      Loop closing brace source line   : 52
;*      Known Minimum Trip Count         : 6128                    
;*      Known Maximum Trip Count         : 6128                    
;*      Known Max Trip Count Factor      : 6128
;*      Loop Carried Dependency Bound(^) : 0
;*      Unpartitioned Resource Bound     : 1
;*      Partitioned Resource Bound(*)    : 1
;*      Resource Partition:
;*                                A-side   B-side
;*      .L units                     0        0     
;*      .S units                     0        0     
;*      .D units                     1*       0     
;*      .M units                     0        0     
;*      .X cross paths               0        0     
;*      .T address paths             1*       0     
;*      Long read paths              0        0     
;*      Long write paths             0        0     
;*      Logical  ops (.LS)           0        0     (.L or .S unit)
;*      Addition ops (.LSD)          1        0     (.L or .S or .D unit)
;*      Bound(.L .S .LS)             0        0     
;*      Bound(.L .S .D .LS .LSD)     1*       0     
;*
;*      Searching for software pipeline schedule at ...
;*         ii = 1  Schedule found with 6 iterations in parallel
;*      Done
;*
;*      Loop will be splooped
;*      Collapsed epilog stages       : 0
;*      Collapsed prolog stages       : 0
;*      Minimum required memory pad   : 0 bytes
;*
;*      Minimum safe trip count       : 1
;*----------------------------------------------------------------------------*
$C$L78:    ; PIPED LOOP PROLOG

           SPLOOPD 1       ;6                ; (P) 
||         MV      .L1     A7,A5
||         MVC     .S2     B4,ILC

;** --------------------------------------------------------------------------*
$C$L79:    ; PIPED LOOP KERNEL
           LDW     .D1T1   *A5++,A3          ; |51| (P) <0,0> 
           NOP             3
           NOP             1

           SPKERNEL 0,0
||         ADD     .L1     A4,A3,A4          ; |51| <0,5> 

;** --------------------------------------------------------------------------*
$C$L80:    ; PIPED LOOP EPILOG
;** 45	-----------------------    if ( L$26 = L$26-1 ) goto g80;  // [0]

           LDW     .D1T1   *A6,A5            ; |54| 
||         SUB     .L2     B0,1,B0           ; |45| 

   [ B0]   BNOP    .S2     $C$L77,3          ; |45| 
           STW     .D1T1   A5,*A7            ; |54| 
           NOP             1
           ; BRANCHCC OCCURS {$C$L77}        ; |45| 
;** --------------------------------------------------------------------------*
$C$L81:    
;**	-----------------------g83:
;** 474	-----------------------    *_ret1 = ret;
;** 578	-----------------------    return 0u;
           STW     .D2T1   A4,*B10           ; |474| 
           LDW     .D2T2   *++SP(8),B3       ; |579| 
           LDDW    .D2T1   *++SP,A11:A10     ; |579| 
           LDDW    .D2T1   *++SP,A13:A12     ; |579| 
           LDDW    .D2T2   *++SP,B11:B10     ; |579| 
           LDDW    .D2T2   *++SP,B13:B12     ; |579| 
           RET     .S2     B3                ; |579| 
           LDW     .D2T1   *++SP(8),A14      ; |579| 
           ZERO    .L1     A4                ; |578| 
           NOP             3
           ; BRANCH OCCURS {B3}              ; |579| 
;; Inlined function references:
;; [  0] loc_l1sram_r_speed
;; [  1] loc_l1sram_w_speed
;; [  2] loc_l1sram_rw_speed
;; [  3] loc_l1sram_r2w2_speed
;; [  4] loc_l1sram_r4w4_speed
;; [  5] loc_l1sram_r6w6_speed
;; [  6] loc_l1sram_r8w8_speed
;; [  7] loc_l1sram_rwram_speed
;; [  8] loc_l1sram_rwramdma_speed
;; [  9] loc_l2sram_r_speed
;; [ 10] loc_l2sram_rw_speed
;; [ 11] loc_l2sram_r1_w2_speed
;; [ 12] loc_l2sram_r1_w2ramdma_speed

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_needed(0)
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_preserved(0)
	.battr "TI", Tag_File, 1, Tag_Tramps_Use_SOC(1)
