;
;	Program	DISASM.INC
;	Date	1998/5/4 .. 1998/5/20
;	Copyright (C) 1998 by AKIYA
;
;--- up date ---
; 
;

	;****************
	;* dis assenble *
	;****************

dis_asm:
	bsr	putc_m		;"*DA-"
	mov.l	#dis_asm_scn,er1
	bsr	fscan
	bcc	dis_asm_2:8
	mov.l	er0,er2		; er2=step
	beq	dis_asm_1:8
dis_asm_0
	push	er2
	bsr	dis_asm_sub
	pop	er2
	bcc	dis_asm_3:8
	push.w	r0		;!
	;* address *
	mov.l	er1,er0
	bsr	putc_ha
	mov.b	#2,r0l
	bsr	putc_sp_n
	;* dump *
	mov.w	@sp,r0
	bsr	dis_asm_siz
	push	er1		;!!
	push.w	r0
	bsr	puts_hb_n
	pop.w	r0
	mov.b	#10,r0h
	sub.b	r0l,r0h
	mov.b	r0h,r0l
	shll.b	r0l
	add.b	#2,r0l
	bsr	putc_sp_n
	;* nemonic *
	mov.w	@(4,sp),r0	;!
	bsr	dis_asm_opc
	;* operand *
	mov.w	@(4,sp),r0	;!
	mov.l	@sp,er1		;!!
	bsr	dis_asm_opr
	;* next *
	pop	er1		;!!
	pop.w	e0		;!
	dec.l	#1,er2
	beq	dis_asm_1:8
	bsr	putc_crlf
	mov.w	e0,r0
	bsr	dis_asm_siz
	extu.w	r0
	extu.l	er0
	add.l	er0,er1
	bra	dis_asm_0:8
dis_asm_1
	orc	#B'00000001,ccr
dis_asm_2
	rts
dis_asm_3
	;* address & data *
	mov.l	er1,er0
	bsr	putc_ha
	mov.b	#2,r0l
	bsr	putc_sp_n
	mov.b	#10,r0l
	bsr	puts_hb_n
	;* error message *
	mov.l	#dis_asm_err,er1
	bsr	puts
	bra	dis_asm_1:8
dis_asm_scn:
	.sdataz	"%x,%x\r"
dis_asm_err:
	.sdataz	"  (invalid code or address error !)"
	.align	2

dis_asm_siz:	;<ent> r0=code <ret> r0l=size <brk> er0
	mov.b	#3,r0h
	mulxu.b	r0h,r0
	extu.l	er0
	mov.b	@(opr_tbl,er0),r0l
	and.b	#H'f,r0l
	rts

dis_asm_opc:	;<ent> r0=code <ret> none. <brk> er0,er1
	push.w	r0
	sub.l	er1,er1
	mov.b	r0h,r1l
	shll.l	er1
	mov.w	@(opc_tbl,er1),r1
	exts.l	er1
	add.l	#opc_tbl,er1
	xor.w	e0,e0
	bsr	dis_asm_opc_sub:8
	pop.w	r0
	mov.b	r0l,r1l
	mov.b	#3,r1h
	mulxu.b	r1h,r1
	extu.l	er1
	mov.b	@(opr_tbl,er1),r1l
	shlr.b	r1l
	shlr.b	r1l
	and.b	#H'0c,r1l
	extu.w	r1
	extu.l	er1
	add.l	#opc_ext,er1
	bsr	dis_asm_opc_sub:8
	mov.b	#" ",r0l
dis_asm_opc_0
	cmp.w	#10,e0
	bhs	dis_asm_opc_1:8
	bsr	putc
	inc.w	#1,e0
	bra	dis_asm_opc_0:8
dis_asm_opc_1
	rts
dis_asm_opc_sub
	mov.b	@er1+,r0l
	beq	dis_asm_opc_sub_0:8
	bsr	putc
	inc.w	#1,e0
	bra	dis_asm_opc_sub:8
dis_asm_opc_sub_0
	rts

dis_asm_opr:	;<emt> r0=code,er1=address <ret> none. <brk> er0,er1
	mov.b	#3,r0h
	mulxu.b	r0h,r0
	extu.l	er0
	add.l	#opr_tbl,er0
	adds	#1,er0
	push.w	r1
	mov.b	@er0+,r1l
	mov.b	@er0,r1h
	mov.w	r1,r0
	pop.w	r1
	cmp.b	#0,r0l
	beq	dis_asm_opr_0:8
	push.w	r0
	bsr	dis_asm_opr_sub:8
	pop.w	r0
	mov.b	#",",r0l
	bsr	putc
dis_asm_opr_0
	mov.b	r0h,r0l
	beq	dis_asm_opr_1:8
	bsr	dis_asm_opr_sub:8
dis_asm_opr_1
	rts
dis_asm_opr_sub	;<ent> r0l=code, er1=address <ret> none. <brk> er0
	push	er1
	push	er1
	mov.b	r0l,r1l
	cmp.b	#H'08,r1l
	bls	dis_asm_opr_sub_0:8
	shlr.b	r1l
	shlr.b	r1l
	shlr.b	r1l
	shlr.b	r1l
	add.b	#H'08,r1l
dis_asm_opr_sub_0
	extu.w	r1
	extu.l	er1
	shll.l	er1
	mov.w	@(opr_fnc,er1),r1
	bmi	dis_asm_opr_sub_1:8
	beq	dis_asm_opr_sub_2:8
	extu.l	er1
	add.l	#opr_fnc,er1
	mov.l	er1,@(4,sp)
	pop	er1
	rts	;(jump)
dis_asm_opr_sub_1
	exts.l	er1
	neg.l	er1
	add.l	#opr_fnc,er1
	bsr	puts
dis_asm_opr_sub_2
	adds	#4,sp
	pop	er1
	rts
opr_fnc:	.data.w	0
		.data.w	-(opr_01s-opr_fnc)
		.data.w	-(opr_02s-opr_fnc)
		.data.w	-(opr_03s-opr_fnc)
		.data.w	-(opr_04s-opr_fnc)
		.data.w	opr_fnc_05-opr_fnc
		.data.w	opr_fnc_06-opr_fnc
		.data.w	opr_fnc_07-opr_fnc
		.data.w	opr_fnc_08-opr_fnc
		.data.w	opr_fnc_10-opr_fnc
		.data.w	opr_fnc_20-opr_fnc
		.data.w	opr_fnc_30-opr_fnc
		.data.w	opr_fnc_40-opr_fnc
		.data.w	opr_fnc_50-opr_fnc
		.data.w	opr_fnc_60-opr_fnc
		.data.w	opr_fnc_70-opr_fnc
		.data.w	opr_fnc_80-opr_fnc
		.data.w	opr_fnc_90-opr_fnc
		.data.w	opr_fnc_A0-opr_fnc
		.data.w	opr_fnc_B0-opr_fnc
		.data.w	opr_fnc_C0-opr_fnc
		.data.w	opr_fnc_D0-opr_fnc
		.data.w	opr_fnc_E0-opr_fnc
		.data.w	opr_fnc_F0-opr_fnc
	.align	2
opr_fnc_05	; @@aa:8
	mov.w	@er1,r0
	mov.b	r0l,r0h
	push	er1
	mov.l	#opr_05s,er1
	bsr	puts
	mov.b	r0h,r0l
	extu.w	r0
	extu.l	er0
	bsr	putc_ha
	bsr	puts
	pop	er1
	rts
opr_fnc_06	; d:8
	mov.w	@er1,r0
	exts.w	r0
	exts.l	er0
	add.l	er1,er0
	adds	#2,er0
	push	er1
	mov.l	#opr_06s,er1
opr_fnc_06_0
	push	er0
	bsr	puts
	pop	er0
	bsr	putc_ha
	bsr	puts
	pop	er1
	rts
opr_fnc_07	; d:16
	mov.w	@(2,er1),r0
	exts.l	er0
	add.l	er1,er0
	adds	#4,er0
	push	er1
	mov.l	#opr_07s,er1
	bra	opr_fnc_06_0:8
opr_fnc_08	; #xx:2
	mov.w	@er1,r0
	shlr.b	r0l
	shlr.b	r0l
	shlr.b	r0l
	shlr.b	r0l
	mov.b	r0l,r0h
	push	er1
	mov.l	#opr_08s,er1
	bsr	puts
	mov.b	r0h,r0l
	bsr	putc_hb_0
	bsr	puts
	pop	er1
	rts
opr_fnc_10	; RnL/RnH
	push	er1
	bsr	opr_fnc_10_0:8
	sub.l	er1,er1
	mov.b	r0h,r1l
	shll.l	er1
	shll.l	er1
	add.l	#reg_byt,er1
	bsr	puts
	pop	er1
	rts
opr_fnc_10_0
	mov.b	r0l,r0h
	and.b	#H'f,r0h
	shlr.b	r0h
	add.b	r0h,r1l
	addx	#0,r1h
	bcc	opr_fnc_10_1:8
	inc.w	#1,e1
opr_fnc_10_1
	mov.b	@er1,r0h
	bld	#0,r0l
	bcs	opr_fnc_10_2:8
	shlr.b	r0h
	shlr.b	r0h
	shlr.b	r0h
	shlr.b	r0h
opr_fnc_10_2
	and.b	#H'f,r0h
	rts
opr_fnc_20	; En/Rn
	push	er1
	bsr	opr_fnc_10_0
	sub.l	er1,er1
	mov.b	r0h,r1l
	shll.l	er1
	shll.l	er1
	add.l	#reg_wrd,er1
	bsr	puts
	pop	er1
	rts
opr_fnc_30	; ERn
	push	er1
	bsr	opr_fnc_10_0
opr_fnc_30_0
	and.b	#H'7,r0h
	sub.l	er1,er1
	mov.b	r0h,r1l
	shll.l	er1
	shll.l	er1
	add.l	#reg_lng,er1
	bsr	puts
	pop	er1
	rts
