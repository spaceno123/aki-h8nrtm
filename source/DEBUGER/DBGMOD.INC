;
;	Program	DBGMOD.INC
;	Date	1998/5/21 .. 1998/5/31
;	Copyright (C) 1998 by AKIYA
;
;--- up date ---
; 1998/6/6  0.51 go command break bug fix ? (loop: bra loop:8)
; 1998/6/7   :   debugging mode direct in
; 1998/6/27 0.52 TRAPA support
; 1998/7/12 0.54 change register
; 1999/10/16 0.61 debugger work size valiable
; 2000/5/27 0.62 view list, view trace
;

	;**********************
	;* debugger mode stop *
	;**********************

dbg_stp:
	mov.l	#dbg_stp_msg,er1
	bsr	puts
	mov.l	#int_end_dbg_jmp,er1
	mov.l	@er1,er1
;*	mov.l	er1,@int_end
	mov.l	er1,@startof DEBUG_RAM + x_int_end		;* 99/10/16
	orc	#B'00000001,ccr
	rts

	;***********************
	;* debugger mode start *
	;***********************

dbg_sta:
	mov.l	#int_end_lop_jmp,er1
	mov.l	@er1,er1
;*	mov.l	er1,@int_end
	mov.l	er1,@startof DEBUG_RAM + x_int_end		;* 99/10/16
_dbg_sta
	mov.l	#dbg_sta_msg,er1
	bsr	puts		; continue next line !

	;*******************************
	;* dump register & instruction *
	;*******************************

dmp_ipr:
	bsr	dmp_reg:8
	bsr	putc_crlf
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.l	@er1,er1
	sub.l	er2,er2
	inc.l	#1,er2
	bra	dis_asm_0

	;*****************
	;* dump register *
	;*****************

dmp_reg:
	mov.l	#pc_msg,er1
	bsr	puts		;"PC="
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0,er0
	push.w	e0
	bsr	putc_ha		;"xxxxxx"
	bsr	puts		;"  CCR="
	pop.w	r0
	mov.b	r0h,r0l
	bsr	putc_hb		;"xx"
	mov.b	@er1+,r0l
	bsr	putc		;":"
	orc	#B'00000001,ccr
	rotxl.b	r0h
dbg_sta_1
	mov.b	@er1+,r0l
	bcs	dbg_sta_1a:8
	mov.b	#".",r0l
dbg_sta_1a
	bsr	putc		;"????????"
	shll.b	r0h
	bne	dbg_sta_1:8
;*	mov.l	#usr_rg,er2
	mov.l	#startof DEBUG_RAM + x_usr_rg,er2		;* 99/10/16
dbg_sta_2
	bsr	puts		;<CR><LF>"ER0="
	mov.l	@er2+,er0
	bsr	putc_hl
;*	cmp.l	#usr_sp,er2
	cmp.l	#startof DEBUG_RAM + x_usr_sp,er2		;* 99/10/16
	bls	dbg_sta_2:8
	orc	#B'00000001,ccr
	rts

	;*****************************
	;* program counter break set *
	;*****************************

pcb_set:
	bsr	putc_m		;">PB-"
	mov.l	#step_scn,er1
	bsr	fscan
	bcs	pcb_set_0:8
	rts
pcb_set_0
;*	mov.l	er0,@brk_adr
	mov.l	er0,@startof DEBUG_RAM + x_brk_adr		;* 99/10/16
	mov.l	#pcb_set_msg,er1
	bra	pcb_dsp_0:8

	;*************************
	;* program counter clear *
	;*************************

pcb_clr:
	mov.l	#pcb_clr_msg,er1
	bsr	puts
	sub.l	er0,er0
;*	mov.l	er0,@brk_adr
	mov.l	er0,@startof DEBUG_RAM + x_brk_adr		;* 99/10/16
	orc	#B'00000001,ccr
	rts

	;******************************
	;* disp program counter break *
	;******************************

pcb_dsp:
	mov.l	#pcb_dsp_msg,er1
pcb_dsp_0
	bsr	puts
;*	mov.l	@brk_adr,er0
	mov.l	@startof DEBUG_RAM + x_brk_adr,er0		;* 99/10/16
	bsr	putc_ha
	orc	#B'00000001,ccr
	rts

	;*******************
	;* change register *
	;*******************

chn_reg:
	bsr	putc_m		;">CR-"
	bsr	getche
	bsr	toupper
	cmp.b	#"P",r0l	; PC
	beq	chn_reg_P:8
	cmp.b	#"C",r0l	; CCR
	beq	chn_reg_C:8
	cmp.b	#"E",r0l	; ERn,En
	beq	chn_reg_E:8
	cmp.b	#"R",r0l	; Rn
	beq	chn_reg_R ;:8
chn_reg_0
	andc	#B'11111110,ccr
chn_reg_1
	rts
	;* PC *
chn_reg_P
	bsr	getche
	bsr	toupper
	cmp.b	#"C",r0l
	bne	chn_reg_0:8
	bsr	getche
	cmp.b	#H'0d,r0l
	bne	chn_reg_0:8
	bsr	chn_reg_PC
	beq	chn_reg_1:8
chn_reg_2
	bsr	chn_reg_CCR
	beq	chn_reg_1:8
	mov.b	#0,r0l
