;******************************************************************************
;* TMS320C6x C/C++ Codegen                                        Unix v7.4.5 *
;* Date/Time created: Wed Dec 11 20:04:10 2013                                *
;******************************************************************************
	.compiler_opts --abi=coffabi --c64p_l1d_workaround=default --endian=little --hll_source=on --long_precision_bits=40 --mem_model:code=near --mem_model:const=data --mem_model:data=far_aggregates --object_format=coff --silicon_version=6500 --symdebug:none 

;******************************************************************************
;* GLOBAL FILE PARAMETERS                                                     *
;*                                                                            *
;*   Architecture      : TMS320C64x+                                          *
;*   Optimization      : Enabled at level 0                                   *
;*   Optimizing for    : Compile time, Ease of Development                    *
;*                       Based on options: -o0, no -ms                        *
;*   Endian            : Little                                               *
;*   Interrupt Thrshld : Disabled                                             *
;*   Data Access Model : Far Aggregate Data                                   *
;*   Pipelining        : Disabled                                             *
;*   Memory Aliases    : Presume are aliases (pessimistic)                    *
;*   Debug Info        : No Debug Info                                        *
;*                                                                            *
;******************************************************************************

	.asg	A15, FP
	.asg	B14, DP
	.asg	B15, SP
	.global	$bss

_qdma:	.usect	".far",128,8
;	opt6x /tmp/149235WwExk /tmp/14923fXU7hF 
	.sect	".text"
	.clink
	.global	_qdma_init

;******************************************************************************
;* FUNCTION NAME: qdma_init                                                   *
;*                                                                            *
;*   Regs Modified     : A3,A4,A5,A6,A7,A8,A9,B0,B4,B5,B6,B7,B8,B9,A16,A17,   *
;*                           A27,A28,A29,A30,A31,B16,B17,B29,B30,B31          *
;*   Regs Used         : A3,A4,A5,A6,A7,A8,A9,B0,B3,B4,B5,B6,B7,B8,B9,A16,A17,*
;*                           A27,A28,A29,A30,A31,B16,B17,B29,B30,B31          *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_init:
;** --------------------------------------------------------------------------*
;** 68	-----------------------    (qdma[0]).event_queue_nr = 1u;
;** 69	-----------------------    (qdma[0]).param_nr = 0u;
;** 70	-----------------------    (qdma[0]).tcc_nr = 0u;
;** 73	-----------------------    ((unsigned *)qdma)[4] = 1u;
;** 74	-----------------------    ((unsigned *)qdma)[5] = 1u;
;** 75	-----------------------    ((unsigned *)qdma)[6] = 1u;
;** 78	-----------------------    ((unsigned *)qdma)[8] = 1u;
;** 79	-----------------------    ((unsigned *)qdma)[9] = 2u;
;** 80	-----------------------    ((unsigned *)qdma)[10] = 2u;
;** 83	-----------------------    ((unsigned *)qdma)[12] = 1u;
;** 84	-----------------------    ((unsigned *)qdma)[13] = 3u;
;** 85	-----------------------    ((unsigned *)qdma)[14] = 3u;
;** 88	-----------------------    ((unsigned *)qdma)[16] = 1u;
;** 89	-----------------------    ((unsigned *)qdma)[17] = 4u;
;** 90	-----------------------    ((unsigned *)qdma)[18] = 4u;
;** 93	-----------------------    ((unsigned *)qdma)[20] = 1u;
;** 94	-----------------------    ((unsigned *)qdma)[21] = 5u;
;** 95	-----------------------    ((unsigned *)qdma)[22] = 5u;
;** 98	-----------------------    ((unsigned *)qdma)[24] = 1u;
;** 99	-----------------------    ((unsigned *)qdma)[25] = 6u;
;** 100	-----------------------    ((unsigned *)qdma)[26] = 6u;
;** 103	-----------------------    ((unsigned *)qdma)[28] = 1u;
;** 104	-----------------------    ((unsigned *)qdma)[29] = 7u;
;** 105	-----------------------    ((unsigned *)qdma)[30] = 7u;
;** 110	-----------------------    if ( (qdmaNr = 0u) >= 8u ) goto g3;
           MVKL    .S2     _qdma,B5
           MVKH    .S2     _qdma,B5
           MVK     .L2     1,B4              ; |68| 
           ADD     .L1X    4,B5,A6
           ADDAW   .D1     A6,3,A4

           STW     .D2T2   B4,*B5            ; |68| 