opr_fnc_40	; #xx:3
	push	er1
	bsr	opr_fnc_10_0
	and.b	#H'7,r0h
	mov.l	#opr_40s,er1
	bsr	puts
	mov.b	r0h,r0l
	bsr	putc_hb_0
	bsr	puts
	pop	er1
	rts
opr_fnc_50	; #xx:8
	push	er1
	bsr	opr_fnc_50_0:8
	mov.l	#opr_50s,er1
	bsr	puts
	mov.b	r0h,r0l
	bsr	putc_hb
	bsr	puts
	pop	er1
	rts
opr_fnc_50_0
	mov.b	r0l,r0h
	and.b	#H'f,r0h
	shlr.b	r0h
	add.b	r0h,r1l
	addx	#0,r1h
	bcc	opr_fnc_50_1:8
	inc.w	#1,e1
opr_fnc_50_1
	mov.b	@er1+,r0h
	rts
opr_fnc_60	; #xx:16
	push	er1
	bsr	opr_fnc_50_0
	mov.b	@er1,r0l
	mov.w	r0,e0
	mov.l	#opr_60s,er1
	bsr	puts
	mov.w	e0,r0
	bsr	putc_hw
	bsr	puts
	pop	er1
	rts
opr_fnc_70	; #xx:32
	push	er1
	bsr	opr_fnc_50_0
	mov.b	@er1+,r0l
	mov.w	r0,e0
	mov.b	@er1+,r0h
	mov.b	@er1,r0l
	push	er0
	mov.l	#opr_70s,er1
	bsr	puts
	pop	er0
	bsr	putc_hl
	bsr	puts
	pop	er1
	rts
opr_fnc_80	; @ER
	push	er1
	bsr	opr_fnc_10_0
	mov.l	#opr_80s,er1
	bsr	puts
	bra	opr_fnc_30_0
opr_fnc_90	; @(d:16,ER)
	push	er1
	bsr	opr_fnc_10_0
	mov.w	r0,e0
	mov.b	#2,r0l
	bsr	opr_fnc_50_0
	mov.b	@er1+,r0l
	mov.l	#opr_90s,er1
	push.w	r0
	bsr	puts
	pop.w	r0
	bsr	putc_hw
	mov.w	e0,r0
opr_fnc_90_0
	bsr	puts
	push	er1
	sub.l	er1,er1
	mov.b	r0h,r1l
	and.b	#H'7,r1l
	shll.l	er1
	shll.l	er1
	add.l	#reg_lng,er1
	bsr	puts
	pop	er1
	bsr	puts
	pop	er1
	rts
opr_fnc_A0	; @(d:24,ER)
	push	er1
	bsr	opr_fnc_10_0
	mov.w	r0,e0
	mov.b	#6,r0l
	bsr	opr_fnc_50_0
	mov.b	@er1+,r0l
	push.w	e0
	mov.w	r0,e0
	mov.b	@er1+,r0h
	mov.b	@er1,r0l
	push.w	r0
	mov.l	#opr_A0s,er1
	bsr	puts
	pop.w	r0
	bsr	putc_ha
	pop.w	r0
	bra	opr_fnc_90_0
opr_fnc_B0	; @ER+
	push	er1
	bsr	opr_fnc_10_0
	mov.l	#opr_B0s,er1
	bra	opr_fnc_90_0
opr_fnc_C0	; @-ER
	push	er1
	bsr	opr_fnc_10_0
	mov.l	#opr_C0s,er1
	bsr	puts
	bra	opr_fnc_30_0
opr_fnc_D0	; @aa:8
	push	er1
	bsr	opr_fnc_50_0
	mov.b	r0h,r0l
	mov.b	#H'ff,r0h
	exts.l	er0
	mov.l	#opr_D0s,er1
	bra	opr_fnc_F0_0:8
opr_fnc_E0	; @aa:16
	push	er1
	bsr	opr_fnc_50_0
	mov.b	@er1,r0l
	exts.l	er0
	mov.l	#opr_E0s,er1
	bra	opr_fnc_F0_0:8
opr_fnc_F0	; @aa:24
	push	er1
	bsr	opr_fnc_50_0
	mov.b	r0h,r0l
	mov.w	r0,e0
	mov.b	@er1+,r0h
	mov.b	@er1,r0l
	mov.l	#opr_F0s,er1
opr_fnc_F0_0
	push	er0
	bsr	puts
	pop	er0
	bsr	putc_ha
	bsr	puts
	pop	er1
	rts

dis_asm_sub:	;<ent> er1=address <ret> cary=1:ok(r0=code),0:error
	bild	#0,r1l
	bcs	dis_asm_sub_0:8
	rts	; address error !
	;* 1st *
dis_asm_sub_0
	sub.l	er0,er0
	mov.b	@er1,r0l
	shlr.b	r0l
	shlr.b	r0l
	shlr.b	r0l
	bclr	#0,r0l
	mov.l	#da_x,er2
dis_asm_sub_0a
	add.l	er2,er0	;(cary=0)
dis_asm_sub_0b
	andc	#B'11111110,ccr
	mov.w	@er0,r0
	beq	dis_asm_sub_2:8
	bpl	dis_asm_sub_1:8
	exts.l	er0
	neg.l	er0
	add.l	er2,er0
	jmp	@er0
	;* ok *
dis_asm_sub_1
	orc	#B'00000001,ccr
dis_asm_sub_2
	rts

da_x			.data.w	-(da_0x_f-da_x)
			.data.w	-(da_1x_f-da_x)
da_2			.data.b	MOV,	opr_50	;@aa:8,RB
da_3			.data.b	MOV,	opr_57	;RB,@aa:8
			.data.w	-(da_4x_f-da_x)
			.data.w	-(da_5x_f-da_x)
			.data.w	-(da_6x_f-da_x)
			.data.w	-(da_7x_f-da_x)
da_8			.data.b	ADD,	opr_1	;#xx:8,RB
da_9			.data.b	ADDX,	opr_10	;#xx:8,RB
da_A			.data.b	CMP,	opr_1	;#xx:8,RB
da_B			.data.b	SUBX,	opr_10	;#xx:8.RB
da_C			.data.b	OR,	opr_1	;#xx:8,RB
da_D			.data.b	XOR,	opr_1	;#xx:8,RB
da_E			.data.b	AND,	opr_1	;#xx:8,RB
da_F			.data.b	MOV,	opr_1	;#xx:8,RB
da_0x			.data.w	-(da_00x_f-da_0x)
			.data.w	-(da_01x_f-da_0x)
			.data.w	-(da_02x_f-da_0x)
			.data.w	-(da_03x_f-da_0x)
da_04			.data.b	ORC,	opr_13	;#xx:8,CCR
da_05			.data.b	XORC,	opr_13	;#xx:8,CCR
da_06			.data.b	ANDC,	opr_13	;#xx:8,CCR
da_07			.data.b	LDC,	opr_38	;#xx:8,CCR
da_08			.data.b	ADD,	opr_2	;RB,RB
da_09			.data.b	ADD,	opr_4	;RW,RW
			.data.w	-(da_0Ax_f-da_0x)
			.data.w	-(da_0Bx_f-da_0x)
da_0C			.data.b	MOV,	opr_2	;RB,RB
da_0D			.data.b	MOV,	opr_4	;RW,RW
da_0E			.data.b	ADDX,	opr_11	;RB,RB
			.data.w	-(da_0Fx_f-da_0x)
da_1x			.data.w	-(da_10x_f-da_1x)
			.data.w	-(da_11x_f-da_1x)
			.data.w	-(da_12x_f-da_1x)
			.data.w	-(da_13x_f-da_1x)
da_14			.data.b	OR,	opr_2	;RB,RB
da_15			.data.b	XOR,	opr_2	;RB,RB
da_16			.data.b	AND,	opr_2	;RB,RB
			.data.w	-(da_17x_f-da_1x)
da_18			.data.b	SUB,	opr_2	;RB,RB
da_19			.data.b	SUB,	opr_4	;RW,RW
			.data.w	-(da_1Ax_f-da_1x)
			.data.w	-(da_1Bx_f-da_1x)
da_1C			.data.b	CMP,	opr_2	;RB,RB
da_1D			.data.b	CMP,	opr_4	;RW,RW
da_1E			.data.b	SUBX,	opr_11	;RB,RB
			.data.w	-(da_1Fx_f-da_1x)
da_40			.data.b	BRA,	opr_17	;d:8
da_41			.data.b	BRN,	opr_17	;d:8
da_42			.data.b	BHI,	opr_17	;d:8
da_43			.data.b	BLS,	opr_17	;d:8
da_44			.data.b	BCC,	opr_17	;d:8
da_45			.data.b	BCS,	opr_17	;d:8
da_46			.data.b	BNE,	opr_17	;d:8
da_47			.data.b	BEQ,	opr_17	;d:8
da_48			.data.b	BVC,	opr_17	;d:8
da_49			.data.b	BVS,	opr_17	;d:8
da_4A			.data.b	BPL,	opr_17	;d:8
da_4B			.data.b	BMI,	opr_17	;d:8
da_4C			.data.b	BGE,	opr_17	;d:8
da_4D			.data.b	BLT,	opr_17	;d:8
da_4E			.data.b	BGT,	opr_17	;d:8
da_4F			.data.b	BLE,	opr_17	;d:8
da_50			.data.b	MULXU,	opr_29	;RB,RW
da_51			.data.b	DIVXU,	opr_29	;RB,RW
			.data.w	-(da_52x_f-da_50)
			.data.w	-(da_53x_f-da_50)
			.data.w	-(da_54x_f-da_50)
da_55			.data.b	BSR,	opr_17	;d:8
			.data.w	-(da_56x_f-da_50)
			.data.w	-(da_57x_f-da_50)
			.data.w	-(da_58x_f-da_50)
			.data.w	-(da_59x_f-da_50)
