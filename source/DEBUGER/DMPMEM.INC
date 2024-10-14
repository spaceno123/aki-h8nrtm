;
;	Program	DMPMEM.INC
;	Date	1998/5/2 .. 1998/5/4
;	Copyright (C) 1998 by AKIYA
;
;--- up date ---
; 1998/5/16	address size 32bit's -> 24bit's
;

	;***************
	;* dump memory *
	;***************

dmp_mem:
	bsr	putc_m		;"*DM-"
	mov.l	#dmp_mem_scn,er1
	bsr	fscan
	bcc	dmp_mem_2:8
	mov.l	er0,er2
dmp_mem_0
	bsr	dmp_mem_hex_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	bsr	dmp_mem_asc_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	bcc	dmp_mem_1:8	; end !
	bsr	chk_rxd_buf
	bcs	dmp_mem_1:8	; break !
	bsr	putc_crlf
	add.l	#H'10,er1
	and.b	#H'f0,r1l
	bra	dmp_mem_0:8
dmp_mem_1
	orc	#B'00000001,ccr
dmp_mem_2
	rts
dmp_mem_scn
	.sdataz	"%x,%x\r"
	.align	2

dmp_mem_hex_lin:
	push	er1
	mov.l	er1,er0
	and.b	#H'f0,r0l
	bsr	putc_ha				; putc_hl -> putc_ha 98/5/16
	bsr	putc_sp
	mov.b	r1l,r0l
	and.b	#H'0f,r0l
	beq	dmp_mem_hex_lin_1:8
	mov.b	#3,r0h
	mulxu.b	r0h,r0
	cmp.b	#8*3,r0l
	bls	dmp_mem_hex_lin_0:8
	add.b	#1,r0l
dmp_mem_hex_lin_0
	add.b	#1,r0l
	bsr	putc_sp_n
dmp_mem_hex_lin_1
	mov.b	r1l,r0l
	and.b	#H'7,r0l
	bne	dmp_mem_hex_lin_2:8
	bsr	putc_sp
dmp_mem_hex_lin_2
	mov.b	@er1,r0l
	bsr	putc_hb
	bsr	putc_sp
	mov.b	r1l,r0l
	cmp.l	er1,er2
	beq	dmp_mem_hex_lin_3:8
	adds	#1,er1
	mov.b	r1l,r0l
	and.b	#H'f,r0l
	bne	dmp_mem_hex_lin_1:8
	pop	er1
	orc	#B'00000001,ccr
	rts
dmp_mem_hex_lin_3
	mov.b	#H'f,r0l
	and.b	#H'f,r1l
	sub.b	r1l,r0l
	beq	dmp_mem_hex_lin_5:8
	mov.b	#3,r0h
	mulxu.b	r0h,r0
	cmp.b	#8*3,r0l
	blo	dmp_mem_hex_lin_4:8
	add.b	#1,r0l
dmp_mem_hex_lin_4
	bsr	putc_sp_n
dmp_mem_hex_lin_5
	pop	er1
	andc	#B'11111110,ccr
	rts

dmp_mem_asc_lin:
	push	er1
	mov.b	r1l,r0l
	and.b	#H'f,r0l
	inc.b	r0l
	bsr	putc_sp_n
dmp_mem_asc_lin_0
	mov.b	@er1,r0l
	cmp.b	#" ",r0l
	blo	dmp_mem_asc_lin_1:8
	cmp.b	#H'80,r0l
	blo	dmp_mem_asc_lin_2:8
	cmp.b	#H'a0,r0l
	blo	dmp_mem_asc_lin_1:8
	cmp.b	#H'e0,r0l
	blo	dmp_mem_asc_lin_2:8
dmp_mem_asc_lin_1
	mov.b	#".",r0l
dmp_mem_asc_lin_2
	bsr	putc
	cmp.l	er1,er2
	beq	dmp_mem_asc_lin_3:8
	adds	#1,er1
	mov.b	r1l,r0l
	and.b	#H'f,r0l
	bne	dmp_mem_asc_lin_0:8
	pop	er1
	orc	#B'00000001,ccr
	rts
dmp_mem_asc_lin_3
	mov.b	#H'f,r0l
	and.b	#H'f,r1l
	sub.b	r1l,r0l
	beq	dmp_mem_asc_lin_4:8
	bsr	putc_sp_n
dmp_mem_asc_lin_4
	pop	er1
	andc	#B'11111110,ccr
	rts

