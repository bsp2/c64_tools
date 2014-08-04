;******************************************************************************
;* TMS320C6x C/C++ Codegen                                        Unix v7.4.5 *
;* Date/Time created: Wed Dec 11 20:04:09 2013                                *
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

;	opt6x /tmp/149083dB3Gf /tmp/14908l4LmKq 
	.sect	".text"
	.clink
	.global	_int_map_event_to_vector

;******************************************************************************
;* FUNCTION NAME: int_map_event_to_vector                                     *
;*                                                                            *
;*   Regs Modified     : A0,A1,A3,A4,A5,B0,B1,B4,B5                           *
;*   Regs Used         : A0,A1,A3,A4,A5,B0,B1,B3,B4,B5                        *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_int_map_event_to_vector:
;** --------------------------------------------------------------------------*
;** 51	-----------------------    if ( _vectorNr < 8u ) goto g7;

           CMPLTU  .L1X    B4,8,A1           ; |51| 
||         MVKL    .S1     0x1800104,A0
||         SUB     .S2     B4,4,B5           ; |54| 
||         CMPLTU  .L2     B4,12,B1          ; |58| 

   [ A1]   BNOP    .S2     $C$L1,4           ; |51| 
||         MVKH    .S1     0x1800104,A0
|| [!A1]   CMPLTU  .L2     B4,16,B0          ; |64| 

   [!A1]   ADD     .L1     4,A0,A0
           ; BRANCHCC OCCURS {$C$L1}         ; |51| 
;** --------------------------------------------------------------------------*
;** 58	-----------------------    if ( _vectorNr < 12u ) goto g6;
;** 64	-----------------------    if ( _vectorNr < 16u ) goto g5;
;** 73	-----------------------    reg = 0u;
;** 73	-----------------------    goto g8;
;**	-----------------------g5:
;** 67	-----------------------    bitoff = _vectorNr-12u;
;** 68	-----------------------    reg = 0x180010cu;
;** 69	-----------------------    goto g8;
;**	-----------------------g6:
;** 61	-----------------------    bitoff = _vectorNr-8u;
;** 62	-----------------------    reg = 0x1800108u;
;** 63	-----------------------    goto g8;
;**	-----------------------g7:
;** 54	-----------------------    bitoff = _vectorNr-4u;
;** 55	-----------------------    reg = 0x1800104u;

   [ B1]   ZERO    .L2     B0
|| [!B1]   ZERO    .L1     A0                ; |73| 
||         SUB     .S2     B4,8,B5           ; |61| 

   [ B0]   MVKL    .S1     0x180010c,A0
|| [!B1]   SUB     .L2     B4,12,B5          ; |67| 

   [ B0]   MVKH    .S1     0x180010c,A0
;** --------------------------------------------------------------------------*
$C$L1:    
;**	-----------------------g8:
;** 76	-----------------------    if ( !reg ) goto g10;
;** 79	-----------------------    bitoff <<= 8;
;** 82	-----------------------    *(volatile unsigned *)reg = *(volatile unsigned *)reg&~(255u<<bitoff)|(_eventId&0x7fu)<<bitoff;
;**	-----------------------g10:
;**  	-----------------------    return;

   [ A0]   LDW     .D1T1   *A0,A3            ; |82| 
||         MVK     .S1     255,A5            ; |82| 
||         SHL     .S2     B5,8,B4           ; |79| 

           EXTU    .S1     A4,25,25,A4       ; |82| 
           SHL     .S2X    A5,B4,B5          ; |82| 
           RETNOP  .S2     B3,1              ; |87| 
   [ A0]   ANDN    .L1X    A3,B5,A3          ; |82| 
           SHL     .S2X    A4,B4,B4          ; |82| 
   [ A0]   OR      .L2X    B4,A3,B4          ; |82| 
   [ A0]   STW     .D1T2   B4,*A0            ; |82| 
           ; BRANCH OCCURS {B3}              ; |87| 
	.sect	".text"
	.clink
	.global	_int_event_enable