da_5A			.data.b	JMP,	opr_36	;@aa:24
da_5B			.data.b	JMP,	opr_37	;@@aa:8
			.data.w	-(da_5Cx_f-da_50)
			.data.w	-(da_5Dx_f-da_50)
da_5E			.data.b	JSR,	opr_36	;@aa:24
da_5F			.data.b	JSR,	opr_37	;@@aa:8
da_60			.data.b	BSET,	opr_11	;RB,RB
da_61			.data.b	BNOT,	opr_11	;RB,RB
da_62			.data.b	BCLR,	opr_11	;RB,RB
da_63			.data.b	BTST,	opr_11	;RB,RB
da_64			.data.b	OR,	opr_4	;RW,RW
da_65			.data.b	XOR,	opr_4	;RW,RW
da_66			.data.b	AND,	opr_4	;RW,RW
da_67L			.data.b	BST,	opr_14	;#xx:3,RB
da_68L			.data.b	MOV,	opr_46	;@ER,RB
da_69L			.data.b	MOV,	opr_60	;@ER,RW
			.data.w	-(da_6Ax_f-da_60)
			.data.w	-(da_6Bx_f-da_60)
da_6CL			.data.b	MOV,	opr_49	;@ER+,RB
			.data.w	-(da_6Dx_f-da_60)
da_6EL			.data.b	MOV,	opr_47	;@(d:16,ER),RB
da_6FL			.data.b	MOV,	opr_61	;@(d:16,ER),RW
da_67H			.data.b	BIST,	opr_14	;#xx:3,RB
da_68H			.data.b	MOV,	opr_53	;RB,@ER
da_69H			.data.b	MOV,	opr_66	;RW,@ER
			.data.w	-(da_6Ax_f-(da_60+da_67H-da_67L))
			.data.w	-(da_6Bx_f-(da_60+da_67H-da_67L))
da_6CH			.data.b	MOV,	opr_56	;RB,@-ER
			.data.w	-(da_6Dx_f-(da_60+da_67H-da_67L))
da_6EH			.data.b	MOV,	opr_54	;RB,@(d:16,ER)
da_6FH			.data.b	MOV,	opr_67	;RW,@(d:16,ER)
da_70L			.data.b	BSET,	opr_14	;#xx:3,RB
			.data.w	0
da_71L			.data.b	BNOT,	opr_14	;#xx:3,RB
			.data.w	0
da_72L			.data.b	BCLR,	opr_14	;#xx:3,RB
			.data.w	0
da_73L			.data.b	BTST,	opr_14	;#xx:3,RB
			.data.w	0
da_74L			.data.b	BOR,	opr_14	;#xx:3,RB
da_74H			.data.b	BIOR,	opr_14	;#xx:3,RB
da_75L			.data.b	BXOR,	opr_14	;#xx:3,RB
da_75H			.data.b	BIXOR,	opr_14	;#xx:3,RB
da_76L			.data.b	BAND,	opr_14	;#xx:3,RB
da_76H			.data.b	BIAND,	opr_14	;#xx:3,RB
da_77L			.data.b	BLD,	opr_14	;#xx:3,RB
da_77H			.data.b	BILD,	opr_14	;#xx:3,RB
			.data.w	-(da_78x_f-da_74L)
			.data.w	-(da_79x_f-da_74L)
			.data.w	-(da_7Ax_f-da_74L)
			.data.w	-(da_7Bx_f-da_74L)
			.data.w	-(da_7Cx_f-da_74L)
			.data.w	-(da_7Dx_f-da_74L)
			.data.w	-(da_7Ex_f-da_74L)
			.data.w	-(da_7Fx_f-da_74L)
da_0000			.data.b	NOP,	opr_85	;
da_0180			.data.b	SLEEP,	opr_85	;
da_020			.data.b	STC,	opr_87	;CCR,RB
da_030			.data.b	LDC,	opr_39	;RB,CCR
da_0AHL			.data.b	ADD,	opr_6	;ER,ER
da_0A0			.data.b	INC,	opr_22	;RB
da_0B0L			.data.b	ADDS,	opr_7	;#1,ER
			.data.w	0
			.data.w	0
			.data.w	0
			.data.w	0
da_0B5			.data.b	INC,	opr_23	;#1,RW
			.data.w	0
da_0B7L			.data.b	INC,	opr_25	;#1,ER
da_0B8L			.data.b	ADDS,	opr_8	;#2,ER
da_0B9L			.data.b	ADDS,	opr_9	;#4,ER
			.data.w	0
			.data.w	0
			.data.w	0
da_0BD			.data.b	INC,	opr_24	;#2,RW
			.data.w	0
da_0BFL			.data.b	INC,	opr_26	;#2,ER
da_0FHL			.data.b	MOV,	opr_6	;ER,ER
da_0F0			.data.b	DAA,	opr_21	;RB
da_100			.data.b	SHLL,	opr_22	;RB
da_101			.data.b	SHLL,	opr_33	;RW
da_103L			.data.b	SHLL,	opr_34	;ER
da_108			.data.b	SHAL,	opr_22	;RB
da_109			.data.b	SHAL,	opr_33	;RW
da_10BL			.data.b	SHAL,	opr_34	;ER
da_110			.data.b	SHLR,	opr_22	;RB
da_111			.data.b	SHLR,	opr_33	;RW
da_113L			.data.b	SHLR,	opr_34	;ER
da_118			.data.b	SHAR,	opr_22	;RB
da_119			.data.b	SHAR,	opr_33	;RW
da_11BL			.data.b	SHAR,	opr_34	;ER
da_120			.data.b	ROTXL,	opr_22	;RB
da_121			.data.b	ROTXL,	opr_33	;RW
da_123L			.data.b	ROTXL,	opr_34	;ER
da_128			.data.b	ROTL,	opr_22	;RB
da_129			.data.b	ROTL,	opr_33	;RW
da_12BL			.data.b	ROTL,	opr_34	;ER
da_130			.data.b	ROTXR,	opr_22	;RB
da_131			.data.b	ROTXR,	opr_33	;RW
da_133L			.data.b	ROTXR,	opr_34	;ER
da_138			.data.b	ROTR,	opr_22	;RB
da_139			.data.b	ROTR,	opr_33	;RW
da_13BL			.data.b	ROTR,	opr_34	;ER
da_170			.data.b	NOT,	opr_22	;RB
da_171			.data.b	NOT,	opr_33	;RW
			.data.w	0
da_173L			.data.b	NOT,	opr_34	;ER
			.data.w	0
da_175			.data.b	EXTU,	opr_33	;RW
			.data.w	0
da_177L			.data.b	EXTU,	opr_34	;ER
da_178			.data.b	NEG,	opr_22	;RB
da_179			.data.b	NEG,	opr_33	;RW
			.data.w	0
da_17BL			.data.b	NEG,	opr_34	;ER
			.data.w	0
da_17D			.data.b	EXTS,	opr_33	;RW
			.data.w	0
da_17FL			.data.b	EXTS,	opr_34	;ER
da_1AHL			.data.b	SUB,	opr_6	;ER,ER
da_1A0			.data.b	DEC,	opr_22	;RB
da_1B0L			.data.b	SUBS,	opr_7	;#1,ER
			.data.w	0
			.data.w	0
			.data.w	0
			.data.w	0
da_1B5			.data.b	DEC,	opr_23	;#1,RW
			.data.w	0
da_1B7L			.data.b	DEC,	opr_25	;#1,ER
da_1B8L			.data.b	SUBS,	opr_8	;#2,ER
da_1B9L			.data.b	SUBS,	opr_9	;#4,ER
			.data.w	0
			.data.w	0
			.data.w	0
da_1BD			.data.b	DEC,	opr_24	;#2,RW
			.data.w	0
da_1BFL			.data.b	DEC,	opr_26	;#2,ER
da_1FHL			.data.b	CMP,	opr_6	;ER,ER
da_1F0			.data.b	DAS,	opr_21	;RB
da_52_L			.data.b	MULXU,	opr_30	;RW,ER
da_53_L			.data.b	DIVXU,	opr_30	;RW,ER
da_5470			.data.b	RTS,	opr_85	;
da_5670			.data.b	RTE,	opr_85	;
da_57L0			.data.b	TRAPA,	opr_94	;#xx:2
da_5800			.data.b	BRA,	opr_18	;d:16
da_5810			.data.b	BRN,	opr_18	;d:16
da_5820			.data.b	BHI,	opr_18	;d:16
da_5830			.data.b	BLS,	opr_18	;d:16
da_5840			.data.b	BCC,	opr_18	;d:16
da_5850			.data.b	BCS,	opr_18	;d:16
da_5860			.data.b	BNE,	opr_18	;d:16
da_5870			.data.b	BEQ,	opr_18	;d:16
da_5880			.data.b	BVC,	opr_18	;d:16
da_5890			.data.b	BVS,	opr_18	;d:16
da_58A0			.data.b	BPL,	opr_18	;d:16
da_58B0			.data.b	BMI,	opr_18	;d:16
da_58C0			.data.b	BGE,	opr_18	;d:16
da_58D0			.data.b	BLT,	opr_18	;d:16
da_58E0			.data.b	BGT,	opr_18	;d:16
da_58F0			.data.b	BLE,	opr_18	;d:16
da_59L0			.data.b	JMP,	opr_35	;@ER
da_5C00			.data.b	BSR,	opr_18	;d:16
da_5DL0			.data.b	JSR,	opr_35	;@ER
da_6A4			.data.b	MOVFPE,	opr_84	;
da_6AC			.data.b	MOVTPE,	opr_84	;
da_6A0			.data.b	MOV,	opr_51	;@aa:16,RB
da_6A8			.data.b	MOV,	opr_58	;RB,@aa:16
da_6A2_00		.data.b	MOV,	opr_52	;@aa:24,RB
da_6AA_00		.data.b	MOV,	opr_59	;RB,@aa:24
da_6B0			.data.b	MOV,	opr_64	;@aa:16,RW
da_6B8			.data.b	MOV,	opr_70	;RW,@aa:16
da_6B2_00		.data.b	MOV,	opr_65	;@aa:24,RW
da_6BA_00		.data.b	MOV,	opr_71	;RW,@aa:24
da_6DL			.data.b	MOV,	opr_63	;@ER+,RW
da_6D7			.data.b	POP,	opr_33	;RW
da_6DH			.data.b	MOV,	opr_69	;RW,@-ER
da_6DF			.data.b	PUSH,	opr_33	;RW
da_78L06A2_00		.data.b	MOV,	opr_48	;@(d:24,ER),RB
da_78L06AA_00		.data.b	MOV,	opr_55	;RB,@(d:24,ER)
da_78L06B2_00		.data.b	MOV,	opr_62	;@(d:24,ER),RW
da_78L06BA_00		.data.b	MOV,	opr_68	;RW,@(d:24,ER)
da_790			.data.b	MOV,	opr_3	;#xx:16,RW
da_791			.data.b	ADD,	opr_3	;#xx:16,RW
da_792			.data.b	CMP,	opr_3	;#xx:16,RW
da_793			.data.b	SUB,	opr_3	;#xx:16,RW
da_794			.data.b	OR,	opr_3	;#xx:16,RW
da_795			.data.b	XOR,	opr_3	;#xx:16,RW
da_796			.data.b	AND,	opr_3	;#xx:16,RW
			.data.w	0
