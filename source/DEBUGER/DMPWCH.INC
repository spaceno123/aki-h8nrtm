;
;	Program	DMPWCH.INC
;	Date	1998/8/30 .. 1998/8/30
;	Copyright (C) 1998 by AKIYA
;

	;**************
	;* dump watch *
	;**************

dmp_wch:
	bsr	putc_m		;"*DW-"
	mov.l	#dmp_wch_scn,er1
	bsr	fscan
	bcc	dmp_wch_4 ;:8	; error !
	mov.l	er0,er2
;	beq	dmp_wch_4 ;:8	; size 0 !
;	add.l	er1,er2
;	dec.l	#1,er2
	push	er1		;!
	push	er2		;!!
	sub.l	er0,er0
dmp_wch_0
	bsr	chk_rxd_buf
	bcs	dmp_wch_2:8	; break !
	push	er0
	bsr	dmp_mem_hex_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	bsr	dmp_mem_asc_lin	;<ent> er1=start, er2=end <ret> cary=0:end
	pop	er0
	bcc	dmp_wch_1:8	; end !
	push.w	r0
	bsr	putc_crlf
	pop.w	r0
	cmp.w	e0,r0
	bne	dmp_wch_0a:8
	inc.w	#1,e0
dmp_wch_0a
	inc.w	#1,r0
	add.l	#H'10,er1
	and.b	#H'f0,r1l
	bra	dmp_wch_0:8
dmp_wch_1
	cmp.w	#24,e0
	bhs	dmp_wch_2a:8
	mov.b	#H'0d,r0l
	bsr	putc
	mov.w	e0,e0
	beq	dmp_wch_1a:8
	mov.l	#dmp_wch_rep,er1
	bsr	puts
	mov.w	e0,r0
	bsr	putc_db
	bsr	puts
dmp_wch_1a
	mov.l	@sp,er2		;!!
	mov.l	@(4,sp),er1	;!
	xor.w	r0,r0
	bra	dmp_wch_0:8
dmp_wch_2
	cmp.w	e0,r0
	beq	dmp_wch_2a:8
	push.w	r0
	bsr	putc_crlf
	pop.w	r0
	inc.w	#1,r0
	bra	dmp_wch_2:8
dmp_wch_2a
	adds	#4,sp		;!!
	adds	#4,sp		;!
	orc	#B'00000001,ccr
dmp_wch_4
	rts
dmp_wch_scn
	.sdataz	"%x,%x\r"
dmp_wch_rep
	.sdataz	<H'1b>"["<0>"A"	; cursor n line up
	.align	2

putc_db		;<ent> r0 <brk> r0
	push.w	e0
	push	er1
	extu.w	r0
	mov.w	r0,e0
	mov.w	#100,r1
	bsr	bin2pac4_0:8
	mov.w	r0,r0
	beq	putc_db_0:8
	add.b	#"0",r0l
	bsr	putc
	mov.w	#10,r1
	bsr	bin2pac4_0:8
	bra	putc_db_1:8
putc_db_0
	mov.w	#10,r1
	bsr	bin2pac4_0:8
	mov.w	r0,r0
	beq	putc_db_2:8
putc_db_1
	add.b	#"0",r0l
	bsr	putc
putc_db_2
	mov.w	e0,r0
	add.b	#"0",r0l
	bsr	putc
	pop	er1
	pop.w	e0
	rts

bin2pac4	;<ent> r0 <ret> r0(0..9999)
	push.w	e0
	push	er1
	extu.l	er0
	mov.w	#10000,r1
	divxu.w	r1,er0
	xor.w	e1,e1
	mov.w	#1000,r1
	bsr	bin2pac4_0:8
	mov.w	#100,r1
	bsr	bin2pac4_0:8
	mov.w	#10,r1
	bsr	bin2pac4_0:8
	or.w	e0,e1
	mov.w	e1,r0
	pop	er1
	pop.w	e0
	rts
bin2pac4_0
	mov.w	e0,r0
	extu.l	er0
	divxu.w	r1,er0
	or.w	r0,e1
	shll.w	e1
	shll.w	e1
	shll.w	e1
	shll.w	e1
	rts