chn_reg_3
	push.w	r0
	bsr	chn_reg_ERn
	beq	chn_reg_4:8
	pop.w	r0
	inc.b	r0l
	cmp.b	#7,r0l
	bls	chn_reg_3:8
	orc	#B'00000001,ccr
	rts
chn_reg_4
	pop.w	r0
	rts
	;* CCR *
chn_reg_C
	bsr	getche
	bsr	toupper
	cmp.b	#"C",r0l
	bne	chn_reg_0:8
	bsr	getche
	bsr	toupper
	cmp.b	#"R",r0l
	bne	chn_reg_0:8
	bsr	getche
	cmp.b	#H'0d,r0l
	bne	chn_reg_0:8
	bra	chn_reg_2:8
	;* ERn,En *
chn_reg_E
	bsr	getche
	bsr	toupper
	cmp.b	#"0",r0l
	blo	chn_reg_0:8
	cmp.b	#"7",r0l
	bls	chn_reg_5:8
	cmp.b	#"R",r0l
	bne	chn_reg_0:8
	;* ERn *
	bsr	getche
	cmp.b	#"0",r0l
	blo	chn_reg_0:8
	cmp.b	#"7",r0l
	bhs	chn_reg_1:8
	mov.b	r0l,r0h
	bsr	getche
	cmp.b	#H'0d,r0l
	bne	chn_reg_0 ;:8
	mov.b	r0h,r0l
	add.b	#-"0",r0l
	bra	chn_reg_3:8
	;* En *
chn_reg_5
	mov.b	r0l,r0h
	bsr	getche
	cmp.b	#H'0d,r0l
	bne	chn_reg_0 ;:8
	mov.b	r0h,r0l
	add.b	#-"0",r0l
chn_reg_6
	push.w	r0
	bsr	chn_reg_En
	beq	chn_reg_7:8
	pop.w	r0
	inc.b	r0l
	cmp.b	#7,r0l
	bls	chn_reg_6:8
	orc	#B'00000001,ccr
	rts
chn_reg_7
	pop.w	r0
	rts
	;* Rn *
chn_reg_R
	bsr	getche
	cmp.b	#"0",r0l
	blo	chn_reg_0 ;:8
	cmp.b	#"7",r0l
	bhs	chn_reg_1 ;:8
	mov.b	r0l,r0h
	bsr	getche
	cmp.b	#H'0d,r0l
	bne	chn_reg_0 ;:8
	mov.b	r0h,r0l
	add.b	#-"0",r0l
chn_reg_8
	push.w	r0
	bsr	chn_reg_Rn
	beq	chn_reg_9:8
	pop.w	r0
	inc.b	r0l
	cmp.b	#7,r0l
	bls	chn_reg_8:8
	orc	#B'00000001,ccr
	rts
chn_reg_9
	pop.w	r0
	rts

chn_reg_PC:
	mov.l	#pc_msg,er1	;"PC="
	bsr	puts
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0,er0
	bsr	putc_ha		;"xxxxxx"
	bsr	putc_m
	mov.b	#6,r0l
	bsr	getval_hc2b_n
	bcc	chn_reg_PC_0:8
	bsr	getche
chn_reg_PC_0
	cmp.b	#H'0d,r0l
	bne	chn_reg_PC_1:8
	cmp.b	#0,r0h
	bne	chn_reg_PC_4:8
	orc	#B'00000001,ccr
	rts	; end
chn_reg_PC_1
	cmp.b	#" ",r0l
	beq	chn_reg_PC_2:8
	cmp.b	#".",r0l
	beq	chn_reg_PC_3:8
	mov.b	#0,r0l
	andc	#B'11111110,ccr
	rts	; error
chn_reg_PC_2
	bsr	putc_bs
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0,er0
	bsr	putc_ha		;"xxxxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next
chn_reg_PC_3
	mov.b	#0,r0l
	orc	#B'00000001,ccr
	rts	; end
chn_reg_PC_4
	bsr	putc_bs
	dec.b	r0h
	bne	chn_reg_PC_4:8
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	er1,@er0
	mov.l	er1,er0
	bsr	putc_ha		;"xxxxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next

chn_reg_CCR:
	mov.l	#ccr_msg,er1	;"CCR="
	bsr	puts
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.b	@er0,r0l
	bsr	putc_hb		;"xx"
	bsr	putc_m
	mov.b	#6,r0l
	bsr	getval_hc2b_n
	bcc	chn_reg_CCR_0:8
	bsr	getche
chn_reg_CCR_0
	cmp.b	#H'0d,r0l
	bne	chn_reg_CCR_1:8
	cmp.b	#0,r0h
	bne	chn_reg_CCR_4:8
	orc	#B'00000001,ccr
	rts	; end
chn_reg_CCR_1
	cmp.b	#" ",r0l
	beq	chn_reg_CCR_2:8
	cmp.b	#".",r0l
	beq	chn_reg_CCR_3:8
	mov.b	#0,r0l
	andc	#B'11111110,ccr
	rts	; error
chn_reg_CCR_2
	bsr	putc_bs
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.b	@er0,r0l
	bsr	putc_hb		;"xx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next
chn_reg_CCR_3
	mov.b	#0,r0l
	orc	#B'00000001,ccr
	rts	; end