da_7A0L			.data.b	MOV,	opr_5	;#xx:32,ER
da_7A1L			.data.b	ADD,	opr_5	;#xx:32,ER
da_7A2L			.data.b	CMP,	opr_5	;#xx:32,ER
da_7A3L			.data.b	SUB,	opr_5	;#xx:32,ER
da_7A4L			.data.b	OR,	opr_5	;#xx:32,ER
da_7A5L			.data.b	XOR,	opr_5	;#xx:32,ER
da_7A6L			.data.b	AND,	opr_5	;#xx:32,ER
			.data.w	0
da_7B5C598F		.data.b	EEPMOV,	opr_31	;
da_7BD4598F		.data.b	EEPMOV,	opr_32	;
da_7CL063_0		.data.b	BTST,	opr_19	;RB,@ER
			.data.b	BTST,	opr_19	;RB,@ER
da_7CL073L0		.data.b	BTST,	opr_15	;#xx:3,@ER
			.data.w	0
da_7CL074L0		.data.b	BOR,	opr_15	;#xx:3,@ER
da_7CL074H0		.data.b	BIOR,	opr_15	;#xx:3,@ER
da_7CL075L0		.data.b	BXOR,	opr_15	;#xx:3,@ER
da_7CL075H0		.data.b	BIXOR,	opr_15	;#xx:3,@ER
da_7CL076L0		.data.b	BAND,	opr_15	;#xx:3,@ER
da_7CL076H0		.data.b	BIAND,	opr_15	;#xx:3,@ER
da_7CL077L0		.data.b	BLD,	opr_15	;#xx:3,@ER
da_7CL077H0		.data.b	BILD,	opr_15	;#xx:3,@ER
da_7DL060_0		.data.b	BSET,	opr_19	;RB,@ER
da_7DL061_0		.data.b	BNOT,	opr_19	;RB,@ER
da_7DL062_0		.data.b	BCLR,	opr_19	;RB,@ER
da_7DL067L0		.data.b	BST,	opr_15	;#xx:3,@ER
da_7DL067H0		.data.b	BIST,	opr_15	;#xx:3,@ER
da_7DL070L0		.data.b	BSET,	opr_15	;#xx:3,@ER
da_7DL071L0		.data.b	BNOT,	opr_15	;#xx:3,@ER
da_7DL072L0		.data.b	BCLR,	opr_15	;#xx:3,@ER
da_7E__63_0		.data.b	BTST,	opr_20	;RB,@aa:8
			.data.b	BTST,	opr_20	;RB,@aa:8
da_7E__73L0		.data.b	BTST,	opr_16	;#xx:3,@aa:8
			.data.w	0
da_7E__74L0		.data.b	BOR,	opr_16	;#xx:3,@aa:8
da_7E__74H0		.data.b	BIOR,	opr_16	;#xx:3,@aa:8
da_7E__75L0		.data.b	BXOR,	opr_16	;#xx:3,@aa:8
da_7E__75H0		.data.b	BIXOR,	opr_16	;#xx:3,@aa:8
da_7E__76L0		.data.b	BAND,	opr_16	;#xx:3,@aa:8
da_7E__76H0		.data.b	BIAND,	opr_16	;#xx:3,@aa:8
da_7E__77L0		.data.b	BLD,	opr_16	;#xx:3,@aa:8
da_7E__77H0		.data.b	BILD,	opr_16	;#xx:3,@aa:8
da_7F__60_0		.data.b	BSET,	opr_20	;RB,@aa:8
da_7F__61_0		.data.b	BNOT,	opr_20	;RB,@aa:8
da_7F__62_0		.data.b	BCLR,	opr_20	;RB,@aa:8
da_7F__67L0		.data.b	BST,	opr_16	;#xx:3,@aa:8
da_7F__67H0		.data.b	BIST,	opr_16	;#xx:3,@aa:8
da_7F__70L0		.data.b	BSET,	opr_16	;#xx:3,@aa:8
da_7F__71L0		.data.b	BNOT,	opr_16	;#xx:3,@aa:8
da_7F__72L0		.data.b	BCLR,	opr_16	;#xx:3,@aa:8
da_01C050		.data.b	MULXS,	opr_27	;RB,RW
da_01C052_L		.data.b	MULXS,	opr_28	;RW,ER
da_01D051		.data.b	DIVXS,	opr_27	;RB,RW
da_01D053_L		.data.b	DIVXS,	opr_28	;RW,ER
da_01F064LL		.data.b	OR,	opr_12	;ER,ER
da_01F065LL		.data.b	XOR,	opr_12	;ER,ER
da_01F066LL		.data.b	AND,	opr_12	;ER,ER
da_01006D7L		.data.b	POP,	opr_86	;ER
da_01006DFL		.data.b	PUSH,	opr_86	;ER
da_010069LL		.data.b	MOV,	opr_72	;@ER,ER
da_010069HL		.data.b	MOV,	opr_78	;ER,@ER
			.data.w	0
			.data.w	0
da_01006DLL		.data.b	MOV,	opr_75	;@ER+,ER
da_01006DHL		.data.b	MOV,	opr_81	;ER,@-ER
da_01006FLL		.data.b	MOV,	opr_73	;@(d:16,ER),ER
da_01006FHL		.data.b	MOV,	opr_79	;ER,@(d:16,ER)
da_014069L0		.data.b	LDC,	opr_40	;@ER,CCR
da_014069H0		.data.b	STC,	opr_88	;CCR,@ER
			.data.w	0
			.data.w	0
da_01406DL0		.data.b	LDC,	opr_43	;@ER+,CCR
da_01406DH0		.data.b	STC,	opr_91	;CCR,@-ER
da_01406FL0		.data.b	LDC,	opr_41	;@(d:16,ER),CCR
da_01406FH0		.data.b	STC,	opr_89	;CCR,@(d:16,ER)
da_010078L06B2L00	.data.b	MOV,	opr_74	;@(d:24,ER),ER
da_010078H06BAL00	.data.b	MOV,	opr_80	;ER,@(d:24,ER)
da_014078L06B2000	.data.b	LDC,	opr_42	;@(d:24,ER),CCR
da_014078L06BA000	.data.b	STC,	opr_90	;CCR,@(d:24,ER)
da_01006B0L		.data.b	MOV,	opr_76	;@aa:16,ER
da_01006B2L00		.data.b	MOV,	opr_77	;@aa:24,ER
da_01006B8L		.data.b	MOV,	opr_82	;ER,@aa:16
da_01006BAL00		.data.b	MOV,	opr_83	;ER,@aa:24
da_01406B00		.data.b	LDC,	opr_44	;@aa:16,CCR
da_01406B2000		.data.b	LDC,	opr_45	;@aa:24,CCR
da_01406B80		.data.b	STC,	opr_92	;CCR,@aa:16
da_01406BA000		.data.b	STC,	opr_93	;CCR,@aa:24
	.align	2
	;* 0x *
da_0x_f:
	mov.l	#da_0x,er2
da_0x_f_0
	sub.l	er0,er0
	mov.b	@er1,r0l
	and.b	#H'f,r0l
	shll.l	er0
	bra	dis_asm_sub_0a
	;* 1x *
da_1x_f:
	mov.l	#da_1x,er2
	bra	da_0x_f_0
	;* 4x *
da_4x_f:
	mov.l	#da_40,er2
	bra	da_0x_f_0
	;* 5x *
da_5x_f:
	mov.l	#da_50,er2
	bra	da_0x_f_0
	;* 6x *
da_6x_f:
	mov.l	#da_60,er2
	mov.w	@er1,r0
	cmp.b	#H'66,r0h
	bls	da_0x_f_0
	bld	#7,r0l
	bcc	da_0x_f_0
	add.l	#da_67H-da_67L,er2
	bra	da_0x_f_0
	;* 7x *
da_7x_f:
	bld	#3,@er1
	bcc	da_7x_f_0:8
	;* 78..7F *
	mov.l	#da_74L,er2
	bra	da_0x_f_0
	;* 70..77 *
da_7x_f_0
	mov.l	#da_70L,er2
	mov.w	@er1,r0
	and.w	#H'0780,r0
	extu.l	er0
	shlr.l	er0
	shlr.l	er0
	shlr.l	er0
	shlr.l	er0
	shlr.l	er0
	shlr.l	er0
	bra	dis_asm_sub_0a
	;* 00x *