||         ZERO    .L1     A5                ; |70| 

           ADD     .L2X    4,A4,B8
||         STW     .D1T1   A5,*A6            ; |69| 
||         MVK     .L1     1,A5              ; |73| 

           ADDAW   .D2     B8,3,B6
||         ADD     .L2     8,B5,B7
||         ZERO    .S2     B4                ; |70| 

           STW     .D2T2   B4,*B7            ; |70| 
||         MV      .L2X    A5,B4             ; |70| 

           ADD     .L1     8,A4,A3
||         STW     .D1T1   A5,*A4            ; |73| 
||         STW     .D2T2   B4,*B8            ; |74| 
||         MV      .S1     A5,A4             ; |73| 

           ADD     .L1X    4,B6,A7
||         STW     .D1T1   A4,*A3            ; |75| 

           ADDAW   .D1     A7,3,A4
           MVK     .L1     2,A5              ; |79| 

           ADD     .L2     8,B6,B5
||         STW     .D2T2   B4,*B6            ; |78| 
||         ADD     .S2X    4,A4,B6

           MV      .L2X    A5,B4             ; |78| 
||         MVK     .S2     3,B7              ; |84| 
||         ADDAW   .D2     B6,3,B8

           STW     .D1T1   A5,*A7            ; |79| 
||         STW     .D2T2   B4,*B5            ; |80| 
||         MVK     .L1     1,A6              ; |83| 

           STW     .D1T1   A6,*A4            ; |83| 
||         STW     .D2T2   B7,*B6            ; |84| 
||         ADD     .L1     8,A4,A3
||         MV      .S1X    B7,A4             ; |83| 

           STW     .D1T1   A4,*A3            ; |85| 
||         MVK     .L1     4,A3              ; |89| 
||         ADD     .S1X    4,B8,A5

           MV      .L2X    A6,B6             ; |84| 
||         ADDAW   .D1     A5,3,A7

           STW     .D2T2   B6,*B8            ; |88| 
||         MV      .L2X    A3,B6             ; |88| 
||         ADD     .S2     8,B8,B4

           STW     .D1T1   A3,*A5            ; |89| 
||         STW     .D2T2   B6,*B4            ; |90| 
||         MVK     .L2     5,B4              ; |94| 
||         ADD     .S2X    4,A7,B9

           MV      .L1     A6,A3             ; |89| 
||         ADDAW   .D2     B9,3,B5

           STW     .D1T1   A3,*A7            ; |93| 
||         STW     .D2T2   B4,*B9            ; |94| 
||         MV      .L1X    B4,A3             ; |93| 
||         ADD     .S1     8,A7,A8

           STW     .D1T1   A3,*A8            ; |95| 
||         MVK     .L1     6,A3              ; |99| 

           ADD     .L1X    4,B5,A9
||         MV      .L2X    A6,B4             ; |94| 

           ADDAW   .D1     A9,3,A16
||         STW     .D2T2   B4,*B5            ; |98| 
||         MV      .L2X    A3,B4             ; |98| 
||         ADD     .S2     8,B5,B16

           STW     .D1T1   A3,*A9            ; |99| 
||         STW     .D2T2   B4,*B16           ; |100| 
||         MVK     .L2     7,B4              ; |104| 

           ADD     .L2X    4,A16,B17
||         MV      .L1     A6,A3             ; |99| 

           STW     .D1T1   A3,*A16           ; |103| 
||         STW     .D2T2   B4,*B17           ; |104| 
||         MV      .L1X    B4,A3             ; |103| 
||         ZERO    .L2     B4                ; |110| 

           CMPLTU  .L2     B4,8,B0           ; |110| 
   [!B0]   BNOP    .S1     $C$L2,3           ; |110| 
           ADD     .L1     8,A16,A17
           STW     .D1T1   A3,*A17           ; |105| 
           ; BRANCHCC OCCURS {$C$L2}         ; |110| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Software pipelining disabled