chn_reg_CCR_4
	bsr	putc_bs
	dec.b	r0h
	bne	chn_reg_CCR_4:8
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.b	r1l,@er0
	mov.b	r1l,r0l
	bsr	putc_hb		;"xx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next

chn_reg_ERn:
	mov.b	r0l,r1l
	mov.b	#7,r1h
	mulxu.b	r1h,r1
	extu.l	er1
	add.l	#er_msg,er1	;"ERn="
	sub.l	er2,er2
	mov.b	r0l,r2l
	shll.l	er2
	shll.l	er2
;*	add.l	#usr_rg,er2
	add.l	#startof DEBUG_RAM + x_usr_rg,er2		;* 99/10/16
	bsr	puts
	mov.l	@er2,er0
	bsr	putc_hl		;"xxxxxxxx"
	bsr	putc_m
	mov.b	#8,r0l
	bsr	getval_hc2b_n
	bcc	chn_reg_ERn_0:8
	bsr	getche
chn_reg_ERn_0
	cmp.b	#H'0d,r0l
	bne	chn_reg_ERn_1:8
	cmp.b	#0,r0h
	bne	chn_reg_ERn_4:8
	orc	#B'00000001,ccr
	rts	; end
chn_reg_ERn_1
	cmp.b	#" ",r0l
	beq	chn_reg_ERn_2:8
	cmp.b	#".",r0l
	beq	chn_reg_ERn_3:8
	mov.b	#0,r0l
	andc	#B'11111110,ccr
	rts	; error
chn_reg_ERn_2
	bsr	putc_bs
	mov.l	@er2,er0
	bsr	putc_hl		;"xxxxxxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next
chn_reg_ERn_3
	mov.b	#0,r0l
	orc	#B'00000001,ccr
	rts	; end
chn_reg_ERn_4
	bsr	putc_bs
	dec.b	r0h
	bne	chn_reg_ERn_4:8
	mov.l	er1,@er2
	mov.l	er1,er0
	bsr	putc_hl		;"xxxxxxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next

chn_reg_En:
	mov.l	#e_msg,er1
	mov.b	r0l,r0h
	shll.b	r0l		;0,1,2,3... -> 0,2,4,6...
_chn_reg_En
	sub.l	er2,er2
	mov.b	r0h,r2l
	shll.l	er2
	shll.l	er2
	add.l	er2,er1
	sub.l	er2,er2
	mov.b	r0l,r2l
	shll.l	er2
;*	add.l	#usr_rg,er2
	add.l	#startof DEBUG_RAM + x_usr_rg,er2		;* 99/10/16
	bsr	puts
	mov.w	@er2,r0
	bsr	putc_hw		;"xxxx"
	bsr	putc_m
	mov.b	#4,r0l
	bsr	getval_hc2b_n
	bcc	chn_reg_En_0:8
	bsr	getche
chn_reg_En_0
	cmp.b	#H'0d,r0l
	bne	chn_reg_En_1:8
	cmp.b	#0,r0h
	bne	chn_reg_En_4:8
	orc	#B'00000001,ccr
	rts	; end
chn_reg_En_1
	cmp.b	#" ",r0l
	beq	chn_reg_En_2:8
	cmp.b	#".",r0l
	beq	chn_reg_En_3:8
	mov.b	#0,r0l
	andc	#B'11111110,ccr
	rts	; error
chn_reg_En_2
	bsr	putc_bs
	mov.w	@er2,r0
	bsr	putc_hw		;"xxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next
chn_reg_En_3
	mov.b	#0,r0l
	orc	#B'00000001,ccr
	rts	; end
chn_reg_En_4
	bsr	putc_bs
	dec.b	r0h
	bne	chn_reg_En_4:8
	mov.w	r1,@er2
	mov.w	r1,r0
	bsr	putc_hw		;"xxxx"
	bsr	putc_crlf
	mov.b	#1,r0l
	rts	; next

chn_reg_Rn:
	mov.l	#r_msg,er1
	mov.b	r0l,r0h
	shll.b	r0l		;0,1,2,3... -> 0,2,4,6...
	inc.b	r0l		;0,2,4,6... -> 1,3,5,7...
	bra	_chn_reg_En

	;********************
	;* break call entry *
	;********************

brk_cal:
	stc.w	ccr,@-sp
	orc	#B'11000000,ccr
	push.w	r0
	mov.b	@(2,sp),r0l
	mov.b	r0l,@(4,sp)
	pop.w	r0
	adds	#2,sp
	bra	int_txd_dbg	; cary=0 return
brk_cal_di

	;********************
	;* break trap entry *
	;********************

brk_trp:
	rte

	;*****************
	;* debug mode in *
	;*****************

	.export	dbg_mode_in
dbg_mode_in:
	stc.w	ccr,@-sp
	orc	#B'11000000,ccr
	push.w	r0
	mov.b	@(2,sp),r0l
	mov.b	r0l,@(4,sp)
	pop.w	r0
	adds	#2,sp
	;* check *
	push	er1	;!
;*	mov.l	@dbg_wke-4,er1
	mov.l	@startof DEBUG_RAM + sizeof DEBUG_RAM -4,er1	;* 99/10/16
	cmp.l	#debug_entry,er1
	beq	dbg_mode_in_0:8
	pop	er1	;!
	rte