da_00x_f
	mov.l	#da_0000,er0
	mov.w	@er1,r2
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 01x *
da_01x_f
	mov.l	#da_0180,er0
	mov.w	@er1,r2
	cmp.b	#H'80,r2l
	beq	dis_asm_sub_0b
	mov.w	@(2,er1),e2
	cmp.b	#H'00,r2l
	beq	da_0100x_f
	cmp.b	#H'40,r2l
	beq	da_0140x_f
	cmp.b	#H'C0,r2l
	beq	da_01C0x_f
	cmp.b	#H'D0,r2l
	beq	da_01D0x_f
	cmp.b	#H'F0,r2l
	beq	da_01F0x_f
	andc	#B'11111110,ccr
	rts	;(error)
	;* 02x *
da_02x_f
	mov.l	#da_020,er0
da_02x_f_0
	mov.w	@er1,r2
	and.b	#H'f0,r2l
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 03x *
da_03x_f
	mov.l	#da_030,er0
	bra	da_02x_f_0:8
	;* 0Ax *
da_0Ax_f
	mov.l	#da_0AHL,er0
da_0Ax_f_0
	mov.w	@er1,r2
	bld	#7,r2l
	biand	#3,r2l
	bcs	dis_asm_sub_0b
	adds	#2,er0
	and.b	#H'f0,r2l
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 0Bx *
da_0Bx_f
	mov.l	#da_0B0L,er2
da_0Bx_f_0
	mov.w	@er1,r0		;0L	0000 0
	mov.b	r0l,r0h		;5	0101
	and.b	#H'70,r0h	;7L	0111 0
	cmp.b	#H'50,r0h	;8L	1000 0
	beq	da_0Bx_f_1:8	;9L	1001 0
	bild	#3,r0l		;D	1101
	bcs	da_0Bx_f_1:8	;FL	1111 0
	rts	;(error)
da_0Bx_f_1
	shlr.b	r0l
	shlr.b	r0l
	shlr.b	r0l
	bclr	#0,r0l
	extu.w	r0
	extu.l	er0
	bra	dis_asm_sub_0a
	;* 0Fx *
da_0Fx_f
	mov.l	#da_0FHL,er0
	bra	da_0Ax_f_0
	;* 10x *
da_10x_f
	mov.l	#da_100,er2
da_10x_f_0
	mov.w	#4,e0
	mov.w	@er1,r0
	mov.b	r0l,r0h
	and.b	#B'01111000,r0l
	cmp.b	#B'00110000,r0l
	beq	da_10x_f_1:8
	dec.w	#2,e0
	and.b	#B'01110000,r0l
	cmp.b	#B'00010000,r0l
	beq	da_10x_f_1:8
	dec.w	#2,e0
	cmp.b	#B'00000000,r0l
	beq	da_10x_f_1:8
	andc	#B'11111110,ccr
	rts	;(error)
da_10x_f_1
	bld	#7,r0h
	mov.w	e0,r0
	bcc	da_10x_f_2:8
	add.b	#2*3,r0l
da_10x_f_2
	extu.l	er0
	bra	dis_asm_sub_0a
	;* 11x *
da_11x_f
	mov.l	#da_110,er2
	bra	da_10x_f_0:8
	;* 12x *
da_12x_f
	mov.l	#da_120,er2
	bra	da_10x_f_0:8
	;* 13x *
da_13x_f
	mov.l	#da_130,er2
	bra	da_10x_f_0:8
	;* 17x *
da_17x_f
	mov.l	#da_170,er2
	mov.w	@er1,r0
	mov.b	r0l,r0h
	and.b	#B'00110000,r0h
	cmp.b	#B'00110000,r0h
	bne	da_0Bx_f_1
	bild	#3,r0l
	bcs	da_0Bx_f_1
	rts	;(error)
	;* 1Ax *
da_1Ax_f
	mov.l	#da_1AHL,er0
	bra	da_0Ax_f_0
	;* 1Bx *
da_1Bx_f
	mov.l	#da_1B0L,er2
	bra	da_0Bx_f_0
	;* 1Fx *
da_1Fx_f
	mov.l	#da_1FHL,er0
	bra	da_0Ax_f_0
	;* 52x *
da_52x_f
	mov.l	#da_52_L,er0
da_52x_f_0
	mov.w	@er1,r2
	bild	#3,r2l
	bcs	dis_asm_sub_0b
	rts	;(error)
	;* 53x *
da_53x_f
	mov.l	#da_53_L,er0
	bra	da_52x_f_0:8
	;* 54x *
da_54x_f
	mov.l	#da_5470,er0
da_54x_f_0
	mov.w	@er1,r2
	cmp.b	#H'70,r2l
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 56x *
da_56x_f
	mov.l	#da_5670,er0
	bra	da_54x_f_0
	;* 57x *
da_57x_f
	mov.l	#da_57L0,er0
da_57x_f_0
	mov.w	@er1,r2
	and.b	#B'10001111,r2l
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 58x *
da_58x_f
	mov.l	#da_5800,er2
	mov.w	@er1,r0
	mov.b	r0l,r0h
	and.b	#H'0f,r0h
	beq	da_0Bx_f_1
	andc	#B'11111110,ccr
	rts	;(error)
	;* 59x *
da_59x_f
	mov.l	#da_59L0,er0
	bra	da_57x_f_0
	;* 5Cx *
da_5Cx_f
	mov.l	#da_5C00,er0
	mov.w	@er1,r2
	cmp.b	#0,r2l
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts	;(error)
	;* 5Dx *
da_5Dx_f
	mov.l	#da_5DL0,er0
	bra	da_57x_f_0
	;* 6Ax *
da_6Ax_f
	mov.l	#da_6A4,er0
	mov.w	@er1,r2
	and.b	#H'f0,r2l
	cmp.b	#H'40,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.b	#H'c0,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
da_6Ax_f_0
	cmp.b	#H'00,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.b	#H'80,r2l
	beq	dis_asm_sub_0b
	mov.b	@(2,er1),r2h
	bne	da_6Ax_f_1:8
	adds	#2,er0
	cmp.b	#H'20,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.b	#H'A0,r2l
	beq	dis_asm_sub_0b
da_6Ax_f_1
	andc	#B'11111110,ccr
	rts	;(error)
	;* 6Bx *
da_6Bx_f
	mov.l	#da_6B0,er0
	mov.w	@er1,r2
	and.b	#H'f0,r2l
	bra	da_6Ax_f_0:8
	;* 6Dx *
da_6Dx_f
	mov.l	#da_6DL,er2
	mov.w	@er1,r0
	bld	#4,r0l
	band	#5,r0l
	band		#6,r0l
	bst	#1,r0l
	bld	#7,r0l
	bst	#2,r0l
da_6Dx_f_0
	and.b	#B'00000110,r0l
	extu.w	r0
	extu.l	er0
	bra	dis_asm_sub_0a
	;* 78x *
da_78x_f
	mov.l	@(2,er1),er0
	mov.b	@(1,er1),r0l
	and.l	#H'fa70ff80,er0
	cmp.l	#H'6a200000,er0
	beq	da_78x_f_0:8
	andc	#B'11111110,ccr
	rts	;(error)
da_78x_f_0
	mov.l	#da_78L06A2_00,er2
	mov.w	@(2,er1),r0
	bld	#7,r0l
	bst	#1,r0l
	bld	#0,r0h
	bst	#2,r0l
	bra	da_6Dx_f_0:8
	;* 79x *
da_79x_f
	mov.l	#da_790,er2
	mov.w	@er1,r0
da_79x_f_0
	cmp.b	#H'70,r0l
	blo	da_79x_f_1:8
	rts	;(error)
da_79x_f_1
	shlr.b	r0l
	shlr.b	r0l
	shlr.b	r0l
	bclr	#0,r0l
	extu.w	r0
	extu.l	er0
	bra	dis_asm_sub_0a
	;* 7Ax *
da_7Ax_f
	mov.l	#da_7A0L,er2
	mov.w	@er1,r0
	bild	#3,r0l
	bcs	da_79x_f_0:8
	rts	;(error)
	;* 7Bx *
da_7Bx_f
	mov.l	#da_7B5C598F,er0
	mov.l	@er1,er2
	cmp.l	#H'7b5c598f,er2
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.l	#H'7bd4598f,er2
	beq	dis_asm_sub_0b
da_7Bx_f_0
	andc	#B'11111110,ccr
	rts	;(error)
	;* 7Cx *
da_7Cx_f
	mov.w	@er1,r0
	bld	#7,r0l
	bcs	da_7Bx_f_0:8
	mov.l	#da_7CL063_0,er2
da_7Cx_f_0
	mov.w	@(2,er1),r0
	and.b	#B'10001111,r0l
	shll.b	r0l
	bne	da_7Bx_f_0:8
	bst	#1,r0l
	cmp.b	#H'63,r0h
	beq	da_7Cx_f_1:8
	cmp.b	#H'73,r0h
	blo	da_7Bx_f_0:8
	cmp.b	#H'77,r0h
	bhi	da_7Bx_f_0:8
	add.b	#-(H'73-H'63-1),r0h
da_7Cx_f_1
	add.b	#-H'63,r0h
	shll.b	r0h
	shll.b	r0h
	add.b	r0h,r0l
	extu.w	r0
	extu.l	er0
	bra	dis_asm_sub_0a
	;* 7Dx *
da_7Dx_f
	mov.l	#da_7DL060_0,er2
	mov.w	@er1,r0
	and.b	#H'8f,r0l
	bne	da_7Bx_f_0
da_7Dx_f_0
	mov.w	@(2,er1),r0
	mov.w	r0,e0
	and.b	#H'f,r0l
	bne	da_7Bx_f_0
	cmp.b	#H'60,r0h
	blo	da_7Bx_f_0
	cmp.b	#H'62,r0h
	bls	da_7Dx_f_3:8
	cmp.b	#H'67,r0h
	blo	da_7Bx_f_0
	mov.w	e0,r0
	cmp.b	#H'68,r0h
	blo	da_7Dx_f_1:8
	cmp.b	#H'70,r0h
	blo	da_7Bx_f_0
	bld	#7,r0l
	bcs	da_7Bx_f_0
	cmp.b	#H'72,r0h
	bls	da_7Dx_f_2:8
	rts	;(error)
