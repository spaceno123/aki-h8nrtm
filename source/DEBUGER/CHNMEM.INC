;
;	Program	CHNMEM.INC
;	Date	1998/7/5 .. 1998/6/27
;	Copyright (C) 1998 by AKIYA
;

	;*****************
	;* change memory *
	;*****************

chn_mem:
	bsr	putc_m		;"*CM-"
	mov.l	#chn_mem_scn,er1
	bsr	fscan
	bcc	chn_mem_1a:8
	mov.l	er0,er1
chn_mem_0
	mov.l	er1,er0
	bsr	putc_ha
	bsr	putc_sp
	mov.b	@er1,r0l
	bsr	putc_hb
	bsr	putc_m
	bsr	getche_hc2b
	bcs	chn_mem_0a:8
	cmp.b	#" ",r0l
	beq	chn_mem_0b:8
	cmp.b	#".",r0l
	beq	chn_mem_1:8
	cmp.b	#H'0d,r0l
	beq	chn_mem_1:8
	andc	#B'11111110,ccr
	bra	chn_mem_1a:8
chn_mem_0a
	mov.b	r0l,r0h
	bsr	getche_hc2b
	bcc	chn_mem_1a:8
	shll.b	r0h
	shll.b	r0h
	shll.b	r0h
	shll.b	r0h
	or.b	r0h,r0l
	mov.b	r0l,@er1
	bsr	putc_bs
chn_mem_0b
	bsr	putc_bs
	mov.b	@er1+,r0l
	bsr	putc_hb
	bsr	putc_crlf
	bra	chn_mem_0:8
chn_mem_1
	orc	#B'00000001,ccr
chn_mem_1a
	rts

chn_mem_scn
	.sdataz	"%x\r"
	.align	2