;*----------------------------------------------------------------------------*
$C$L1:    
;**	-----------------------g2:
;** 113	-----------------------    *((volatile unsigned *)(qdmaNr*4u)+0x1c00200u) = (*((qdmaNr<<4)+&qdma)).param_nr<<5|0x1cu;
;** 116	-----------------------    *(volatile unsigned *)0x1c00260u = (*((qdmaNr<<4)+&qdma)).event_queue_nr<<qdmaNr*4u;
;** 119	-----------------------    *(volatile unsigned *)0x1c01070u = 1u<<(*((qdmaNr<<4)+&qdma)).tcc_nr;
;** 122	-----------------------    *(volatile unsigned *)0x1c0108cu = 1u<<qdmaNr;
;** 124	-----------------------    (*((qdmaNr<<4)+&qdma)).b_running = 0;
;** 110	-----------------------    if ( (++qdmaNr) < 8u ) goto g2;

           SUBAW   .D1     A17,29,A3
||         SHL     .S2     B4,4,B5           ; |113| 
||         SHL     .S1X    B4,4,A31          ; |116| 
||         ZERO    .L1     A27               ; |124| 

           MVKL    .S2     0x1c00200,B6
||         MVKL    .S1     0x1c00260,A4
||         SUBAW   .D1     A17,28,A5

           ADD     .L2X    A3,B5,B5          ; |113| 
||         MVKH    .S2     0x1c00200,B6
||         MVKH    .S1     0x1c00260,A4

           SHL     .S2     B4,2,B31          ; |116| 
||         SHL     .S1X    B4,4,A28          ; |124| 
||         LDW     .D2T2   *B5,B5            ; |113| 

           ADD     .L2X    -4,A3,B7
           ADDAW   .D2     B7,3,B29
           MVKL    .S1     0x1c01070,A29
           ADD     .L1X    B7,A31,A3         ; |116| 
           SHL     .S2     B5,5,B5           ; |113| 
           SET     .S2     B5,2,4,B5         ; |113| 
           STW     .D2T2   B5,*+B6[B4]       ; |113| 
           LDW     .D1T1   *A3,A3            ; |116| 
           SHL     .S2     B4,4,B30          ; |119| 
           MVKH    .S1     0x1c01070,A29
           ADDAW   .D1     A29,7,A6
           ADD     .L1X    A5,B30,A30        ; |119| 
           SHL     .S2X    A3,B31,B5         ; |116| 

           STW     .D1T2   B5,*A4            ; |116| 
||         MVK     .L1     1,A4              ; |119| 

           LDW     .D1T1   *A30,A3           ; |119| 
           MV      .L2X    A4,B5             ; |119| 

           SHL     .S2     B5,B4,B5          ; |122| 
||         ADD     .L2     1,B4,B4           ; |110| 

           CMPLTU  .L2     B4,8,B0           ; |110| 
   [ B0]   B       .S1     $C$L1             ; |110| 
           SHL     .S1     A4,A3,A3          ; |119| 
           ADD     .L1X    B29,A28,A4        ; |124| 
           STW     .D1T1   A3,*A29           ; |119| 
           STW     .D1T2   B5,*A6            ; |122| 
           STW     .D1T1   A27,*A4           ; |124| 
           ; BRANCHCC OCCURS {$C$L1}         ; |110| 
;** --------------------------------------------------------------------------*
$C$L2:    
;**	-----------------------g3:
;** 129	-----------------------    *(volatile unsigned *)0x1c00280u = 16u;
;** 134	-----------------------    *(volatile unsigned *)0x1c00284u = 0u;
;** 140	-----------------------    *(volatile unsigned *)0x1c10140u = 0u;
;** 141	-----------------------    *(volatile unsigned *)0x1c10540u = 0u;
;**  	-----------------------    return;
           MVKL    .S2     0x1c00280,B5
           MVKL    .S2     0x1c10140,B6
           RET     .S2     B3                ; |153| 
           MVKH    .S2     0x1c00280,B5
           MVK     .S2     16,B4             ; |129| 

           ZERO    .L2     B4                ; |140| 
