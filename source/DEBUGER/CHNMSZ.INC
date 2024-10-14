;
;	Program	CMEMSZ.INC
;	Date	1998/12/6 .. 1998/12/6
;	Copyright (C) 1998 by AKIYA
;

	;**********************
	;* change memory size *
	;**********************

chn_msz:
	bsr	putc_m		;"*CS-"
	mov.l	#chn_msz_scn,er1
	bsr	fscan
	bcc	chn_msz_1b ;:8
	cmp.l	#0,er0
	beq	chn_msz_1b ;:8
	cmp.l	#8,er0
	bhi	chn_msz_1b:8
	mov.l	er0,er2		; er2=size
	push	er1		; @sp=address
chn_msz_0
	mov.l	@sp,er0
	bsr	putc_ha
	bsr	putc_sp
	mov.l	@sp,er1
	bsr	chn_msz_dsp ;:8
	bsr	putc_m
	mov.b	r2l,r0l
	shll.b	r0l
	bsr	chn_msz_get
	bcs	chn_msz_0a:8
	cmp.b	#" ",r0l
	beq	chn_msz_0b:8
	cmp.b	#".",r0l
	beq	chn_msz_1:8
	cmp.b	#H'0d,r0l
	beq	chn_msz_1:8
	andc	#B'11111110,ccr
	bra	chn_msz_1a:8
chn_msz_0a
	push.w	r3
	mov.w	r2,r3
	push	er2
	mov.l	@(4+2,sp),er2
	bsr	chn_msz_sub ;:8
	pop	er2
	pop.w	r3
chn_msz_0b
	mov.l	@sp,er1
	bsr	chn_msz_dsp ;:8
	bsr	putc_crlf
	mov.l	@sp,er1
	add.l	er2,er1
	mov.l	er1,@sp
	bra	chn_msz_0:8
chn_msz_1
	orc	#B'00000001,ccr
chn_msz_1a
	adds	#4,sp
chn_msz_1b
	rts

chn_msz_scn
	.sdataz	"%x,%x\r"
	.align	2

;- get value with echo hex n charactor's to binary -
; entry	 r0l=n(1..16)
; return er2:er1=value
;	 r0h=input value count
;	 cary=1:get end,0:get not hex charactor(r0l=charctor,r0h==0 ? no value)
; break	 er0,er1,er2
; stack	 (4)+50
getval_hc2b_n2:
	sub.l	er2,er2
	sub.l	er1,er1
	extu.w	r0	; r0h=0
	mov.w	r0,e0
	beq	getval_hc2b_n2_2:8
	bsr	getval_hc2b
	bcs	getval_hc2b_n2_1:8
	xor.w	e0,e0
	bra	getval_hc2b_n2_3:8
getval_hc2b_n2_0
	bsr	getval_hc2b
	bcc	getval_hc2b_n2_3:8
getval_hc2b_n2_1
	shll.l	er1
	rotxl.l	er2
	shll.l	er1
	rotxl.l	er2
	shll.l	er1
	rotxl.l	er2
	shll.l	er1
	rotxl.l	er2
	or.b	r0l,r1l
	inc.b	r0h						;+ 98/7/12
	dec.w	#1,e0
	bne	getval_hc2b_n2_0:8
getval_hc2b_n2_2
	orc	#B'00000001,ccr
getval_hc2b_n2_3
	rts

chn_msz_get:	;<ent> r0l=keta <ret> cary=1(er0:er1=value),0:(r0l=code)
	push	er2
	bsr	getval_hc2b_n2
	bcs	chn_msz_get_1:8
	cmp.b	#H'0d,r0l
	beq	chn_msz_get_0:8
	mov.w	r0,r2
	cmp.b	#0,r0h
	beq	chn_msz_get_1:8
	mov.b	#0,r2l
	andc	#B'11111110,ccr
	bra	chn_msz_get_1:8
chn_msz_get_0
	orc	#B'00000001,ccr
chn_msz_get_1
	stc.w	ccr,@-sp
	mov.b	r0h,r0h
	beq	chn_msz_get_2a:8
chn_msz_get_2
	bsr	putc_bs
	dec.b	r0h
	bne	chn_msz_get_2:8
chn_msz_get_2a
	ldc.w	@sp+,ccr
	mov.l	er2,er0
	pop	er2
	rts

chn_msz_sub:
	mov.b	#8,r3h
	sub.b	r3l,r3h
	bld	#0,r2l
	bor	#0,r3l
	bcs	chn_msz_sub_3:8
	;* evn and (2 4 6 8) *
	cmp.b	#4,r3l
	blo	chn_msz_sub_2:8
	beq	chn_msz_sub_0:8
	cmp.b	#8,r3l
	blo	chn_msz_sub_1:8
	;* evn 8 *
	mov.l	er0,@er2
	adds	#4,er2
	;* evn 4 *
chn_msz_sub_0
	mov.l	er1,@er2
	rts
	;* evn 6 *
chn_msz_sub_1
	mov.w	r0,@er2
	adds	#2,er2
	mov.w	e1,@er2
	adds	#2,er2
chn_msz_sub_2
	mov.w	r1,@er2
	rts
	;* odd or (1 3 5 7) *
chn_msz_sub_3
	mov.b	#8,r3l
chn_msz_sub_3a
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	shll.l	er1
	rotxl.l	er0
	bst	#0,r1l
	mov.b	r3h,r3h
	beq	chn_msz_sub_3b:8
	dec.b	r3h
	bra	chn_msz_sub_3c:8
chn_msz_sub_3b
	mov.b	r1l,@er2
	adds	#1,er2
chn_msz_sub_3c
	dec.b	r3l
	bne	chn_msz_sub_3a:8
	rts

chn_msz_dsp:
	mov.b	r2l,r2h	; r2h=count
	bld	#0,r2l
	bor	#0,r1l
	bcc	chn_msz_dsp_1:8
	;* odd *
chn_msz_dsp_0
	mov.b	@er1+,r0l
	bsr	putc_hb
	dec	r2h
	bne	chn_msz_dsp_0:8
	rts
	;* evn *
chn_msz_dsp_1
	cmp.b	#4,r2l
	beq	chn_msz_dsp_2:8
	cmp.b	#8,r2l
	beq	chn_msz_dsp_3:8
	;* word *
chn_msz_dsp_1a
	mov.w	@er1+,r0
	xor.b	r0l,r0h
	xor.b	r0h,r0l
	xor.b	r0l,r0h
	bsr	putc_hb
	xor.b	r0l,r0h
	xor.b	r0h,r0l
	xor.b	r0l,r0h
	bsr	putc_hb
	add.b	#-2,r2h
	bne	chn_msz_dsp_1a:8
	rts
	;* 4 *
chn_msz_dsp_2
	mov.l	@er1+,er0
chn_msz_dsp_2a
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	rotl.l	er0
	bsr	putc_hb
	dec.b	r2h
	bne	chn_msz_dsp_2a:8
	rts
	;* 8 *
chn_msz_dsp_3
	mov.l	@er1+,er0
	mov.l	@er1,er1
	mov.b	#4,r2h
	bsr	chn_msz_dsp_2a:8
	mov.l	er1,er0
	mov.b	#4,r2h
	bra	chn_msz_dsp_2a:8