da_7Dx_f_1
	bld	#7,r0l
	addx	#H'70-H'67-2,r0h
da_7Dx_f_2
	add.b	#-(H'70-2-H'63),r0h
da_7Dx_f_3
	mov.b	r0h,r0l
	add.b	#-H'60,r0l
	extu.w	r0
	extu.l	er0
	shll.l	er0
	bra	dis_asm_sub_0a
	;* 7Ex *
da_7Ex_f
	mov.l	#da_7E__63_0,er2
	bra	da_7Cx_f_0
	;* 7Fx *
da_7Fx_f
	mov.l	#da_7F__60_0,er2
	bra	da_7Dx_f_0:8
	;* 0100x *
da_0100x_f
	mov.w	@(2,er1),r2
	cmp.b	#H'78,r2h
	beq	da_010078x_f
	bld	#3,r2l
	bcs	da_0100x_f_1:8
	cmp.b	#H'6b,r2h
	beq	da_01006BxL_f
	mov.l	#da_01006D7L,er0
	and.b	#H'f8,r2l
	cmp.w	#H'6d70,r2
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.w	#H'6df0,r2
	beq	dis_asm_sub_0b
	adds	#2,er0
	mov.l	er0,er2
da_0100x_f_0
	mov.w	@(2,er1),r0
	bld	#7,r0l
	addx	#0,r0h
	mov.b	r0h,r0l
	add.b	#-H'69,r0l
	extu.w	r0
	extu.l	er0
	shll.l	er0
	bra	dis_asm_sub_0a
da_0100x_f_1
	andc	#B'11111110,ccr
	rts	;(error)
	;* 0140x *
da_0140x_f
	mov.w	@(2,er1),r0
	and.b	#H'0f,r0l
	bne	da_0100x_f_1:8
	cmp.b	#H'78,r0h
	beq	da_014078_0x_f
	cmp.b	#H'6b,r0h
	beq	da_01406B_0x_f
	mov.l	#da_014069L0,er2
	bra	da_0100x_f_0:8
	;* 01C0x *
da_01C0x_f
	mov.l	#da_01C050,er0
	mov.w	@(2,er1),r2
	cmp.b	#H'50,r2h
	beq	dis_asm_sub_0b
	adds	#2,er0
	and.b	#B'00001000,r2l
	cmp.w	#H'5200,r2
	beq	dis_asm_sub_0b
	andc	#B'11111110,ccr
	rts
	;* 01D0x *
da_01D0x_f
	mov.l	#da_01D051,er0
	mov.w	@(2,er1),r2
	cmp.b	#H'51,r2h
	beq	dis_asm_sub_0b
	adds	#2,er0
	and.b	#B'00001000,r2l
	cmp.w	#H'5300,r2
	beq	dis_asm_sub_0b
da_01D0x_f_0
	andc	#B'11111110,ccr
	rts
	;* 01F0x *
da_01F0x_f
	mov.w	@(2,er1),r0
	bld	#3,r0l
	bor	#7,r0l
	bcs	da_01D0x_f_0:8
	cmp.b	#H'64,r0h
	blo	da_01D0x_f_0:8
	cmp.b	#H'66,r0h
	bhi	da_01D0x_f_0:8
	mov.b	r0h,r0l
	add.b	#-H'64,r0l
	extu.w	r0
	extu.l	er0
	shll.l	er0
	mov.l	#da_01F064LL,er2
	bra	dis_asm_sub_0a
	;* 010078x *
da_010078x_f
	mov.l	@(4,er1),er0
	and.l	#H'ff78ff00,er0
	cmp.l	#H'6B200000,er0
	bne	da_010078x_f_0:8
	mov.b	@(3,er1),r2l
	mov.b	r2l,r2h
	and.b	#H'f,r2h
	bne	da_010078x_f_0:8
	mov.l	#da_010078L06B2L00,er0
	bld	#7,r2l
	bcc	dis_asm_sub_0b
	adds	#2,er0
	bra	dis_asm_sub_0b
da_010078x_f_0
	andc	#B'11111110,ccr
	rts	;(error)
	;* 01006BxL *
da_01006BxL_f
	mov.l	#da_01006B0L,er0
da_01006BxL_f_0
	mov.b	@(3,er1),r2l
	and.b	#H'f8,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.b	#H'20,r2l
	beq	da_01006BxL_f_1:8
	adds	#2,er0
	cmp.b	#H'80,r2l
	beq	dis_asm_sub_0b
	adds	#2,er0
	cmp.b	#H'a0,r2l
	bne	da_010078x_f_0:8
da_01006BxL_f_1
	mov.b	@(4,er1),r2l
	beq	dis_asm_sub_0b
	rts	;(error)
	;* 01406B_0x *
da_01406B_0x_f
	mov.l	#da_01406B00,er0
	mov.b	@(3,er1),r2l
	and.b	#H'0f,r2l
	beq	da_01006BxL_f_0:8
	andc	#B'11111110,ccr
	rts	;(error)
	;* 014078_0x *
da_014078_0x_f
	mov.b	@(3,er1),r0l
	bld	#7,r0l
	bcs	da_010078x_f_0:8
	mov.l	@(4,er1),er0
	mov.w	e0,r2
	and.l	#H'ff7fff00,er0
	cmp.l	#H'6B200000,er0
	bne	da_010078x_f_0:8
	mov.l	#da_014078L06B2000,er0
	bld	#7,r2l
	bcc	dis_asm_sub_0b
	adds	#2,er0
	bra	dis_asm_sub_0b

;	H'00	(SP)
;	H'01	#1
;	H'02	#2
;	H'03	CCR
;	H'04	#4
;	H'05	@@aa:8
;	H'06	d:8
;	H'07	d:16
;	H'08	#xx:2
;	H'10	RnH/RnL
;	H'20	Rn
;	H'30	ERn
;	H'40	#xx:3
;	H'50	#xx:8
;	H'60	#xx:16
;	H'70	#xx:32
;	H'80	@ER
;	H'90	@(d:16,ER)
;	H'A0	@(d:24,ER)
;	H'B0	@ER+
;	H'C0	@-ER
;	H'D0	@aa:8
;	H'E0	@aa:16
;	H'F0	@aa:24

opr_tbl
	.data.b	0,0,0
opr_1	.equ	($-opr_tbl)/3
	.data.b	H'12,H'52,H'11
opr_2	.equ	($-opr_tbl)/3
	.data.b	H'12,H'12,H'13
opr_3	.equ	($-opr_tbl)/3
	.data.b	H'24,H'64,H'23
opr_4	.equ	($-opr_tbl)/3
	.data.b	H'22,H'22,H'23
opr_5	.equ	($-opr_tbl)/3
	.data.b	H'36,H'74,H'33
opr_6	.equ	($-opr_tbl)/3
	.data.b	H'32,H'32,H'33
opr_7	.equ	($-opr_tbl)/3
	.data.b	H'02,H'01,H'33
opr_8	.equ	($-opr_tbl)/3
	.data.b	H'02,H'02,H'33
opr_9	.equ	($-opr_tbl)/3
	.data.b	H'02,H'04,H'33
opr_10	.equ	($-opr_tbl)/3
	.data.b	H'02,H'52,H'11
opr_11	.equ	($-opr_tbl)/3
	.data.b	H'02,H'12,H'13
opr_12	.equ	($-opr_tbl)/3
	.data.b	H'34,H'36,H'37
opr_13	.equ	($-opr_tbl)/3
	.data.b	H'02,H'52,H'03
opr_14	.equ	($-opr_tbl)/3
	.data.b	H'02,H'42,H'13
opr_15	.equ	($-opr_tbl)/3
	.data.b	H'04,H'46,H'82
opr_16	.equ	($-opr_tbl)/3
	.data.b	H'04,H'46,H'D2
opr_17	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'06
opr_18	.equ	($-opr_tbl)/3
	.data.b	H'04,H'00,H'07
opr_19	.equ	($-opr_tbl)/3
	.data.b	H'04,H'16,H'82
opr_20	.equ	($-opr_tbl)/3
	.data.b	H'04,H'16,H'D2
opr_21	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'13
opr_22	.equ	($-opr_tbl)/3
	.data.b	H'12,H'00,H'13
opr_23	.equ	($-opr_tbl)/3
	.data.b	H'22,H'01,H'23
opr_24	.equ	($-opr_tbl)/3
	.data.b	H'22,H'02,H'23
opr_25	.equ	($-opr_tbl)/3
	.data.b	H'32,H'01,H'33
opr_26	.equ	($-opr_tbl)/3
	.data.b	H'32,H'02,H'33
opr_27	.equ	($-opr_tbl)/3
	.data.b	H'14,H'16,H'27
opr_28	.equ	($-opr_tbl)/3
	.data.b	H'24,H'26,H'37
opr_29	.equ	($-opr_tbl)/3
	.data.b	H'12,H'12,H'23
opr_30	.equ	($-opr_tbl)/3
	.data.b	H'22,H'22,H'33
opr_31	.equ	($-opr_tbl)/3
	.data.b	H'14,H'00,H'00
opr_32	.equ	($-opr_tbl)/3
	.data.b	H'24,H'00,H'00
opr_33	.equ	($-opr_tbl)/3
	.data.b	H'22,H'00,H'23
opr_34	.equ	($-opr_tbl)/3
	.data.b	H'32,H'00,H'33
opr_35	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'82
opr_36	.equ	($-opr_tbl)/3
	.data.b	H'04,H'00,H'F2
opr_37	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'05
opr_38	.equ	($-opr_tbl)/3
	.data.b	H'12,H'52,H'03
opr_39	.equ	($-opr_tbl)/3
	.data.b	H'12,H'13,H'03
opr_40	.equ	($-opr_tbl)/3
	.data.b	H'24,H'86,H'03