dbg_mode_in_0
	mov.l	#dbg_mode_in_1,er1
;*	mov.l	er1,@dbg_wke-4
	mov.l	er1,@startof DEBUG_RAM + sizeof DEBUG_RAM -4	;* 99/10/16
	pop	er1	;!
	bra	int_txd_dbg	; cary=0 return
dbg_mode_in_1
	mov.l	#int_end_lop_jmp,er1
	mov.l	@er1,er1
;*	mov.l	er1,@int_end
	mov.l	er1,@startof DEBUG_RAM + x_int_end		;* 99/10/16
	mov.l	#debug_entry_msg,er1
	bsr	puts
	bsr	putc_crlf
	mov.l	#debug_prompt,er1	; return address set
	push	er1			;
	bra	_dbg_sta

	;******
	;* go *
	;******

go:
	bsr	step_sub
	bcc	step_1
	bsr	is_pc_break
	bcs	step_2
	bsr	get_rxd_buf					;+ 98/6/6
	bcs	step_1						;+ 98/6/6
	bra	go:8

	;*************
	;* view list *
	;*************
	;  _____________
	; |             
	; | list 19 line - current line 9
	; |
	; |============= < space
	; | regs 3 line 
	; |_____________ < command line
	; total 24 line
vew_lst:
	bsr	putc_crlf
_vew_lst
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1
	mov.l	@er1,er1
	and.w	#H'ff,e1
	push	er1	;!
	mov.b	#10*8,r0l	; max 10 byte * 8 step
vew_lst_0
	mov.b	#0,r0h
	push.w	r0	;!!	; @sp.b = count, @sp+1.b = start offset
	extu.l	er0
	sub.l	er0,er1		; er1 = dis assemble start address
vew_lst_1
	mov.l	@(2,sp),er0
	cmp.l	er0,er1
	beq	vew_lst_2:8	; match !
	blo	vew_lst_1a:8	; next !
	;* over *
	pop.w	r0	;!!
	add.b	#-2,r0l
	mov.l	@sp,er1	;!
	bra	vew_lst_0:8
	;* next *
vew_lst_1a
	mov.l	er1,er1
	bmi	vew_lst_1b:8	; miss !
	bsr	dis_asm_sub
	bcc	vew_lst_1b:8	; error !
	bsr	dis_asm_siz	;<ret> r0l=size
	bra	vew_lst_1c:8
vew_lst_1b
	mov.b	#2,r0l		; error then (.data.w H'xxxx)
vew_lst_1c
	mov.b	@sp,r0h
	inc.b	r0h
	mov.b	r0h,@sp
	extu.w	r0
	extu.l	er0
	add.l	er0,er1
	bra	vew_lst_1:8
	;* disp *
vew_lst_2
	pop.w	r0	;!!
	pop	er1	;!
	sub.l	er2,er2
	mov.b	r0l,r2l
	sub.l	er2,er1
	bsr	vew_lst_skp	;r0l=(0..7),8..n
	mov.b	#" ",r0l
	bsr	vew_lst_dsp
	mov.b	#1,r0h
	mov.b	#">",r0l
	bsr	vew_lst_dsp
	mov.b	#10,r0h
	mov.b	#" ",r0l
	bsr	vew_lst_dsp
	bsr	putc_cll
	bsr	putc_crlf
	bra	dmp_reg

vew_lst_skp:	;<ent> r0h=0..7,8..n, er1=address
	cmp.b	#8,r0h
	beq	vew_lst_skp_1:8		; ok !
	blo	vew_lst_skp_2:8
	;* 8 < r0h *
	add.b	#-8,r0h
vew_lst_skp_0
	push.w	r0
	mov.l	er1,er1
	bmi	vew_lst_skp_0a:8	; miss !
	bsr	dis_asm_sub
	bcc	vew_lst_skp_0a:8	; error !
	bsr	dis_asm_siz
	bra	vew_lst_skp_0b:8
vew_lst_skp_0a
	mov.b	#2,r0l
vew_lst_skp_0b
	extu.w	r0
	extu.l	er0
	add.l	er0,er1
	pop.w	r0
	dec.b	r0h
	bne	vew_lst_skp_0;8
	mov.b	#8,r0h
vew_lst_skp_1
	rts
	;* r0h < 8 *
vew_lst_skp_2
	mov.b	#8,r0l
	sub.b	r0h,r0l
vew_lst_skp_2a
	push.w	r0
	bsr	putc_cll
	bsr	putc_crlf
	pop.w	r0
	dec.b	r0l
	bne	vew_lst_skp_2a:8
	rts

putc_cll:
	mov.b	#H'1b,r0l
	bsr	putc
	mov.b	#"[",r0l
	bsr	putc
	mov.b	#"K",r0l
	bra	putc

vew_lst_dsp:	;<ent> r0l=1st char, r0h=line, er1=address
	push.w	r0
	bsr	putc		; ">" or " "
	mov.l	er1,er0
	bsr	putc_ha
	mov.b	#8,r0l
	bsr	putc_sp_n
	mov.l	er1,er1
	bmi	vew_lst_dsp_0:8	; miss !
	bsr	dis_asm_sub
	bcc	vew_lst_dsp_0:8	; error !
	;* opc,opr *
	push.w	r0
	push	er1
	bsr	dis_asm_opc
	mov.w	@(4,sp),r0
	mov.l	@sp,er1
	bsr	dis_asm_opr
	pop	er1
	pop.w	r0
	bsr	dis_asm_siz
	extu.w	r0
	extu.l	er0
	add.l	er0,er1
	bra	vew_lst_dsp_1:8
	;* .data.w H'xxxx *