||         STW     .D2T2   B4,*B5            ; |129| 
||         ADD     .L1X    4,B5,A5
||         MVKH    .S2     0x1c10140,B6
||         MVKL    .S1     0x1c10540,A3
||         ZERO    .D1     A4                ; |140| 

           STW     .D2T2   B4,*B6            ; |140| 
||         STW     .D1T1   A4,*A5            ; |134| 
||         MVKH    .S1     0x1c10540,A3

           STW     .D1T1   A4,*A3            ; |141| 
           ; BRANCH OCCURS {B3}              ; |153| 
	.sect	".text"
	.clink
	.global	_qdma_wait

;******************************************************************************
;* FUNCTION NAME: qdma_wait                                                   *
;*                                                                            *
;*   Regs Modified     : A0,A3,A4,B0,B4,B5,B6,B7,A30,A31,B31                  *
;*   Regs Used         : A0,A3,A4,B0,B3,B4,B5,B6,B7,A30,A31,B31               *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_wait:
;** --------------------------------------------------------------------------*
;** 160	-----------------------    if ( !(*((_qdmaNr<<4)+&qdma)).b_running ) goto g5;
           MV      .L2X    A4,B4             ; |157| 

           SHL     .S2     B4,4,B5           ; |160| 
||         MVKL    .S1     _qdma+12,A3

           MVKH    .S1     _qdma+12,A3
           ADD     .L1X    A3,B5,A3          ; |160| 
           LDW     .D1T1   *A3,A0            ; |160| 
           NOP             4
   [!A0]   BNOP    .S1     $C$L4,5           ; |160| 
           ; BRANCHCC OCCURS {$C$L4}         ; |160| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Software pipelining disabled
;*----------------------------------------------------------------------------*
$C$L3:    
;**	-----------------------g3:
;** 162	-----------------------    if ( !(*(volatile unsigned *)0x1c01068u&1<<(*((_qdmaNr<<4)+&qdma)).tcc_nr) ) goto g3;

           MVKL    .S1     _qdma+8,A3
||         SHL     .S2     B4,4,B6           ; |162| 

           MVKH    .S1     _qdma+8,A3
||         MVKL    .S2     0x1c01068,B5

           MVKH    .S2     0x1c01068,B5
           ADD     .L2X    A3,B6,B6          ; |162| 
           LDW     .D2T2   *B6,B6            ; |162| 
           LDW     .D2T2   *B5,B5            ; |162| 
           MVK     .L2     1,B7              ; |162| 
           NOP             2
           SHL     .S2     B7,B6,B6          ; |162| 
           AND     .L2     B6,B5,B0          ; |162| 

   [ B0]   SHL     .S2     B4,4,B5           ; |168| 
|| [!B0]   B       .S1     $C$L3             ; |162| 

   [ B0]   ADD     .L2X    A3,B5,B5          ; |168| 
   [ B0]   LDW     .D2T2   *B5,B7            ; |168| 
   [ B0]   SHL     .S2     B4,4,B6           ; |170| 
   [ B0]   MVK     .L2     1,B4              ; |168| 
   [ B0]   MVKL    .S2     0x1c01070,B31
           ; BRANCHCC OCCURS {$C$L3}         ; |162| 
;** --------------------------------------------------------------------------*
;** 168	-----------------------    *(volatile unsigned *)0x1c01070u = 1u<<(*((_qdmaNr<<4)+&qdma)).tcc_nr;
;** 170	-----------------------    (*((_qdmaNr<<4)+&qdma)).b_running = 0;
;**	-----------------------g5:
;**  	-----------------------    return;

           ADD     .L1     4,A3,A31
||         MVKH    .S2     0x1c01070,B31
||         ZERO    .S1     A30               ; |170| 

           SHL     .S2     B4,B7,B4          ; |168| 
||         ADD     .L1X    A31,B6,A4         ; |170| 

           STW     .D2T2   B4,*B31           ; |168| 
           STW     .D1T1   A30,*A4           ; |170| 
;** --------------------------------------------------------------------------*
$C$L4:    
           RETNOP  .S2     B3,5              ; |172| 
           ; BRANCH OCCURS {B3}              ; |172| 
	.sect	".text"
	.clink
	.global	_qdma_copy1d

