;
;	Program	INIMEM.INC
;	Date	1998/6/27 .. 1998/6/27
;	Copyright (C) 1998 by AKIYA
;
;--- up date ---
; 1999/10/16 0.61 debugger work size valiable
;

	;*********************
	;* initialize memory *
	;*********************

ini_mem:
	bsr	putc_m		;"*IM-"
	mov.l	#ini_mem_scn,er1
	bsr	fscan
	bcc	ini_mem_3:8
	mov.b	@MDCR,r0h
	and.b	#7,r0h	;001:1M,010:1M,011:16M,100:16M,101:1M,110:16M,111:1M
	cmp.b	#3,r0h
	beq	ini_mem_1:8
	cmp.b	#4,r0h
	beq	ini_mem_1:8
	cmp.b	#6,r0h
	beq	ini_mem_1:8
	;* address mode 1M *
	and.l	#H'fffff,er1
	and.l	#H'fffff,er2
ini_mem_0	; r0l=init. er1=end, er2=start
	cmp.l	er1,er2
	bhi	ini_mem_2:8
	push.w	r0
;*	mov.l	#dbg_wrk,er0
	mov.l	#startof DEBUG_RAM,er0				;* 99/10/16
	and.l	#H'fffff,er0
	cmp.l	er0,er2
	blo	ini_mem_0a:8
;*	mov.l	#dbg_wke,er0
	mov.l	#startof DEBUG_RAM + sizeof DEBUG_RAM,er0	;* 99/10/16
	and.l	#H'fffff,er0
	cmp.l	er0,er2
	blo	ini_mem_0b:8
ini_mem_0a
	mov.w	@sp,r0
	mov.b	r0l,@er2
ini_mem_0b
	pop.w	r0
	adds	#1,er2
	bra	ini_mem_0:8
	;* address mode 16M *
ini_mem_1	; r0l=init. er1=end, er2=start
	cmp.l	er1,er2
	bhi	ini_mem_2:8
;*	cmp.l	#dbg_wrk,er2
	cmp.l	#startof DEBUG_RAM,er2				;* 99/10/16
	blo	ini_mem_1a:8
;*	cmp.l	#dbg_wke,er2
	cmp.l	#startof DEBUG_RAM + sizeof DEBUG_RAM,er2	;* 99/10/16
	blo	ini_mem_1b:8
ini_mem_1a
	mov.b	r0l,@er2
ini_mem_1b
	adds	#1,er2
	bra	ini_mem_1:8
ini_mem_2
	orc	#B'00000001,ccr
ini_mem_3
	rts
ini_mem_scn
	.sdataz	"%x,%x,%2x\r"
	.align	2