vew_lst_dsp_0
	push	er1
	mov.l	#vew_lst_dsp_dat,er1
	bsr	puts
	pop	er1
	mov.w	@er1+,r0
	bsr	putc_hw
vew_lst_dsp_1
	bsr	putc_cll
	bsr	putc_crlf
	pop.w	r0
	dec.b	r0h
	bne	vew_lst_dsp:8
	rts
vew_lst_dsp_dat
	.sdataz	".DATA.W   H'"
	.align	2

	;**************
	;* view trace *
	;**************

vew_trc:
	mov.l	#vew_trc_msg,er1
_vew_trc
	bsr	puts
	bsr	step_sub
	bcc	step_1
	bra	_vew_lst

vew_trc_msg
	.sdata	<H'0d><H'1b>"[K"
vew_trc_msg_0
	.sdataz	<H'1b>"[24A"
	.align	2

	;*************
	;* view step *
	;*************

vew_stp:
	mov.b	#H'0d,r0l
	bsr	putc
	bsr	putc_cll
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1
	mov.l	@er1,er1
	and.w	#H'ff,e1
	push	er1	;!
	bsr	dis_asm_sub
	bcc	vew_stp_1:8	; error !
	bsr	vew_stp_cal
	bcc	vew_stp_4:8	; normal !
	bsr	dis_asm_siz
	mov.l	@sp,er1	;!
	extu.w	r0
	extu.l	er0
	add.l	er0,er1
	mov.l	er1,@sp	;!
vew_stp_0
	bsr	step_sub
	bcc	vew_stp_1:8	; error !
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1
	mov.l	@er1,er1
	and.w	#H'ff,e1
	mov.l	@sp,er0	;!
	cmp.l	er0,er1
	beq	vew_stp_3:8	; ok !
	bsr	is_pc_break
	bcs	vew_stp_2:8	; pc break !
	bsr	get_rxd_buf
	bcc	vew_stp_0:8
	;* error or break *
vew_stp_1
	adds	#4,sp
	bra	step_1
	;* pc break *
vew_stp_2
	adds	#4,sp
	bra	step_2
	;* ok *
vew_stp_3
	adds	#4,sp
	mov.l	#vew_trc_msg_0,er1
	bsr	puts
	bra	_vew_lst
	;* jump code *
vew_stp_4
	adds	#4,sp
	mov.l	#vew_trc_msg_0,er1
	bra	_vew_trc

vew_stp_cal:
	cmp.b	#BRA,r0h
	bls	vew_stp_cal_0:8	; normal !
	cmp.b	#BLE,r0h
	bls	vew_stp_cal_1:8	; branch !
	cmp.b	#BSR,r0h
	beq	vew_stp_cal_1:8	; call !
	cmp.b	#JSR,r0h
	beq	vew_stp_cal_1:8	; call !
	cmp.b	#TRAPA,r0h
	beq	vew_stp_cal_1:8	; call !
vew_stp_cal_0
	andc	#B'11111110,ccr
	rts
vew_stp_cal_1
	orc	#B'00000001,ccr
	rts

	;*************
	;* one trace *
	;*************

one_trace:
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.l	@er1,er1
	and.w	#H'ff,e1
	push	er1
	bsr	step_sub
	pop	er1
	bcc	step_1 ;:8	; break
	sub.l	er2,er2
	inc.l	#1,er2
	bra	dis_asm_0

	;*********
	;* trace *
	;*********

trace:
	bsr	putc_m		;"*TR-"
	mov.l	#step_scn,er1
	bsr	fscan
	bcs	trace_0:8
	rts
	;* loop *
trace_0
	push	er0
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.l	@er1,er1
	and.w	#H'ff,e1
	push	er1
	bsr	step_sub
	pop	er1
	pop	er0
	bcc	step_1 ;:8	; break
	push	er0
	push	er1
;t	mov.b	#"#",r0l					;+ test 98/6/6
;t	bsr	putc						;+ test 98/6/6
	bsr	get_rxd_buf
	pop	er1
	pop	er0
	bcs	step_1 ;:8	; break
	push	er0
	sub.l	er2,er2
	inc.l	#1,er2
	bsr	dis_asm_0
	bsr	putc_crlf
	pop	er0
	dec.l	#1,er0
	beq	dmp_ipr		; end
	push	er0
	bsr	is_pc_break
	pop	er0
	bcs	step_2:8	; break
	bra	trace_0:8

;trace:
;*;	mov.l	@usr_sp,er1
;	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
;	mov.l	@er1,er1
;	and.w	#H'ff,e1
;	push	er1
;	bsr	step_sub
;	pop	er1
;	bcc	step_1 ;:8	; break
;	push	er1
;	bsr	get_rxd_buf
;	pop	er1
;	bcs	step_1 ;:8	; break
;	sub.l	er2,er2
;	inc.l	#1,er2
;	bsr	dis_asm_0
;	bcc	trace_0:8
;	bsr	putc_crlf
;	bra	one_trace:8
;trace_0
;	rts

	;************
	;* one step *
	;************