opr_41	.equ	($-opr_tbl)/3
	.data.b	H'26,H'96,H'03
opr_42	.equ	($-opr_tbl)/3
	.data.b	H'2A,H'A6,H'03
opr_43	.equ	($-opr_tbl)/3
	.data.b	H'24,H'B6,H'03
opr_44	.equ	($-opr_tbl)/3
	.data.b	H'26,H'E8,H'03
opr_45	.equ	($-opr_tbl)/3
	.data.b	H'28,H'FA,H'03
opr_46	.equ	($-opr_tbl)/3
	.data.b	H'12,H'82,H'13
opr_47	.equ	($-opr_tbl)/3
	.data.b	H'14,H'92,H'13
opr_48	.equ	($-opr_tbl)/3
	.data.b	H'18,H'A2,H'17
opr_49	.equ	($-opr_tbl)/3
	.data.b	H'12,H'B2,H'13
opr_50	.equ	($-opr_tbl)/3
	.data.b	H'12,H'D2,H'11
opr_51	.equ	($-opr_tbl)/3
	.data.b	H'14,H'E4,H'13
opr_52	.equ	($-opr_tbl)/3
	.data.b	H'16,H'F6,H'13
opr_53	.equ	($-opr_tbl)/3
	.data.b	H'12,H'13,H'82
opr_54	.equ	($-opr_tbl)/3
	.data.b	H'14,H'13,H'92
opr_55	.equ	($-opr_tbl)/3
	.data.b	H'18,H'17,H'A2
opr_56	.equ	($-opr_tbl)/3
	.data.b	H'12,H'13,H'C2
opr_57	.equ	($-opr_tbl)/3
	.data.b	H'12,H'11,H'D2
opr_58	.equ	($-opr_tbl)/3
	.data.b	H'14,H'13,H'E4
opr_59	.equ	($-opr_tbl)/3
	.data.b	H'16,H'13,H'F6
opr_60	.equ	($-opr_tbl)/3
	.data.b	H'22,H'82,H'23
opr_61	.equ	($-opr_tbl)/3
	.data.b	H'24,H'92,H'23
opr_62	.equ	($-opr_tbl)/3
	.data.b	H'28,H'A2,H'27
opr_63	.equ	($-opr_tbl)/3
	.data.b	H'22,H'B2,H'23
opr_64	.equ	($-opr_tbl)/3
	.data.b	H'24,H'E4,H'23
opr_65	.equ	($-opr_tbl)/3
	.data.b	H'26,H'F6,H'23
opr_66	.equ	($-opr_tbl)/3
	.data.b	H'22,H'23,H'82
opr_67	.equ	($-opr_tbl)/3
	.data.b	H'24,H'23,H'92
opr_68	.equ	($-opr_tbl)/3
	.data.b	H'28,H'27,H'A2
opr_69	.equ	($-opr_tbl)/3
	.data.b	H'22,H'23,H'C2
opr_70	.equ	($-opr_tbl)/3
	.data.b	H'24,H'23,H'E4
opr_71	.equ	($-opr_tbl)/3
	.data.b	H'26,H'23,H'F6
opr_72	.equ	($-opr_tbl)/3
	.data.b	H'34,H'86,H'37
opr_73	.equ	($-opr_tbl)/3
	.data.b	H'36,H'96,H'37
opr_74	.equ	($-opr_tbl)/3
	.data.b	H'3A,H'A6,H'3B
opr_75	.equ	($-opr_tbl)/3
	.data.b	H'34,H'B6,H'37
opr_76	.equ	($-opr_tbl)/3
	.data.b	H'36,H'E8,H'37
opr_77	.equ	($-opr_tbl)/3
	.data.b	H'38,H'FA,H'37
opr_78	.equ	($-opr_tbl)/3
	.data.b	H'34,H'37,H'86
opr_79	.equ	($-opr_tbl)/3
	.data.b	H'36,H'37,H'96
opr_80	.equ	($-opr_tbl)/3
	.data.b	H'3A,H'3B,H'A6
opr_81	.equ	($-opr_tbl)/3
	.data.b	H'34,H'37,H'C6
opr_82	.equ	($-opr_tbl)/3
	.data.b	H'36,H'37,H'E8
opr_83	.equ	($-opr_tbl)/3
	.data.b	H'38,H'37,H'FA
opr_84	.equ	($-opr_tbl)/3
	.data.b	H'04,H'00,H'00
opr_85	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'00
opr_86	.equ	($-opr_tbl)/3
	.data.b	H'34,H'00,H'37
opr_87	.equ	($-opr_tbl)/3
	.data.b	H'12,H'03,H'13
opr_88	.equ	($-opr_tbl)/3
	.data.b	H'24,H'03,H'86
opr_89	.equ	($-opr_tbl)/3
	.data.b	H'26,H'03,H'96
opr_90	.equ	($-opr_tbl)/3
	.data.b	H'2A,H'03,H'A6
opr_91	.equ	($-opr_tbl)/3
	.data.b	H'24,H'03,H'C6
opr_92	.equ	($-opr_tbl)/3
	.data.b	H'26,H'03,H'E8
opr_93	.equ	($-opr_tbl)/3
	.data.b	H'28,H'03,H'FA
opr_94	.equ	($-opr_tbl)/3
	.data.b	H'02,H'00,H'08
	.align	2

opc_tbl
	.data.w	0
ADD	.equ	($-opc_tbl)/2
	.data.w	ADDs-opc_tbl
ADDS	.equ	($-opc_tbl)/2
	.data.w	ADDSs-opc_tbl
ADDX	.equ	($-opc_tbl)/2
	.data.w	ADDXs-opc_tbl
AND	.equ	($-opc_tbl)/2
	.data.w	ANDs-opc_tbl
ANDC	.equ	($-opc_tbl)/2
	.data.w	ANDCs-opc_tbl
BAND	.equ	($-opc_tbl)/2
	.data.w	BANDs-opc_tbl
BRA	.equ	($-opc_tbl)/2
	.data.w	BRAs-opc_tbl
BRN	.equ	($-opc_tbl)/2
	.data.w	BRNs-opc_tbl
BHI	.equ	($-opc_tbl)/2
	.data.w	BHIs-opc_tbl
BLS	.equ	($-opc_tbl)/2
	.data.w	BLSs-opc_tbl
BCC	.equ	($-opc_tbl)/2
	.data.w	BCCs-opc_tbl
BCS	.equ	($-opc_tbl)/2
	.data.w	BCSs-opc_tbl
BNE	.equ	($-opc_tbl)/2
	.data.w	BNEs-opc_tbl
BEQ	.equ	($-opc_tbl)/2
	.data.w	BEQs-opc_tbl
BVC	.equ	($-opc_tbl)/2
	.data.w	BVCs-opc_tbl
BVS	.equ	($-opc_tbl)/2
	.data.w	BVSs-opc_tbl
BPL	.equ	($-opc_tbl)/2
	.data.w	BPLs-opc_tbl
BMI	.equ	($-opc_tbl)/2
	.data.w	BMIs-opc_tbl
BGE	.equ	($-opc_tbl)/2
	.data.w	BGEs-opc_tbl
BLT	.equ	($-opc_tbl)/2
	.data.w	BLTs-opc_tbl
BGT	.equ	($-opc_tbl)/2
	.data.w	BGTs-opc_tbl
BLE	.equ	($-opc_tbl)/2
	.data.w	BLEs-opc_tbl
BCLR	.equ	($-opc_tbl)/2
	.data.w	BCLRs-opc_tbl
BIAND	.equ	($-opc_tbl)/2
	.data.w	BIANDs-opc_tbl
BILD	.equ	($-opc_tbl)/2
	.data.w	BILDs-opc_tbl
BIOR	.equ	($-opc_tbl)/2
	.data.w	BIORs-opc_tbl
BIST	.equ	($-opc_tbl)/2
	.data.w	BISTs-opc_tbl
BIXOR	.equ	($-opc_tbl)/2
	.data.w	BIXORs-opc_tbl
BLD	.equ	($-opc_tbl)/2
	.data.w	BLDs-opc_tbl
BNOT	.equ	($-opc_tbl)/2
	.data.w	BNOTs-opc_tbl
BOR	.equ	($-opc_tbl)/2
	.data.w	BORs-opc_tbl
BSET	.equ	($-opc_tbl)/2
	.data.w	BSETs-opc_tbl
BSR	.equ	($-opc_tbl)/2
	.data.w	BSRs-opc_tbl
BST	.equ	($-opc_tbl)/2
	.data.w	BSTs-opc_tbl
BTST	.equ	($-opc_tbl)/2
	.data.w	BTSTs-opc_tbl
BXOR	.equ	($-opc_tbl)/2
	.data.w	BXORs-opc_tbl
CMP	.equ	($-opc_tbl)/2
	.data.w	CMPs-opc_tbl
DAA	.equ	($-opc_tbl)/2
	.data.w	DAAs-opc_tbl
DAS	.equ	($-opc_tbl)/2
	.data.w	DASs-opc_tbl
DEC	.equ	($-opc_tbl)/2
	.data.w	DECs-opc_tbl
DIVXS	.equ	($-opc_tbl)/2
	.data.w	DIVXSs-opc_tbl
DIVXU	.equ	($-opc_tbl)/2
	.data.w	DIVXUs-opc_tbl
EEPMOV	.equ	($-opc_tbl)/2
	.data.w	EEPMOVs-opc_tbl
EXTS	.equ	($-opc_tbl)/2
	.data.w	EXTSs-opc_tbl
EXTU	.equ	($-opc_tbl)/2
	.data.w	EXTUs-opc_tbl
INC	.equ	($-opc_tbl)/2
	.data.w	INCs-opc_tbl
JMP	.equ	($-opc_tbl)/2
	.data.w	JMPs-opc_tbl
JSR	.equ	($-opc_tbl)/2
	.data.w	JSRs-opc_tbl
