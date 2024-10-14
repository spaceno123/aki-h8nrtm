;
;	Program	WCHSIZ.INC
;	Date	1998/9/13 .. 1998/9/13
;	Copyright (C) 1998 by AKIYA
;

	;*********************
	;* watch memory size *
	;*********************

wch_siz:
	bsr	putc_m		;"*WS-"
	mov.l	#wch_siz_scn,er1
	bsr	fscan		;<ret> er2=start,er1=size,er0=wide
	bcc	wch_siz_4 ;:8
	cmp.l	#0,er1
	beq	wch_siz_4 ;:8	; size == 0 !
	cmp.l	#H'ff,er0
	bhi	wch_siz_4 ;:8	; wide > 255 !
	cmp.l	er0,er1
	bhs	wch_siz_0:8	; size >= wide !
	mov.l	er1,er0
wch_siz_0
	mov.w	r0,e0		; e0 = wide
	xor.l	er1,er2
	xor.l	er2,er1
	xor.l	er1,er2		; er1=start,er2=size
	add.l	er1,er2
	dec.l	#1,er2		; er2=end
	push	er1		;!
	push	er2		;!!
	xor.w	r0,r0
wch_siz_1
	bsr	chk_rxd_buf
	bcs	wch_siz_3:8	; break !
	push.w	r0
	bsr	dmp_siz_hex_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	pop.w	r0
	bcc	wch_siz_2:8	; end !
	push.w	r0		;!!!
	bsr	putc_crlf
	pop.w	r0		;!!!
	cmp.b	r0l,r0h
	bne	wch_siz_1a:8
	bsr	wch_siz_inc_r0h	;inc.b	r0h
wch_siz_1a
	bsr	wch_siz_inc_r0l	;inc.b	r0l
	add.w	e0,r1
	bcc	wch_siz_1:8
	inc.w	#1,e1
	bra	wch_siz_1:8
wch_siz_2
	cmp.b	r0l,r0h
	bne	wch_siz_2a:8
	push.w	e0
	push	er2
	inc.l	#1,er2
	sub.l	er1,er2
	mov.w	r2,e0
	bsr	wch_siz_inc_r0h	;inc.b	r0h
	dec.b	r0h		; must be -1 !
	pop	er2
	pop.w	e0
wch_siz_2a
	cmp.b	#24,r0h
	bhs	wch_siz_3a:8
	push.w	r0		;!!!
	mov.b	#H'0d,r0l
	bsr	putc
	mov.b	@sp,r0h
	beq	wch_siz_2b:8
	mov.l	#wch_siz_rep,er1
	bsr	puts
	mov.b	@sp,r0l
	extu.w	r0
	bsr	putc_db
	bsr	puts
wch_siz_2b
	pop.w	r0		;!!!
	mov.l	@sp,er2		;!!
	mov.l	@(4,sp),er1	;!
	xor.b	r0l,r0l
	bra	wch_siz_1:8
wch_siz_3
	cmp.b	r0h,r0l
	beq	wch_siz_3a:8
	push.w	r0
	bsr	putc_crlf
	pop.w	r0
	inc.b	r0l
	bra	wch_siz_3:8
wch_siz_3a
	adds	#4,sp		;!!
	adds	#4,sp		;!
	orc	#B'00000001,ccr
wch_siz_4
	rts
wch_siz_inc_r0h
	push.w	e0
wch_siz_inc_r0h_0
	inc.b	r0h
	sub.w	#32,e0
	bgt	wch_siz_inc_r0h_0:8
	pop.w	e0
	rts
wch_siz_inc_r0l
	push.w	e0
wch_siz_inc_r0l_0
	inc.b	r0l
	sub.w	#32,e0
	bgt	wch_siz_inc_r0l_0:8
	pop.w	e0
	rts
wch_siz_scn
	.sdataz	"%x,%x,%x\r"
wch_siz_rep
	.sdataz	<H'1b>"["<0>"A"	; cursor n line up
	.align	2

