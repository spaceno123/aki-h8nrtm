;
;	Program	USRFNC.INC
;	Date	1999/10/16 .. 1999/10/16
;	Copyright (C) 1999 by AKIYA
;

	;*****************
	;* user function *
	;*****************

usr_fnc:
	mov.l	@startof DEBUG_RAM + x_usr_fnc,er1
	beq	usr_fnc_1:8
	;* exist *
	mov.l	@startof DEBUG_RAM + x_int_end,er0
	and.w	#H'ff,e0
	sub.l	#int_end_dbg,er0
	beq	usr_fnc_0:8
	xor.l	er0,er0
	adds	#1,er0
usr_fnc_0
	jsr	@er1	;<ent> er0=0:monitor or 1:debug
	sub.w	#1,r0
	rts
	;* un-define *
usr_fnc_1
	mov.l	#usr_fnc_ndf,er1
	bsr	puts
	orc	#B'00000001,ccr
	rts

	;*********************
	;* set user function *
	;*********************

set_usr:
	bsr	putc_m		;">SU-"
	mov.l	#usr_scn,er1
	bsr	fscan
	bcs	set_usr_0:8
	rts
set_usr_0
	mov.l	er0,@startof DEBUG_RAM + x_usr_fnc
	mov.l	#set_usr_msg,er1
	bra	dsp_usr_0:8

	;***********************
	;* clear user function *
	;***********************

clr_usr:
	mov.l	#clr_usr_msg,er1
	bsr	puts
	sub.l	er0,er0
	mov.l	er0,@startof DEBUG_RAM + x_usr_fnc
	orc	#B'00000001,ccr
	rts

	;**********************
	;* disp user function *
	;**********************

dsp_usr:
	mov.l	#dsp_usr_msg,er1
dsp_usr_0
	bsr	puts
	mov.l	@startof DEBUG_RAM + x_usr_fnc,er0
	bsr	putc_ha
	orc	#B'00000001,ccr
	rts

	;*************************
	;* defined user function *
	;*************************
	; entry	er0 = function address
	.export	_def_usr
_def_usr:
	mov.l	er0,@startof DEBUG_RAM + x_usr_fnc
	rts

	;*************
	;* user putc *
	;*************
	.export	_uputc
_uputc	.equ	putc

	;*************
	;* user getc *
	;*************
	.export	_ugetc
_ugetc	.equ	getc

	;*******************
	;* user getc check *
	;*******************
	.export	_ugetchk
_ugetchk:
	mov.b	@startof DEBUG_RAM + x_rxd_act,r0l
	extu.w	r0
	rts

usr_fnc_ndf
	.sdataz	" not defined"
set_usr_msg
	.sdata	"Set "		; continue next line !
dsp_usr_msg
	.sdataz	"SU="
clr_usr_msg
	.sdataz	"SU clear"
usr_scn
	.sdataz	"%x\r"
	.align	2
