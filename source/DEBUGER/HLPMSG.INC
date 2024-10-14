;
;	Program	HLPMSG.INC
;	Date	1998/9/19 .. 1998/9/19
;	Copyright (C) 1998 by AKIYA
;
;--- up date ---
; 1998/11/15 0.59 watch dog reset help message
; 1998/12/12 0.60 change memory size help message
; 1999/10/17 0.61 debugger work size valiable, user function
; 2000/5/27  0.62 view list, view trace
;

	;****************
	;* help messege *
	;****************

hlp_msg:
	mov.l	#hlp_msg_mon,er1
	mov.b	#"*",r0h
	bra	_hlp_msg:8

hlp_msg2:
	mov.l	#hlp_msg_dbg,er1
	mov.b	#">",r0h
_hlp_msg
	push	er1
	mov.l	#_debug_entry_msg,er1
	bsr	puts
	mov.l	#hlp_msg_def,er1
	bsr	puts
	pop	er1
hlp_msg_0
	push	er1	;!
	mov.l	@er1,er1
	beq	hlp_msg_1:8
	bsr	putc_crlf
	bsr	hlp_msg_2:8
	bsr	puts
	pop	er1	;!
	adds	#4,er1
	push	er1	;!
	mov.l	@er1,er1
	beq	hlp_msg_1:8
	bsr	hlp_msg_3:8
	bsr	puts
	pop	er1	;!
	adds	#4,er1
	bra	hlp_msg_0:8
hlp_msg_1
	pop	er1	;!
	orc	#B'00000001,ccr
	rts
hlp_msg_2
	mov.b	#" ",r0l
	bsr	putc
hlp_msg_3
	mov.b	r0h,r0l
	bra	putc

hlp_msg_mon:
	.data.l	exi_cmd_hlp
	.data.l	dbg_min_hlp
	.data.l	dmp_mem_hlp
	.data.l	dmp_siz_hlp
	.data.l	dis_asm_hlp
	.data.l	ini_mem_hlp
	.data.l	chn_mem_hlp
	.data.l	chn_msz_hlp
	.data.l	dmp_wch_hlp
	.data.l	wch_siz_hlp
	.data.l	wch_res_hlp
	.data.l	usr_fnc_hlp
	.data.l	set_usr_hlp
	.data.l	clr_usr_hlp
	.data.l	dsp_usr_hlp
	.data.l	hlp_msg_hlp
	.data.l	0

hlp_msg_dbg:
	.data.l	dbg_out_hlp
	.data.l	dmp_mem_hlp
	.data.l	dmp_siz_hlp
	.data.l	dis_asm_hlp
	.data.l	ini_mem_hlp
	.data.l	chn_mem_hlp
	.data.l	chn_msz_hlp
	.data.l	chn_reg_hlp
	.data.l	dmp_ipr_hlp
	.data.l	dmp_reg_hlp
	.data.l	go_hlp
	.data.l	one_step_hlp
	.data.l	one_trace_hlp
	.data.l	step_hlp
	.data.l	trace_hlp
	.data.l	pcb_set_hlp
	.data.l	pcb_clr_hlp
	.data.l	pcb_dsp_hlp
	.data.l	wch_res_hlp
	.data.l	usr_fnc_hlp
	.data.l	set_usr_hlp
	.data.l	clr_usr_hlp
	.data.l	dsp_usr_hlp
	.data.l	vew_lst_hlp
	.data.l	vew_trc_hlp
	.data.l	vew_stp_hlp
	.data.l	hlp_msg_hlp
	.data.l	0

hlp_msg_def	;123456789012345678901234567890123456789
	.data.b	H'0d,H'0a
	.sdata	" SS=start, EE=end, LL=length, WW=wide,"
	.sdataz	" NN=line, II=initial, RR=reg., BB=addr."
exi_cmd_hlp
	.sdataz	"@[CR]           exit monitor          "
dbg_min_hlp
	.sdataz	"DI[CR]          debug mode in         "
dbg_out_hlp
	.sdataz	"DO[CR]          debug mode out        "
dmp_mem_hlp
	.sdataz	"DM-SS,EE[CR]    dump memory           "
dmp_siz_hlp
	.sdataz	"DS-SS,LL,WW[CR] dump memory size      "
dis_asm_hlp
	.sdataz	"DA-SS,NN[CR]    dis assemble          "
ini_mem_hlp
	.sdataz	"IM-SS,EE,II[CR] initial memory        "
chn_mem_hlp
	.sdataz	"CM-SS[CR]       change memory         "
chn_msz_hlp
	.sdataz	"CS-SS,WW[CR]    change memory size    "
chn_reg_hlp
	.sdataz	"CR-RR[CR]       change register       "
dmp_ipr_hlp
	.sdataz	"DI[CR]          dump reg. and inst.   "
dmp_reg_hlp
	.sdataz	"DR[CR]          dump register         "
go_hlp
	.sdataz	"G[CR]           go                    "
one_step_hlp
	.sdataz	"S[CR]           single step           "
one_trace_hlp
	.sdataz	"T[CR]           single trace          "
step_hlp
	.sdataz	"ST-NN[CR]       step                  "
trace_hlp
	.sdataz	"TR-NN[CR]       trace                 "
pcb_set_hlp
	.sdataz	"PB-BB[CR]       program break set     "
pcb_clr_hlp
	.sdataz	"_PB[CR]         program break clear   "
pcb_dsp_hlp
	.sdataz	"?PB[CR]         program break disp    "
dmp_wch_hlp
	.sdataz	"DW-SS,EE[CR]    dump memory watch     "
wch_siz_hlp
	.sdataz	"WS-SS,LL,WW[CR] watch memory size     "
hlp_msg_hlp
	.sdataz	"?[CR]           this help message     "
wch_res_hlp
	.sdataz	"Reset[CR]       system reset          "
usr_fnc_hlp
	.sdataz	"U               user function call    "
set_usr_hlp
	.sdataz	"SU-BB[CR]       set user function     "
clr_usr_hlp
	.sdataz	"_SU[CR]         clear user function   "
dsp_usr_hlp
	.sdataz	"?SU[CR]         disp user function    "
vew_lst_hlp
	.sdataz	"VL              view list             "
vew_trc_hlp
	.sdataz	"VT              view trace            "
vew_stp_hlp
	.sdataz	"VS              view step             "
	.align	2

