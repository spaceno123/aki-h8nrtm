;
;	Program	WRESET.INC
;	Date	1998/11/15 .. 1998/11/15
;	Copyright (C) 1998 by AKIYA
;

	;*******************
	;* watch dog reset *
	;*******************

wreset:
	mov.l	#wreset_scn,er1
	bsr	fscan
	bcs	wreset_0:8
	rts
	;* message *
wreset_0
;	orc	#B'11000000,ccr
	mov.l	#wreset_msg,er1
	bsr	dputs:8
	;* reset *
	mov.w	#H'a500+B'00011000,r0
	mov.w	r0,@TCSR
	mov.b	@RSTCSR:8,r0l
	mov.w	#H'a500,r0
	mov.w	r0,@RSTCSRw
	mov.w	#H'5a00+H'ff,r0
	mov.w	r0,@TCNTw
	mov.w	#H'a500+B'01111000,r0
	mov.w	r0,@TCSR
	;* loop *
wreset_1
	bra	wreset_1:8

dputs:
	mov.b	@er1+,r0l
	bne	dputs_0:8
	rts
dputs_0
	bld	#7,@SSRx:8
	bcc	dputs_0:8
	mov.b	r0l,@TDRx:8
	bclr	#7,@SSRx:8
	bra	dputs:8

wreset_scn
	.sdataz	"eset\r"
wreset_msg
	.sdataz	"::: RESET :::"<H'0d><H'0a>
	.align	2