;******************************************************************************
;* FUNCTION NAME: int_event_enable                                            *
;*                                                                            *
;*   Regs Modified     : A3,A4,B4,B5                                          *
;*   Regs Used         : A3,A4,B3,B4,B5                                       *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_int_event_enable:
;** --------------------------------------------------------------------------*
;** 93	-----------------------    reg = (_eventId>>3&0xfffffffcu)+0x1800080u;
;** 94	-----------------------    bitNr = _eventId&0x1fu;
;** 96	-----------------------    *(volatile unsigned *)reg = *(volatile unsigned *)reg&~(1u<<bitNr);
;**  	-----------------------    return;
           SHRU    .S1     A4,3,A3           ; |93| 

           AND     .L1     -4,A3,A3          ; |93| 
||         MVKL    .S2     0x1800080,B4

           MVKH    .S2     0x1800080,B4
           ADD     .L2X    B4,A3,B4          ; |93| 
           LDW     .D2T2   *B4,B5            ; |96| 
           RET     .S2     B3                ; |97| 

           EXTU    .S1     A4,27,27,A3       ; |94| 
||         MVK     .L1     1,A4              ; |96| 

           SHL     .S1     A4,A3,A3          ; |96| 
           NOP             1
           ANDN    .L2X    B5,A3,B5          ; |96| 
           STW     .D2T2   B5,*B4            ; |96| 
           ; BRANCH OCCURS {B3}              ; |97| 
	.sect	".text"
	.clink
	.global	_int_event_disable

;******************************************************************************
;* FUNCTION NAME: int_event_disable                                           *
;*                                                                            *
;*   Regs Modified     : A3,A4,B4,B5                                          *
;*   Regs Used         : A3,A4,B3,B4,B5                                       *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_int_event_disable:
;** --------------------------------------------------------------------------*
;** 103	-----------------------    reg = (_eventId>>3&0xfffffffcu)+0x1800080u;
;** 104	-----------------------    bitNr = _eventId&0x1fu;
;** 106	-----------------------    *(volatile unsigned *)reg |= 1u<<bitNr;
;**  	-----------------------    return;
           SHRU    .S1     A4,3,A3           ; |103| 

           AND     .L1     -4,A3,A3          ; |103| 
||         MVKL    .S2     0x1800080,B4

           MVKH    .S2     0x1800080,B4
           ADD     .L2X    B4,A3,B4          ; |103| 
           LDW     .D2T2   *B4,B5            ; |106| 
           RET     .S2     B3                ; |107| 

           EXTU    .S1     A4,27,27,A3       ; |104| 
||         MVK     .L1     1,A4              ; |106| 

           SHL     .S1     A4,A3,A3          ; |106| 
           NOP             1
           OR      .L2X    A3,B5,B5          ; |106| 
           STW     .D2T2   B5,*B4            ; |106| 
           ; BRANCH OCCURS {B3}              ; |107| 
	.sect	".text"
	.clink
	.global	_int_event_clear

;******************************************************************************
;* FUNCTION NAME: int_event_clear                                             *
;*                                                                            *
;*   Regs Modified     : A3,A4,A5,B4                                          *
;*   Regs Used         : A3,A4,A5,B3,B4                                       *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_int_event_clear:
;** --------------------------------------------------------------------------*
;** 113	-----------------------    reg = (_eventId>>3&0xfffffffcu)+0x1800040u;
;** 114	-----------------------    bitNr = _eventId&0x1fu;
;** 116	-----------------------    *(volatile unsigned *)reg = 1u<<bitNr;
;**  	-----------------------    return;
           RET     .S2     B3                ; |117| 
           SHRU    .S1     A4,3,A3           ; |113| 

           AND     .L1     -4,A3,A3          ; |113| 
||         MVKL    .S2     0x1800040,B4

           MVK     .L1     1,A4              ; |116| 
||         MVKH    .S2     0x1800040,B4
||         EXTU    .S1     A4,27,27,A5       ; |114| 

           SHL     .S1     A4,A5,A3          ; |116| 
||         ADD     .L2X    B4,A3,B4          ; |113| 

           STW     .D2T1   A3,*B4            ; |116| 
           ; BRANCH OCCURS {B3}              ; |117| 

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_needed(0)
	.battr "TI", Tag_File, 1, Tag_ABI_stack_align_preserved(0)
	.battr "TI", Tag_File, 1, Tag_Tramps_Use_SOC(1)