one_step:
	sub.l	er0,er0
	inc.l	#1,er0
	bra	step_0:8

	;********
	;* step *
	;********

step:
	bsr	putc_m		;"*ST-"
	mov.l	#step_scn,er1
	bsr	fscan
	bcs	step_0:8
	rts
	;* loop *
step_0
	push	er0
	bsr	step_sub ;:8
	pop	er0
	bcc	step_1:8	; break
	dec.l	#1,er0
	beq	dmp_ipr		; end
	push	er0
	bsr	is_pc_break:8
	pop	er0
	bcs	step_2:8	; break
	push	er0						;+ 98/6/6 ..
	bsr	get_rxd_buf
	pop	er0
	bcs	step_1:8	; break
	push	er0						;.. + 98/6/6
	bsr	dmp_ipr
	bsr	putc_crlf
	pop	er0
	bra	step_0:8
	;* break *
step_1
	bsr	get_rxd_buf
	mov.l	#break_msg,er1
	bsr	puts
	bra	dmp_ipr		; break
	;* program counter break *
step_2
	mov.l	#pc_break_msg,er1
	bsr	puts
	bra	dmp_ipr		; break
step_scn
	.sdataz	"%x\r"
	.align	2

is_pc_break:
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0,er0
	and.w	#H'ff,e0
;*	mov.l	@brk_adr,er1
	mov.l	@startof DEBUG_RAM + x_brk_adr,er1		;* 99/10/16
	cmp.l	er0,er1
	beq	is_pc_break_0:8
	andc	#B'11111110,ccr
	rts
is_pc_break_0
	orc	#B'00000001,ccr
	rts

step_sub:
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.l	@er1,er1
	bsr	dis_asm_sub
	bcs	step_sub_0:8
	rts	;(error)
step_sub_0
	cmp.b	#BRA,r0h
	blo	step_sub_0a:8
	cmp.b	#BLE,r0h
	bls	step_sub_bcc
	cmp.b	#BSR,r0h
	beq	step_sub_cal
	cmp.b	#JSR,r0h
	beq	step_sub_cal
	cmp.b	#JMP,r0h
	beq	step_sub_jmp
	cmp.b	#RTE,r0h
	beq	step_sub_rte
	cmp.b	#RTS,r0h
	beq	step_sub_rts
	cmp.b	#TRAPA,r0h
	beq	step_sub_trapa
step_sub_brn_chk
step_sub_0a
	bsr	dis_asm_siz
	push.w	r0
;*	mov.l	#stp_wrk,er2
	mov.l	#startof DEBUG_RAM + x_stp_wrk,er2		;* 99/10/16
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	xor.w	e2,r2
	xor.w	r2,e2
	xor.w	e2,r2
	mov.b	r2l,@(1,er0)
	xor.w	e2,r2
	xor.w	r2,e2
	xor.w	e2,r2
	mov.w	r2,@(2,er0)
	pop.w	r0
	push	er1	;!		; @sp = execute code address
step_sub_1
	mov.b	@er1+,r0h
	mov.b	r0h,@er2
	adds	#1,er2
	dec.b	r0l
	bne	step_sub_1:8
	mov.l	#step_sub_jsr_code,er0
	mov.l	@er0,er0
	mov.l	er0,@er2		; er2 = jsr code address
step_sub_2
	bsr	int_end_dbg
	bcc	step_sub_2a:8
	;* break ! *
	push	er1
	bsr	put_rxd_buf
	pop	er1
step_sub_2a
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0,er0
	and.w	#H'ff,e0
;*	cmp.l	#stp_wrk,er0
	cmp.l	#startof DEBUG_RAM + x_stp_wrk,er0		;* 99/10/16
	beq	step_sub_2b:8	; exit
	cmp.l	er2,er0
	beq	step_sub_3:8		; end & exit
	adds	#4,er2
	cmp.l	er2,er0
	beq	step_sub_2c:8	; end
	cmp.l	#brk_cal,er0
	blo	step_sub_2c:8	; error !
	cmp.l	#brk_cal_di,er0
	blo	step_sub_2:8		; continue !
	bra	step_sub_3:8		; error !
step_sub_2b
	mov.l	@sp,er1
	bra	step_sub_3:8
step_sub_2c
	xorc	#B'00000001,ccr
step_sub_3
	adds	#4,sp	;!
	;* save ip *
step_sub_4
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	xor.w	r1,e1
	xor.w	e1,r1
	xor.w	r1,e1
	mov.b	r1l,@(1,er0)
	mov.w	e1,@(2,er0)
	rts
step_sub_jsr_code
	jsr	@brk_cal
	;* Bcc *
step_sub_bcc
	push.w	r0
	add.b	#-BRA,r0h
	mov.b	r0h,r0l
	extu.w	r0
	extu.l	er0
	shll.l	er0
	mov.l	#step_sub_bcc_tbl,er2
	add.l	er2,er0
	mov.w	@er0,r0
	exts.l	er0
	add.l	er0,er2
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.w	@er0,e0		; e0h=ccr
	or.w	#H'c000,e0	; di & udi
	pop.w	r0
	push.w	e0
	ldc.w	@sp+,ccr
	jmp	@er2
	;* BSR,JSR *
