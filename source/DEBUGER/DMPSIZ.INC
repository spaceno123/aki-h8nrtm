;
;	Program	DMPSIZ.INC
;	Date	1998/9/13 .. 1998/9/13
;	Copyright (C) 1998 by AKIYA
;

	;********************
	;* dump memory size *
	;********************

dmp_siz:
	bsr	putc_m		;"*DS-"
	mov.l	#dmp_siz_scn,er1
	bsr	fscan		;<ret> er2=start,er1=size,er0=wide
	bcc	dmp_siz_2:8
	cmp.l	#0,er1
	beq	dmp_siz_2:8	; size == 0 !
	cmp.l	#H'ff,er0
	bhi	dmp_siz_2:8	; wide > 255 !
	mov.w	r0,e0		; e0 = wide
	xor.l	er1,er2
	xor.l	er2,er1
	xor.l	er1,er2		; er1=start,er2=size
	add.l	er1,er2
	dec.l	#1,er2		; er2=end
dmp_siz_0
	bsr	dmp_siz_hex_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	bcc	dmp_siz_1:8	; end !
	bsr	chk_rxd_buf
	bcs	dmp_siz_1:8	; break !
	bsr	putc_crlf
	add.w	e0,r1
	bcc	dmp_siz_0:8
	inc.w	#1,e1
	bra	dmp_siz_0:8
dmp_siz_1
	orc	#B'00000001,ccr
dmp_siz_2
	rts
dmp_siz_scn
	.sdataz	"%x,%x,%x\r"
	.align	2

dmp_siz_hex_lin:
	push	er1
	push.w	e0
	mov.l	er1,er0
	bsr	putc_ha				; putc_hl -> putc_ha 98/5/16
	bsr	putc_sp
	xor.w	e0,e0
dmp_siz_hex_lin_0
	mov.w	e0,r0
	beq	dmp_siz_hex_lin_0a:8				;+ 98/9/19 ..
	and.b	#31,r0l
	bne	dmp_siz_hex_lin_0a:8
	bsr	putc_crlf
	mov.b	#7,r0l
	bsr	putc_sp_n
	mov.w	e0,r0
dmp_siz_hex_lin_0a						;.. + 98/9/19
	and.b	#H'7,r0l
	bne	dmp_siz_hex_lin_1:8
	bsr	putc_sp
dmp_siz_hex_lin_1
	mov.b	@er1,r0l
	bsr	putc_hb
	cmp.l	er1,er2
	beq	dmp_siz_hex_lin_2:8
	adds	#1,er1
	inc.w	#1,e0
	mov.w	@sp,r0
	cmp.w	r0,e0
	blo	dmp_siz_hex_lin_0:8
	orc	#B'00000001,ccr
dmp_siz_hex_lin_2
	pop.w	e0
	pop	er1
	rts