;******************************************************************************
;* FUNCTION NAME: qdma_copy1d                                                 *
;*                                                                            *
;*   Regs Modified     : A3,A5,A7,B4,B5,B6,B7,B8                              *
;*   Regs Used         : A3,A4,A5,A6,A7,B3,B4,B5,B6,B7,B8                     *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_copy1d:
;** --------------------------------------------------------------------------*
;** 178	-----------------------    param = (volatile unsigned *)((*((_qdmaNr<<4)+&qdma)).param_nr<<5)+0x1c04000u;
;** 180	-----------------------    *param = (*((_qdmaNr<<4)+&qdma)).tcc_nr<<12|0x100008u;
;** 191	-----------------------    param[1] = (unsigned)_src;
;** 193	-----------------------    param[2] = _numBytes&0xffffu|0x10000u;
;** 199	-----------------------    param[3] = (unsigned)_dst;
;** 201	-----------------------    param[4] = 0u;
;** 203	-----------------------    param[5] = 65535u;
;** 209	-----------------------    param[6] = 0u;
;** 211	-----------------------    param[7] = 1u;
;** 213	-----------------------    (*((_qdmaNr<<4)+&qdma)).b_running = 1;
;**  	-----------------------    return;
           MVKL    .S1     _qdma+4,A5
           MVKH    .S1     _qdma+4,A5
           SHL     .S2X    A4,4,B5           ; |180| 

           SHL     .S1     A4,4,A3           ; |178| 
||         ADD     .L2X    4,A5,B7

           ADD     .L1     A5,A3,A3          ; |178| 
||         ADD     .L2     B7,B5,B5          ; |180| 

           LDW     .D1T1   *A3,A3            ; |178| 
||         LDW     .D2T2   *B5,B5            ; |180| 

           MVKL    .S1     0x100008,A7
           MVKL    .S2     0x1c04000,B7
           MVKH    .S1     0x100008,A7
           MVKH    .S2     0x1c04000,B7

           SHL     .S1     A3,5,A5           ; |178| 
||         MV      .L1X    B6,A3             ; |176| 
||         SHL     .S2     B5,12,B6          ; |180| 

           OR      .L2X    A7,B6,B6          ; |180| 
           ADD     .L2X    B7,A5,B5          ; |178| 

           STW     .D2T2   B6,*B5            ; |180| 
||         EXTU    .S1     A3,16,16,A3       ; |193| 

           STW     .D2T1   A6,*+B5(4)        ; |191| 
||         SET     .S1     A3,16,16,A3       ; |193| 

           STW     .D2T1   A3,*+B5(8)        ; |193| 

           ZERO    .L2     B8
||         RET     .S2     B3                ; |214| 
||         STW     .D2T2   B4,*+B5(12)       ; |199| 
||         ZERO    .L1     A3                ; |201| 

           SET     .S2     B8,0x0,0xf,B8
||         STW     .D2T1   A3,*+B5(16)       ; |201| 

           STW     .D2T2   B8,*+B5(20)       ; |203| 
||         SHL     .S1     A4,4,A3           ; |213| 
||         ZERO    .L2     B4                ; |209| 
||         MVKL    .S2     _qdma+12,B7

           STW     .D2T2   B4,*+B5(24)       ; |209| 
||         MVK     .L2     1,B4              ; |211| 
||         MVKH    .S2     _qdma+12,B7

           STW     .D2T2   B4,*+B5(28)       ; |211| 
||         ADD     .L2X    B7,A3,B5          ; |213| 

           STW     .D2T2   B4,*B5            ; |213| 
           ; BRANCH OCCURS {B3}              ; |214| 
	.sect	".text"
	.clink
	.global	_qdma_copy2d