step_sub_cal
	push.w	r0
	bsr	dis_asm_siz
	sub.l	er2,er2
	mov.b	r0l,r2l
	add.l	er1,er2
	pop.w	r0
	bsr	step_sub_adr
	bcc	step_sub_cal_0:8
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.w	r2,@(2,er1)
	mov.w	e2,r2
	mov.b	r2l,@(1,er1)
	mov.b	@er1,r2h
	mov.b	#0,r2l
	mov.b	r2l,@er1
	or.w	r2,e0
	mov.l	er0,@-er1
;*	mov.l	er1,@usr_sp
	mov.l	er1,@startof DEBUG_RAM + x_usr_sp		;* 99/10/16
	bsr	int_rxd_chk					;+ 98/6/6 ..
	xorc	#B'00000001,ccr
	bcs	step_sub_cal_0:8
	bsr	put_rxd_buf					;.. + 98/6/6
;*	orc	#B'00000001,ccr
	andc	#B'11111110,ccr					;* 98/6/6
step_sub_cal_0
	rts
	;* BRA,JMP *
step_sub_bra_chk
step_sub_jmp
	bsr	step_sub_adr
	bcc	step_sub_cal_0:8
	mov.l	er0,er1
	bsr	int_rxd_chk					;+ 98/6/6 ..
	xorc	#B'00000001,ccr
	bcs	step_sub_4
	push	er1
	bsr	put_rxd_buf
	pop	er1						;.. + 98/6/6
;*	orc	#B'00000001,ccr
	andc	#B'11111110,ccr					;* 98/6/6
	bra	step_sub_4
	;* RTE *
step_sub_rte
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	adds	#4,er0
;*	mov.l	er0,@usr_sp
	mov.l	er0,@startof DEBUG_RAM + x_usr_sp		;* 99/10/16
	bsr	int_rxd_chk					;+ 98/6/6 ..
	xorc	#B'00000001,ccr
	bcs	step_sub_rte_0:8
	bsr	put_rxd_buf
	andc	#B'11111110,ccr
step_sub_rte_0							;.. + 98/6/6
;-	orc	#B'00000001,ccr					;- 98/6/6
	rts
	;* RTS *
step_sub_rts
;*	mov.l	@usr_sp,er0
	mov.l	@startof DEBUG_RAM + x_usr_sp,er0		;* 99/10/16
	mov.l	@er0+,er2
	mov.l	@er0+,er1
	mov.l	er2,@-er0
;*	mov.l	er0,@usr_sp
	mov.l	er0,@startof DEBUG_RAM + x_usr_sp		;* 99/10/16
	bsr	int_rxd_chk					;+ 98/6/6 ..
	xorc	#B'00000001,ccr
	bcs	step_sub_4
	push	er1
	bsr	put_rxd_buf
	pop	er1						;.. + 98/6/6
;*	orc	#B'00000001,ccr
	andc	#B'11111110,ccr					;* 98/6/6
	bra	step_sub_4
	;* TRAPA *
step_sub_trapa
	push.w	r0
	bsr	dis_asm_siz
	sub.l	er2,er2
	mov.b	r0l,r2l
	add.l	er1,er2
	pop.w	r0
	bsr	step_sub_adr
	bcc	step_sub_trapa_0:8
;*	mov.l	@usr_sp,er1
	mov.l	@startof DEBUG_RAM + x_usr_sp,er1		;* 99/10/16
	mov.w	r2,@(2,er1)
	mov.w	e2,r2
	mov.b	r2l,@(1,er1)
	mov.b	@er1,r2h
	or.b	#B'10000000,r2h
	bild	#3,@SYSCR
	bst	#6,r2h
	mov.b	#0,r2l
	or.w	r2,e0
	mov.l	er0,@-er1
;*	mov.l	er1,@usr_sp
	mov.l	er1,@startof DEBUG_RAM + x_usr_sp		;* 99/10/16
	bsr	int_rxd_chk					;+ 98/6/6 ..
	xorc	#B'00000001,ccr
	bcs	step_sub_trapa_0:8
	bsr	put_rxd_buf					;.. + 98/6/6
;*	orc	#B'00000001,ccr
	andc	#B'11111110,ccr					;* 98/6/6
step_sub_trapa_0
	rts
	;* address *
step_sub_adr	;<ent> r0l=opr_no,er1=ip <ret> cary=1:er0=address.0:error
	mov.b	#3,r0h
	mulxu.b	r0h,r0
	extu.l	er0
	add.l	#opr_tbl,er0
	adds	#2,er0
	mov.b	@er0,r0l
	cmp.b	#H'05,r0l
	beq	step_sub_adr_05:8
	cmp.b	#H'06,r0l
	beq	step_sub_adr_06:8
	cmp.b	#H'07,r0l
	beq	step_sub_adr_07:8
	cmp.b	#H'08,r0l
	beq	step_sub_adr_08:8
	cmp.b	#H'82,r0l
	beq	step_sub_adr_82:8
	cmp.b	#H'f2,r0l
	beq	step_sub_adr_f2:8
	andc	#B'11111110,ccr
	rts
	;* @@aa:8 *