LDC	.equ	($-opc_tbl)/2
	.data.w	LDCs-opc_tbl
MOV	.equ	($-opc_tbl)/2
	.data.w	MOVs-opc_tbl
MOVFPE	.equ	($-opc_tbl)/2
	.data.w	MOVFPEs-opc_tbl
MOVTPE	.equ	($-opc_tbl)/2
	.data.w	MOVTPEs-opc_tbl
MULXS	.equ	($-opc_tbl)/2
	.data.w	MULXSs-opc_tbl
MULXU	.equ	($-opc_tbl)/2
	.data.w	MULXUs-opc_tbl
NEG	.equ	($-opc_tbl)/2
	.data.w	NEGs-opc_tbl
NOP	.equ	($-opc_tbl)/2
	.data.w	NOPs-opc_tbl
NOT	.equ	($-opc_tbl)/2
	.data.w	NOTs-opc_tbl
OR	.equ	($-opc_tbl)/2
	.data.w	ORs-opc_tbl
ORC	.equ	($-opc_tbl)/2
	.data.w	ORCs-opc_tbl
POP	.equ	($-opc_tbl)/2
	.data.w	POPs-opc_tbl
PUSH	.equ	($-opc_tbl)/2
	.data.w	PUSHs-opc_tbl
ROTL	.equ	($-opc_tbl)/2
	.data.w	ROTLs-opc_tbl
ROTR	.equ	($-opc_tbl)/2
	.data.w	ROTRs-opc_tbl
ROTXL	.equ	($-opc_tbl)/2
	.data.w	ROTXLs-opc_tbl
ROTXR	.equ	($-opc_tbl)/2
	.data.w	ROTXRs-opc_tbl
RTE	.equ	($-opc_tbl)/2
	.data.w	RTEs-opc_tbl
RTS	.equ	($-opc_tbl)/2
	.data.w	RTSs-opc_tbl
SHAL	.equ	($-opc_tbl)/2
	.data.w	SHALs-opc_tbl
SHAR	.equ	($-opc_tbl)/2
	.data.w	SHARs-opc_tbl
SHLL	.equ	($-opc_tbl)/2
	.data.w	SHLLs-opc_tbl
SHLR	.equ	($-opc_tbl)/2
	.data.w	SHLRs-opc_tbl
SLEEP	.equ	($-opc_tbl)/2
	.data.w	SLEEPs-opc_tbl
STC	.equ	($-opc_tbl)/2
	.data.w	STCs-opc_tbl
SUB	.equ	($-opc_tbl)/2
	.data.w	SUBs-opc_tbl
SUBS	.equ	($-opc_tbl)/2
	.data.w	SUBSs-opc_tbl
SUBX	.equ	($-opc_tbl)/2
	.data.w	SUBXs-opc_tbl
TRAPA	.equ	($-opc_tbl)/2
	.data.w	TRAPAs-opc_tbl
XOR	.equ	($-opc_tbl)/2
	.data.w	XORs-opc_tbl
XORC	.equ	($-opc_tbl)/2
	.data.w	XORCs-opc_tbl

ADDs	.sdataz	"ADD"
ADDSs	.sdataz	"ADDS"
ADDXs	.sdataz	"ADDX"
ANDs	.sdataz	"AND"
ANDCs	.sdataz	"ANDC"
BANDs	.sdataz	"BAND"
BRAs	.sdataz	"BRA"
BRNs	.sdataz	"BRN"
BHIs	.sdataz	"BHI"
BLSs	.sdataz	"BLS"
BCCs	.sdataz	"BCC"
BCSs	.sdataz	"BCS"
BNEs	.sdataz	"BNE"
BEQs	.sdataz	"BEQ"
BVCs	.sdataz	"BVC"
BVSs	.sdataz	"BVS"
BPLs	.sdataz	"BPL"
BMIs	.sdataz	"BMI"
BGEs	.sdataz	"BGE"
BLTs	.sdataz	"BLT"
BGTs	.sdataz	"BGT"
BLEs	.sdataz	"BLE"
BCLRs	.sdataz	"BCLR"
BIANDs	.sdataz	"BIAND"
BILDs	.sdataz	"BILD"
BIORs	.sdataz	"BIOR"
BISTs	.sdataz	"BIST"
BIXORs	.sdataz	"BIXOR"
BLDs	.sdataz	"BLD"
BNOTs	.sdataz	"BNOT"
BORs	.sdataz	"BOR"
BSETs	.sdataz	"BSET"
BSRs	.sdataz	"BSR"
BSTs	.sdataz	"BST"
BTSTs	.sdataz	"BTST"
BXORs	.sdataz	"BXOR"
CMPs	.sdataz	"CMP"
DAAs	.sdataz	"DAA"
DASs	.sdataz	"DAS"
DECs	.sdataz	"DEC"
DIVXSs	.sdataz	"DIVXS"
DIVXUs	.sdataz	"DIVXU"
EEPMOVs	.sdataz	"EEPMOV"
EXTSs	.sdataz	"EXTS"
EXTUs	.sdataz	"EXTU"
INCs	.sdataz	"INC"
JMPs	.sdataz	"JMP"
JSRs	.sdataz	"JSR"
LDCs	.sdataz	"LDC"
MOVs	.sdataz	"MOV"
MOVFPEs	.sdataz	"MOVFPE"
MOVTPEs	.sdataz	"MOVTPE"
MULXSs	.sdataz	"MULXS"
MULXUs	.sdataz	"MULXU"
NEGs	.sdataz	"NEG"
NOPs	.sdataz	"NOP"
NOTs	.sdataz	"NOT"
ORs	.sdataz	"OR"
ORCs	.sdataz	"ORC"
POPs	.sdataz	"POP"
PUSHs	.sdataz	"PUSH"
ROTLs	.sdataz	"ROTL"
ROTRs	.sdataz	"ROTR"
ROTXLs	.sdataz	"ROTXL"
ROTXRs	.sdataz	"ROTXR"
RTEs	.sdataz	"RTE"
RTSs	.sdataz	"RTS"
SHALs	.sdataz	"SHAL"
SHARs	.sdataz	"SHAR"
SHLLs	.sdataz	"SHLL"
SHLRs	.sdataz	"SHLR"
SLEEPs	.sdataz	"SLEEP"
STCs	.sdataz	"STC"
SUBs	.sdataz	"SUB"
SUBSs	.sdataz	"SUBS"
SUBXs	.sdataz	"SUBX"
TRAPAs	.sdataz	"TRAPA"
XORs	.sdataz	"XOR"
XORCs	.sdataz	"XORC"

	.align	2
opc_ext	.sdataz	<0><0><0>
	.sdataz	".B"<0>
	.sdataz	".W"<0>
	.sdataz	".L"<0>

					;	H'00	(SP)
opr_01s	.sdataz	"#1"			;	H'01	#1
opr_02s	.sdataz	"#2"			;	H'02	#2
opr_03s	.sdataz	"CCR"			;	H'03	CCR
opr_04s	.sdataz	"#4"			;	H'04	#4
opr_05s	.sdataz	"@@H'"<0>":8"		;	H'05	@@aa:8
opr_06s	.sdataz	"H'"<0>":8"		;	H'06	d:8
opr_07s	.sdataz	"H'"<0>":16"		;	H'07	d:16
opr_08s	.sdataz	"#"<0>":2"		;	H'08	#xx:2
					;	H'10	RnH/RnL
					;	H'20	Rn
					;	H'30	ERn
opr_40s	.sdataz	"#"<0>":3"		;	H'40	#xx:3
opr_50s	.sdataz	"#H'"<0>":8"		;	H'50	#xx:8
opr_60s	.sdataz	"#H'"<0>":16"		;	H'60	#xx:16
opr_70s	.sdataz	"#H'"<0>":32"		;	H'70	#xx:32
opr_80s	.sdataz	"@"			;	H'80	@ER
opr_90s	.sdataz	"@(H'"<0>":16,"<0>")"	;	H'90	@(d:16,ER)
opr_A0s	.sdataz	"@(H'"<0>":24,"<0>")"	;	H'A0	@(d:24,ER)
opr_B0s	.sdataz	"@"<0>"+"		;	H'B0	@ER+
opr_C0s	.sdataz	"@-"			;	H'C0	@-ER
opr_D0s	.sdataz	"@H'"<0>":8"		;	H'D0	@aa:8
opr_E0s	.sdataz	"@H'"<0>":16"		;	H'E0	@aa:16
opr_F0s	.sdataz	"@H'"<0>":24"		;	H'F0	@aa:24
	.align	2

reg_byt	.sdataz	"R0H"
	.sdataz	"R1H"
	.sdataz	"R2H"
	.sdataz	"R3H"
	.sdataz	"R4H"
	.sdataz	"R5H"
	.sdataz	"R6H"
	.sdataz	"R7H"
	.sdataz	"R0L"
	.sdataz	"R1L"
	.sdataz	"R2L"
	.sdataz	"R3L"
	.sdataz	"R4L"
	.sdataz	"R5L"
	.sdataz	"R6L"
	.sdataz	"R7L"

reg_wrd	.sdataz	"R0"<0>
	.sdataz	"R1"<0>
	.sdataz	"R2"<0>
	.sdataz	"R3"<0>
	.sdataz	"R4"<0>
	.sdataz	"R5"<0>
	.sdataz	"R6"<0>
	.sdataz	"R7"<0>
	.sdataz	"E0"<0>
	.sdataz	"E1"<0>
	.sdataz	"E2"<0>
	.sdataz	"E3"<0>
	.sdataz	"E4"<0>
	.sdataz	"E5"<0>
	.sdataz	"E6"<0>
	.sdataz	"E7"<0>

reg_lng	.sdataz	"ER0"
	.sdataz	"ER1"
	.sdataz	"ER2"
	.sdataz	"ER3"
	.sdataz	"ER4"
	.sdataz	"ER5"
	.sdataz	"ER6"
	.sdataz	"ER7"

	.align	2