;******************************************************************************
;* FUNCTION NAME: qdma_copy2d                                                 *
;*                                                                            *
;*   Regs Modified     : A3,A5,B4,B5,B7,B8,B9,B16                             *
;*   Regs Used         : A3,A4,A5,A6,A8,A10,B3,B4,B5,B6,B7,B8,B9,B16          *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_copy2d:
;** --------------------------------------------------------------------------*
;** 224	-----------------------    param = (volatile unsigned *)((*((_qdmaNr<<4)+&qdma)).param_nr<<5)+0x1c04000u;
;** 226	-----------------------    *param = (*((_qdmaNr<<4)+&qdma)).tcc_nr<<12|0x10000cu;
;** 237	-----------------------    param[1] = (unsigned)_src;
;** 239	-----------------------    param[2] = _numY<<16|_numBytesX&0xffffu;
;** 245	-----------------------    param[3] = (unsigned)_dst;
;** 247	-----------------------    param[4] = (unsigned)(_dstPitch<<16)|(unsigned)_srcPitch&0xffffu;
;** 251	-----------------------    param[5] = 65535u;
;** 257	-----------------------    param[6] = 0u;
;** 259	-----------------------    param[7] = 1u;
;** 261	-----------------------    (*((_qdmaNr<<4)+&qdma)).b_running = 1;
;**  	-----------------------    return;
           MVKL    .S1     _qdma+4,A5
           MVKH    .S1     _qdma+4,A5
           SHL     .S1     A4,4,A3           ; |224| 

           ADD     .L2X    4,A5,B5
||         ADD     .L1     A5,A3,A5          ; |224| 

           SHL     .S1     A4,4,A3           ; |226| 
||         LDW     .D1T1   *A5,A5            ; |224| 

           MVKL    .S2     0x1c04000,B7
           ADD     .L2X    B5,A3,B5          ; |226| 
           LDW     .D2T2   *B5,B5            ; |226| 
           MVKH    .S2     0x1c04000,B7
           SHL     .S1     A5,5,A3           ; |224| 
           MVKL    .S1     0x10000c,A5
           MVKH    .S1     0x10000c,A5

           ADD     .L2X    B7,A3,B5          ; |224| 
||         SHL     .S2     B5,12,B7          ; |226| 

           OR      .L2X    A5,B7,B7          ; |226| 

           STW     .D2T2   B7,*B5            ; |226| 
||         MV      .L1X    B8,A3             ; |222| 

           STW     .D2T2   B6,*+B5(4)        ; |237| 
||         PACK2   .L1     A10,A3,A3         ; |239| 

           MV      .L2X    A6,B8             ; |222| 
||         STW     .D2T1   A3,*+B5(8)        ; |239| 
||         ZERO    .S2     B16

           STW     .D2T2   B4,*+B5(12)       ; |245| 
||         PACK2   .L2X    B8,A8,B4          ; |247| 
||         RET     .S2     B3                ; |262| 

           STW     .D2T2   B4,*+B5(16)       ; |247| 
||         SET     .S2     B16,0x0,0xf,B16

           STW     .D2T2   B16,*+B5(20)      ; |251| 
||         ZERO    .L2     B4                ; |257| 
||         SHL     .S1     A4,4,A3           ; |261| 
||         MVKL    .S2     _qdma+12,B9

           STW     .D2T2   B4,*+B5(24)       ; |257| 
||         MVK     .L2     1,B4              ; |259| 
||         MVKH    .S2     _qdma+12,B9

           STW     .D2T2   B4,*+B5(28)       ; |259| 
||         ADD     .L2X    B9,A3,B5          ; |261| 

           STW     .D2T2   B4,*B5            ; |261| 
           ; BRANCH OCCURS {B3}              ; |262| 
	.sect	".text"
	.clink
	.global	_qdma_link1d

;******************************************************************************
;* FUNCTION NAME: qdma_link1d                                                 *
;*                                                                            *
;*   Regs Modified     : A0,A1,A3,A5,A6,A7,B4,B5,B6,B7,B8,B9,B16,B17          *
;*   Regs Used         : A0,A1,A3,A4,A5,A6,A7,A8,B3,B4,B5,B6,B7,B8,B9,B16,B17 *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_link1d:
;** --------------------------------------------------------------------------*
;** 271	-----------------------    param = (volatile unsigned *)(_paramNr<<5)+0x1c04000u;
;** 273	-----------------------    if ( _linkToParamNr <= 127u ) goto g3;
;** 290	-----------------------    *param = (*((_qdmaNr<<4)+&qdma)).tcc_nr<<12|0x100008u;
;** 290	-----------------------    goto g4;
;**	-----------------------g3:
;** 276	-----------------------    *param = 0u;
;**	-----------------------g4:
;** 303	-----------------------    param[1] = (unsigned)_src;
;** 305	-----------------------    param[2] = _numBytes&0xffffu|0x10000u;
;** 311	-----------------------    param[3] = (unsigned)_dst;
;** 313	-----------------------    param[4] = 0u;
;** 315	-----------------------    param[5] = (_linkToParamNr <= 127u) ? _linkToParamNr<<5 : 65535u;
;** 321	-----------------------    param[6] = 0u;
;** 323	-----------------------    param[7] = 1u;
;** 326	-----------------------    if ( _paramNr != _qdmaNr ) goto g6;
;** 328	-----------------------    (*((_qdmaNr<<4)+&qdma)).b_running = 1;
;**	-----------------------g6:
;**  	-----------------------    return;
           MVKL    .S1     _qdma+8,A5

           MVK     .S2     127,B5            ; |273| 