step_sub_adr_05
	mov.w	@er1,r0
	extu.w	r0
	extu.l	er0
	mov.l	@er0,er0
	orc	#B'00000001,ccr
	rts
	;* d:8 *
step_sub_adr_06
	mov.w	@er1,r0
	exts.w	r0
	exts.l	er0
	add.l	er1,er0
	adds	#2,er0
	orc	#B'00000001,ccr
	rts
	;* d:16 *
step_sub_adr_07
	mov.w	@(2,er1),r0
	exts.l	er0
	add.l	er1,er0
	adds	#4,er0
	orc	#B'00000001,ccr
	rts
	;* #xx:2 *
step_sub_adr_08
	mov.w	@er1,r0
	shlr.b	r0l
	shlr.b	r0l
	extu.w	r0
	extu.l	er0
	mov.l	@(8*4,er0),er0
	orc	#B'00000001,ccr
	rts
	;* @ER *
step_sub_adr_82
	push	er1
	bsr	opr_fnc_10_0
	pop	er1
	mov.b	r0h,r0l
	and.b	#7,r0l
	extu.w	r0
	extu.l	er0
	shll.l	er0
	shll.l	er0
;*	mov.l	@(usr_rg,er0),er0
	mov.l	@(startof DEBUG_RAM + x_usr_rg,er0),er0		;* 99/10/16
	orc	#B'00000001,ccr
	rts
	;* @aa:24 *
step_sub_adr_f2
	push	er1
	bsr	opr_fnc_50_0
	mov.b	r0h,r0l
	mov.b	#0,r0h
	mov.w	r0,e0
	mov.b	@er1+,r0h
	mov.b	@er1,r0l
	pop	er1
	orc	#B'00000001,ccr
	rts
step_sub_bcc_tbl
	.data.w	step_sub_bra_chk-step_sub_bcc_tbl
	.data.w	step_sub_brn_chk-step_sub_bcc_tbl
	.data.w	step_sub_bhi_chk-step_sub_bcc_tbl
	.data.w	step_sub_bls_chk-step_sub_bcc_tbl
	.data.w	step_sub_bcc_chk-step_sub_bcc_tbl
	.data.w	step_sub_bcs_chk-step_sub_bcc_tbl
	.data.w	step_sub_bne_chk-step_sub_bcc_tbl
	.data.w	step_sub_beq_chk-step_sub_bcc_tbl
	.data.w	step_sub_bvc_chk-step_sub_bcc_tbl
	.data.w	step_sub_bvs_chk-step_sub_bcc_tbl
	.data.w	step_sub_bpl_chk-step_sub_bcc_tbl
	.data.w	step_sub_bmi_chk-step_sub_bcc_tbl
	.data.w	step_sub_bge_chk-step_sub_bcc_tbl
	.data.w	step_sub_blt_chk-step_sub_bcc_tbl
	.data.w	step_sub_bgt_chk-step_sub_bcc_tbl
	.data.w	step_sub_ble_chk-step_sub_bcc_tbl
step_sub_bhi_chk
	bhi	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bls_chk
	bls	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bcc_chk
	bcc	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bcs_chk
	bcs	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bne_chk
	bne	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_beq_chk
	beq	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bvc_chk
	bvc	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bvs_chk
	bvs	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bpl_chk
	bpl	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bmi_chk
	bmi	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bge_chk
	bge	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_blt_chk
	blt	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_bgt_chk
	bgt	step_sub_bra_chk
	bra	step_sub_brn_chk
step_sub_ble_chk
	ble	step_sub_bra_chk
	bra	step_sub_brn_chk

dbg_sta_msg
	.sdataz	"=== debugging mode in ==="<H'0d><H'0a>
dbg_stp_msg
	.sdataz	"=== debugging mode out ==="
pc_msg	.sdata	"PC="<0>"  "
ccr_msg	.sdata	"CCR="<0>":IUHUNZVC"<H'0d><H'0a>
er_msg	.sdata	"ER0="<0>"  "
	.sdata	"ER1="<0>"  "
	.sdata	"ER2="<0>"  "
	.sdata	"ER3="<0><H'0d><H'0a>
	.sdata	"ER4="<0>"  "
	.sdata	"ER5="<0>"  "
	.sdata	"ER6="<0>"  "
	.sdata	"ER7="<0>
break_msg
	.sdataz	"user break !"<H'0d><H'0a>
pc_break_msg
	.sdataz	"program counter break !"<H'0d><H'0a>
pcb_set_msg
	.sdata	"Set "		; continue next line !
pcb_dsp_msg
	.sdataz	"PB="
pcb_clr_msg
	.sdataz	"PB clear"
e_msg	.sdata	"E0="<0>
	.sdata	"E1="<0>
	.sdata	"E2="<0>
	.sdata	"E3="<0>
	.sdata	"E4="<0>
	.sdata	"E5="<0>
	.sdata	"E6="<0>
	.sdata	"E7="<0>
r_msg	.sdata	"R0="<0>
	.sdata	"R1="<0>
	.sdata	"R2="<0>
	.sdata	"R3="<0>
	.sdata	"R4="<0>
	.sdata	"R5="<0>
	.sdata	"R6="<0>
	.sdata	"R7="<0>
	.align	2