||         SHL     .S1     A4,4,A3           ; |290| 

           MVKH    .S1     _qdma+8,A5

           CMPGTU  .L1X    A6,B5,A0          ; |273| 
||         ADD     .S1     A5,A3,A3          ; |290| 

   [ A0]   LDW     .D1T1   *A3,A7            ; |290| 
           MVKL    .S2     0x1c04000,B16
           SHL     .S2     B4,5,B9           ; |271| 
           SHL     .S1     A6,5,A3           ; |315| 
           MVKL    .S2     0x100008,B17

           CMPGTU  .L1X    A6,B5,A1          ; |315| 
|| [ A0]   SHL     .S1     A7,12,A6          ; |290| 
||         MVKH    .S2     0x1c04000,B16

           MVKH    .S2     0x100008,B17
||         ADD     .L2     B16,B9,B5         ; |271| 
||         MV      .L1X    B8,A5             ; |269| 
||         ZERO    .D2     B8                ; |276| 

   [ A0]   OR      .L2X    B17,A6,B9         ; |290| 
|| [!A0]   STW     .D2T2   B8,*B5            ; |276| 

   [ A0]   STW     .D2T2   B9,*B5            ; |290| 
||         EXTU    .S1     A5,16,16,A5       ; |305| 
||         MV      .L2X    A8,B7             ; |269| 

           STW     .D2T2   B7,*+B5(4)        ; |303| 
||         SET     .S1     A5,16,16,A5       ; |305| 

           STW     .D2T1   A5,*+B5(8)        ; |305| 

           STW     .D2T2   B6,*+B5(12)       ; |311| 
||         ZERO    .L2     B6                ; |313| 
|| [ A1]   ZERO    .L1     A3

           RET     .S2     B3                ; |330| 
||         STW     .D2T2   B6,*+B5(16)       ; |313| 
|| [ A1]   SET     .S1     A3,0x0,0xf,A3

           STW     .D2T1   A3,*+B5(20)       ; |315| 
||         SHL     .S1     A4,4,A3           ; |328| 
||         MVKL    .S2     _qdma+12,B8

           MVK     .L1     1,A5              ; |323| 
||         MVKH    .S2     _qdma+12,B8

           STW     .D2T2   B6,*+B5(24)       ; |321| 
||         CMPEQ   .L1X    B4,A4,A0          ; |326| 
||         ADD     .L2X    B8,A3,B4          ; |328| 

           STW     .D2T1   A5,*+B5(28)       ; |323| 
||         MV      .L2X    A5,B5             ; |323| 

   [ A0]   STW     .D2T2   B5,*B4            ; |328| 
           ; BRANCH OCCURS {B3}              ; |330| 
	.sect	".text"
	.clink
	.global	_qdma_link2d

;******************************************************************************
;* FUNCTION NAME: qdma_link2d                                                 *
;*                                                                            *
;*   Regs Modified     : A0,A3,A5,A6,B0,B4,B5,B6,B7,B8,B9                     *
;*   Regs Used         : A0,A3,A4,A5,A6,A8,A10,A12,B0,B3,B4,B5,B6,B7,B8,B9,   *
;*                           B10                                              *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_qdma_link2d:
;** --------------------------------------------------------------------------*
;** 341	-----------------------    param = (volatile unsigned *)(_paramNr<<5)+0x1c04000u;
;** 343	-----------------------    if ( _linkToParamNr <= 127u ) goto g3;
;** 360	-----------------------    *param = (*((_qdmaNr<<4)+&qdma)).tcc_nr<<12|0x10000cu;
;** 360	-----------------------    goto g4;
;**	-----------------------g3:
;** 346	-----------------------    *param = 4u;
;**	-----------------------g4:
;** 373	-----------------------    param[1] = (unsigned)_src;
;** 375	-----------------------    param[2] = _numY<<16|_numBytesX&0xffffu;
;** 381	-----------------------    param[3] = (unsigned)_dst;
;** 383	-----------------------    param[4] = (unsigned)(_dstPitch<<16)|(unsigned)_srcPitch&0xffffu;
;** 387	-----------------------    param[5] = (_linkToParamNr <= 127u) ? _linkToParamNr<<5 : 65535u;
;** 393	-----------------------    param[6] = 0u;
;** 395	-----------------------    param[7] = 1u;
;** 398	-----------------------    if ( _paramNr != _qdmaNr ) goto g6;
;** 400	-----------------------    (*((_qdmaNr<<4)+&qdma)).b_running = 1;
;**	-----------------------g6:
;**  	-----------------------    return;
           MVKL    .S1     _qdma+8,A5
           SHL     .S1     A4,4,A3           ; |360| 

           MVKH    .S1     _qdma+8,A5
||         MVK     .S2     127,B5            ; |343| 

           ADD     .L1     A5,A3,A3          ; |360| 
||         CMPGTU  .L2X    A6,B5,B0          ; |343| 

   [ B0]   LDW     .D1T1   *A3,A3            ; |360| 
           MVKL    .S2     0x1c04000,B7
           MVKL    .S2     0x10000c,B9
           MVKH    .S2     0x1c04000,B7
           CMPGTU  .L1X    A6,B5,A0          ; |387| 

           SHL     .S2     B4,5,B5           ; |341| 
|| [ B0]   SHL     .S1     A3,12,A3          ; |360| 

           MVKH    .S2     0x10000c,B9
||         ADD     .L2     B7,B5,B5          ; |341| 
||         MVK     .D2     4,B7              ; |346| 

   [ B0]   OR      .L2X    B9,A3,B9          ; |360| 
|| [!B0]   STW     .D2T2   B7,*B5            ; |346| 

   [ B0]   STW     .D2T2   B9,*B5            ; |360| 
||         SHL     .S1     A6,5,A5           ; |387| 
||         MV      .L1X    B8,A6             ; |339| 

           STW     .D2T1   A6,*+B5(4)        ; |373| 
||         PACK2   .L1X    A12,B10,A3        ; |375| 

           MV      .L2X    A8,B8             ; |339| 
||         STW     .D2T1   A3,*+B5(8)        ; |375| 

           STW     .D2T2   B6,*+B5(12)       ; |381| 
||         PACK2   .L2X    B8,A10,B6         ; |383| 
|| [ A0]   ZERO    .L1     A5

           RET     .S2     B3                ; |402| 
||         STW     .D2T2   B6,*+B5(16)       ; |383| 
|| [ A0]   SET     .S1     A5,0x0,0xf,A5

           STW     .D2T1   A5,*+B5(20)       ; |387| 
||         MVK     .L1     1,A5              ; |395| 

           ZERO    .L2     B6                ; |393| 
||         SHL     .S1     A4,4,A3           ; |400| 
||         MVKL    .S2     _qdma+12,B7

           STW     .D2T2   B6,*+B5(24)       ; |393| 
||         MVKH    .S2     _qdma+12,B7
||         CMPEQ   .L1X    B4,A4,A0          ; |398| 
||         MV      .L2X    A5,B4             ; |398| 

           STW     .D2T1   A5,*+B5(28)       ; |395| 
||         ADD     .L2X    B7,A3,B5          ; |400| 

   [ A0]   STW     .D2T2   B4,*B5            ; |400| 
           ; BRANCH OCCURS {B3}              ; |402| 

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_needed(0)
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_preserved(0)
	.battr "TI", Tag_File, 1, Tag_Tramps_Use_SOC(1)
