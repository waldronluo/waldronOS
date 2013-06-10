[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_task_now
	EXTERN	_memman_alloc_4k
	EXTERN	_file_readfat
	EXTERN	_queue8_init
	EXTERN	_timer_alloc
	EXTERN	_timer_init
	EXTERN	_timer_settimer
	EXTERN	_io_cli
	EXTERN	_queue8_status
	EXTERN	_queue8_get
	EXTERN	_io_sti
	EXTERN	_boxfill8
	EXTERN	_sheet_refresh
	EXTERN	_find_palette
	EXTERN	_task_sleep
	EXTERN	_putfonts8_asc_sht
	EXTERN	_memman_total
	EXTERN	_sprintf
	EXTERN	_file_search
	EXTERN	_file_loadfile
	EXTERN	_memman_free_4k
	EXTERN	_set_segmdesc
	EXTERN	_start_app
[FILE "console.c"]
[SECTION .text]
	GLOBAL	_console_task
_console_task:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	LEA	EBX,DWORD [-540+EBP]
	SUB	ESP,572
	CALL	_task_now
	PUSH	11520
	PUSH	3932160
	MOV	DWORD [-576+EBP],EAX
	CALL	_memman_alloc_4k
	PUSH	1049088
	PUSH	EAX
	MOV	DWORD [-580+EBP],EAX
	CALL	_file_readfat
	MOV	ESI,DWORD [-576+EBP]
	MOV	EAX,DWORD [8+EBP]
	ADD	ESI,16
	MOV	DWORD [-540+EBP],EAX
	LEA	EAX,DWORD [-524+EBP]
	MOV	DWORD [-536+EBP],8
	MOV	DWORD [-532+EBP],28
	MOV	DWORD [-528+EBP],-1
	MOV	DWORD [4076],EBX
	PUSH	DWORD [-576+EBP]
	PUSH	EAX
	PUSH	128
	PUSH	ESI
	CALL	_queue8_init
	ADD	ESP,32
	CALL	_timer_alloc
	PUSH	1
	MOV	EDI,EAX
	PUSH	ESI
	PUSH	EAX
	CALL	_timer_init
	PUSH	50
	PUSH	EDI
	CALL	_timer_settimer
	PUSH	1
	PUSH	62
	PUSH	EBX
	CALL	_cons_putchar
	ADD	ESP,32
L22:
	CALL	_io_cli
	MOV	EBX,DWORD [-576+EBP]
	ADD	EBX,16
	PUSH	EBX
	CALL	_queue8_status
	POP	EDX
	TEST	EAX,EAX
	JE	L26
	PUSH	EBX
	CALL	_queue8_get
	MOV	ESI,EAX
	CALL	_io_sti
	POP	EAX
	CMP	ESI,1
	JLE	L27
L7:
	CMP	ESI,2
	JE	L28
L12:
	CMP	ESI,3
	JE	L29
L13:
	LEA	EBX,DWORD [-256+ESI]
	MOV	DWORD [-584+EBP],EBX
	CMP	EBX,255
	JA	L14
	CMP	ESI,264
	JE	L30
	CMP	ESI,266
	JE	L31
	MOV	ECX,DWORD [-536+EBP]
	CMP	ECX,475
	JG	L14
	MOV	EAX,ECX
	MOV	EDX,8
	MOV	EBX,EDX
	PUSH	1
	CDQ
	IDIV	EBX
	MOV	ECX,EAX
	MOV	EAX,ESI
	MOV	BYTE [-574+EBP+ECX*1],AL
	LEA	EAX,DWORD [-540+EBP]
	PUSH	DWORD [-584+EBP]
	PUSH	EAX
L24:
	CALL	_cons_putchar
L25:
	ADD	ESP,12
L14:
	CMP	DWORD [-528+EBP],-1
	JE	L21
	MOV	ECX,DWORD [-532+EBP]
	MOV	EDX,DWORD [-536+EBP]
	LEA	EAX,DWORD [16+ECX]
	PUSH	EAX
	LEA	EAX,DWORD [7+EDX]
	PUSH	EAX
	PUSH	ECX
	MOV	ECX,DWORD [8+EBP]
	PUSH	EDX
	MOVZX	EAX,BYTE [-528+EBP]
	PUSH	EAX
	PUSH	DWORD [8+ECX]
	PUSH	DWORD [4+ECX]
	CALL	_boxfill8
	ADD	ESP,28
L21:
	MOV	ECX,DWORD [-532+EBP]
	MOV	EDX,DWORD [-536+EBP]
	LEA	EAX,DWORD [16+ECX]
	PUSH	EAX
	LEA	EAX,DWORD [8+EDX]
	PUSH	EAX
	PUSH	ECX
	PUSH	EDX
	PUSH	DWORD [8+EBP]
	CALL	_sheet_refresh
	ADD	ESP,20
	JMP	L22
L31:
	PUSH	0
	LEA	EBX,DWORD [-540+EBP]
	PUSH	32
	PUSH	EBX
	CALL	_cons_putchar
	PUSH	EBX
	MOV	EAX,DWORD [-536+EBP]
	MOV	EDX,8
	MOV	ECX,EDX
	CDQ
	IDIV	ECX
	MOV	BYTE [-574+EBP+EAX*1],0
	CALL	_cons_newline
	LEA	EAX,DWORD [-572+EBP]
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [-580+EBP]
	PUSH	EBX
	PUSH	EAX
	CALL	_cons_runcmd
	ADD	ESP,32
	PUSH	1
	PUSH	62
	PUSH	EBX
	JMP	L24
L30:
	CMP	DWORD [-536+EBP],16
	JLE	L14
	PUSH	0
	LEA	EAX,DWORD [-540+EBP]
	PUSH	32
	PUSH	EAX
	CALL	_cons_putchar
	SUB	DWORD [-536+EBP],8
	JMP	L25
L29:
	MOV	ECX,DWORD [-532+EBP]
	MOV	EDX,DWORD [-536+EBP]
	LEA	EAX,DWORD [15+ECX]
	PUSH	EAX
	LEA	EAX,DWORD [7+EDX]
	PUSH	EAX
	PUSH	ECX
	PUSH	EDX
	PUSH	0
	CALL	_find_palette
	MOV	ECX,DWORD [8+EBP]
	MOVZX	EAX,AL
	MOV	DWORD [ESP],EAX
	PUSH	DWORD [8+ECX]
	PUSH	DWORD [4+ECX]
	CALL	_boxfill8
	ADD	ESP,28
	MOV	DWORD [-528+EBP],-1
	JMP	L13
L28:
	PUSH	16777215
	CALL	_find_palette
	MOV	DWORD [-528+EBP],EAX
	POP	EAX
	JMP	L12
L27:
	TEST	ESI,ESI
	JE	L8
	PUSH	0
	PUSH	EBX
	PUSH	EDI
	CALL	_timer_init
	ADD	ESP,12
	CMP	DWORD [-528+EBP],0
	JS	L10
	PUSH	16777215
L23:
	CALL	_find_palette
	POP	EBX
	MOV	DWORD [-528+EBP],EAX
L10:
	PUSH	50
	PUSH	EDI
	CALL	_timer_settimer
	POP	EDX
	POP	ECX
	JMP	L7
L8:
	PUSH	1
	PUSH	EBX
	PUSH	EDI
	CALL	_timer_init
	ADD	ESP,12
	CMP	DWORD [-528+EBP],0
	JS	L10
	PUSH	0
	JMP	L23
L26:
	PUSH	DWORD [-576+EBP]
	CALL	_task_sleep
	CALL	_io_sti
	POP	EAX
	JMP	L22
[SECTION .data]
LC0:
	DB	" ",0x00
[SECTION .text]
	GLOBAL	_cons_putchar
_cons_putchar:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	MOV	DL,BYTE [16+EBP]
	MOV	EBX,DWORD [8+EBP]
	MOV	BYTE [-7+EBP],DL
	MOV	BYTE [-6+EBP],AL
	CMP	AL,9
	MOV	BYTE [-5+EBP],0
	JE	L34
	CMP	AL,10
	JE	L46
	CMP	AL,13
	JE	L32
	PUSH	1
	LEA	EAX,DWORD [-6+EBP]
	PUSH	EAX
	PUSH	0
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	16777215
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	DWORD [8+EBX]
	PUSH	DWORD [4+EBX]
	PUSH	DWORD [EBX]
	CALL	_putfonts8_asc_sht
	ADD	ESP,28
	CMP	BYTE [-7+EBP],0
	JE	L32
	MOV	EAX,DWORD [4+EBX]
	ADD	EAX,8
	MOV	DWORD [4+EBX],EAX
	CMP	EAX,475
	JG	L46
L32:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L46:
	PUSH	EBX
	CALL	_cons_newline
	POP	ECX
	JMP	L32
L34:
	PUSH	1
	PUSH	LC0
	PUSH	0
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	16777215
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	DWORD [8+EBX]
	PUSH	DWORD [4+EBX]
	PUSH	DWORD [EBX]
	CALL	_putfonts8_asc_sht
	ADD	ESP,28
	MOV	EAX,DWORD [4+EBX]
	ADD	EAX,8
	MOV	DWORD [4+EBX],EAX
	CMP	EAX,475
	JG	L47
L37:
	MOV	EAX,DWORD [4+EBX]
	SUB	EAX,8
	AND	EAX,31
	JNE	L34
	JMP	L32
L47:
	PUSH	EBX
	CALL	_cons_newline
	POP	EAX
	JMP	L37
	GLOBAL	_cons_newline
_cons_newline:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	EAX
	PUSH	EAX
	MOV	EAX,DWORD [8+EBP]
	MOV	EDX,DWORD [8+EBP]
	MOV	EAX,DWORD [EAX]
	MOV	DWORD [-20+EBP],EAX
	MOV	EAX,DWORD [8+EDX]
	CMP	EAX,475
	JG	L49
	ADD	EAX,16
	MOV	DWORD [8+EDX],EAX
L50:
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [4+EAX],8
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L49:
	MOV	DWORD [-16+EBP],28
L60:
	MOV	EDI,8
L59:
	MOV	EAX,DWORD [-20+EBP]
	MOV	EDX,DWORD [-16+EBP]
	MOV	ECX,DWORD [8+EAX]
	MOV	EBX,DWORD [4+EAX]
	MOV	EAX,DWORD [-16+EBP]
	ADD	EAX,16
	IMUL	EAX,ECX
	IMUL	EDX,ECX
	LEA	EAX,DWORD [EAX+EDI*1]
	LEA	EDX,DWORD [EDX+EDI*1]
	INC	EDI
	MOV	AL,BYTE [EAX+EBX*1]
	CMP	EDI,483
	MOV	BYTE [EDX+EBX*1],AL
	JLE	L59
	INC	DWORD [-16+EBP]
	CMP	DWORD [-16+EBP],476
	JLE	L60
	MOV	DWORD [-16+EBP],476
L70:
	MOV	EDI,8
L69:
	MOV	EDX,DWORD [-20+EBP]
	MOV	EBX,DWORD [-16+EBP]
	IMUL	EBX,DWORD [8+EDX]
	MOV	ESI,DWORD [4+EDX]
	PUSH	0
	LEA	EBX,DWORD [EBX+EDI*1]
	CALL	_find_palette
	INC	EDI
	MOV	BYTE [EBX+ESI*1],AL
	CMP	EDI,483
	POP	EAX
	JLE	L69
	INC	DWORD [-16+EBP]
	CMP	DWORD [-16+EBP],492
	JLE	L70
	PUSH	492
	PUSH	492
	PUSH	28
	PUSH	8
	PUSH	DWORD [-20+EBP]
	CALL	_sheet_refresh
	ADD	ESP,20
	JMP	L50
[SECTION .data]
LC1:
	DB	"mem",0x00
LC2:
	DB	"cls",0x00
LC3:
	DB	"clear",0x00
LC4:
	DB	"dir",0x00
LC5:
	DB	"ls",0x00
LC6:
	DB	"type ",0x00
LC7:
	DB	"cat ",0x00
LC8:
	DB	"BAD COMMAND",0x00
[SECTION .text]
	GLOBAL	_cons_runcmd
_cons_runcmd:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	EDI,LC1
	PUSH	EBX
	SUB	ESP,12
	MOV	EAX,DWORD [8+EBP]
	MOV	ECX,DWORD [16+EBP]
	MOV	DWORD [-24+EBP],EAX
	MOV	DWORD [-16+EBP],ECX
	MOV	EAX,DWORD [20+EBP]
	MOV	ESI,DWORD [-24+EBP]
	MOV	DWORD [-20+EBP],EAX
	MOV	EBX,DWORD [12+EBP]
	MOV	EAX,4
	CLD
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L93
	MOV	EAX,4
	MOV	ESI,DWORD [-24+EBP]
	CLD
	MOV	EDI,LC2
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L83
	MOV	EAX,6
	MOV	ESI,DWORD [-24+EBP]
	MOV	EDI,LC3
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L83
	MOV	EAX,4
	MOV	ESI,DWORD [-24+EBP]
	CLD
	MOV	EDI,LC4
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L86
	MOV	EAX,3
	MOV	ESI,DWORD [-24+EBP]
	MOV	EDI,LC5
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L86
	MOV	EAX,5
	MOV	ESI,DWORD [-24+EBP]
	CLD
	MOV	EDI,LC6
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L89
	MOV	EAX,4
	MOV	ESI,DWORD [-24+EBP]
	MOV	EDI,LC7
	MOV	ECX,EAX
	REPE
	CMPSB
	JE	L89
	MOV	EAX,DWORD [-24+EBP]
	CMP	BYTE [EAX],0
	JNE	L94
L79:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L94:
	PUSH	EAX
	PUSH	DWORD [-16+EBP]
	PUSH	EBX
	CALL	_cmd_app
	ADD	ESP,12
	TEST	EAX,EAX
	JNE	L79
	PUSH	12
	PUSH	LC8
	PUSH	0
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	16777215
	CALL	_find_palette
	MOV	DWORD [ESP],EAX
	PUSH	DWORD [8+EBX]
	PUSH	8
	PUSH	DWORD [EBX]
	CALL	_putfonts8_asc_sht
	MOV	DWORD [8+EBP],EBX
	ADD	ESP,28
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cons_newline
L89:
	MOV	EAX,DWORD [-24+EBP]
	MOV	ECX,DWORD [-16+EBP]
	MOV	DWORD [16+EBP],EAX
	MOV	DWORD [12+EBP],ECX
	MOV	DWORD [8+EBP],EBX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cmd_cat
L86:
	MOV	DWORD [8+EBP],EBX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cmd_ls
L83:
	MOV	DWORD [8+EBP],EBX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cmd_cls
L93:
	MOV	EAX,DWORD [-20+EBP]
	MOV	DWORD [8+EBP],EBX
	MOV	DWORD [12+EBP],EAX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cmd_mem
[SECTION .data]
LC9:
	DB	"total",0x09,"%dMB",0x0A,"free",0x09,"%dKB",0x0A,0x0A,0x00
[SECTION .text]
	GLOBAL	_cmd_mem
_cmd_mem:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	SUB	ESP,64
	MOV	EBX,DWORD [12+EBP]
	SHR	EBX,20
	PUSH	3932160
	CALL	_memman_total
	SHR	EAX,10
	MOV	DWORD [ESP],EAX
	PUSH	EBX
	PUSH	LC9
	LEA	EBX,DWORD [-68+EBP]
	PUSH	EBX
	CALL	_sprintf
	PUSH	EBX
	PUSH	DWORD [8+EBP]
	CALL	_cons_putstr0
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
	GLOBAL	_cmd_cls
_cmd_cls:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	EAX
	PUSH	EAX
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [-16+EBP],28
	MOV	EAX,DWORD [EAX]
	MOV	DWORD [-20+EBP],EAX
L106:
	MOV	EDI,8
L105:
	MOV	EAX,DWORD [-20+EBP]
	MOV	EBX,DWORD [-16+EBP]
	IMUL	EBX,DWORD [8+EAX]
	MOV	ESI,DWORD [4+EAX]
	PUSH	0
	LEA	EBX,DWORD [EBX+EDI*1]
	CALL	_find_palette
	INC	EDI
	MOV	BYTE [EBX+ESI*1],AL
	CMP	EDI,491
	POP	EAX
	JLE	L105
	INC	DWORD [-16+EBP]
	CMP	DWORD [-16+EBP],491
	JLE	L106
	PUSH	492
	PUSH	492
	PUSH	28
	PUSH	8
	PUSH	DWORD [-20+EBP]
	CALL	_sheet_refresh
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [8+EAX],28
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
[SECTION .data]
LC10:
	DB	"filename.ext    %7d",0x0A,0x00
[SECTION .text]
	GLOBAL	_cmd_ls
_cmd_ls:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	XOR	ESI,ESI
	PUSH	EBX
	XOR	EBX,EBX
	SUB	ESP,32
	MOV	EDI,DWORD [8+EBP]
L124:
	MOV	AL,BYTE [1058304+EBX]
	TEST	AL,AL
	JE	L111
	CMP	AL,-27
	JE	L114
	TEST	BYTE [1058315+EBX],24
	JE	L128
L114:
	INC	ESI
	ADD	EBX,32
	CMP	ESI,223
	JLE	L124
L111:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L128:
	PUSH	DWORD [1058332+EBX]
	PUSH	LC10
	LEA	EAX,DWORD [-44+EBP]
	PUSH	EAX
	CALL	_sprintf
	LEA	EDX,DWORD [1058304+EBX]
	XOR	ECX,ECX
	ADD	ESP,12
L123:
	MOV	AL,BYTE [EDX]
	INC	EDX
	MOV	BYTE [-44+EBP+ECX*1],AL
	INC	ECX
	CMP	ECX,7
	JLE	L123
	MOV	AL,BYTE [1058312+EBX]
	MOV	BYTE [-35+EBP],AL
	MOV	AL,BYTE [1058313+EBX]
	MOV	BYTE [-34+EBP],AL
	MOV	AL,BYTE [1058314+EBX]
	MOV	BYTE [-33+EBP],AL
	LEA	EAX,DWORD [-44+EBP]
	PUSH	EAX
	PUSH	EDI
	CALL	_cons_putstr0
	POP	EDX
	POP	ECX
	JMP	L114
[SECTION .data]
LC11:
	DB	"File Not Found.",0x0A,0x00
[SECTION .text]
	GLOBAL	_cmd_cat
_cmd_cat:
	PUSH	EBP
	MOV	ECX,4
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	EDI,LC7
	PUSH	EBX
	PUSH	EBX
	PUSH	EBX
	MOV	EAX,DWORD [8+EBP]
	MOV	EBX,DWORD [16+EBP]
	MOV	DWORD [-16+EBP],EAX
	MOV	ESI,EBX
	MOV	EAX,DWORD [12+EBP]
	CLD
	MOV	DWORD [-20+EBP],EAX
	REPE
	CMPSB
	SETNE	AL
	MOVZX	EAX,AL
	PUSH	224
	PUSH	1058304
	LEA	EAX,DWORD [4+EBX+EAX*1]
	PUSH	EAX
	CALL	_file_search
	ADD	ESP,12
	MOV	ESI,EAX
	TEST	EAX,EAX
	JE	L132
	PUSH	DWORD [28+EAX]
	PUSH	3932160
	CALL	_memman_alloc_4k
	PUSH	1064448
	PUSH	DWORD [-20+EBP]
	PUSH	EAX
	MOV	EBX,EAX
	PUSH	DWORD [28+ESI]
	MOVZX	EAX,WORD [26+ESI]
	PUSH	EAX
	CALL	_file_loadfile
	PUSH	DWORD [28+ESI]
	PUSH	EBX
	PUSH	DWORD [-16+EBP]
	CALL	_cons_putstr1
	MOV	EAX,DWORD [28+ESI]
	MOV	DWORD [16+EBP],EAX
	ADD	ESP,40
	MOV	DWORD [12+EBP],EBX
	MOV	DWORD [8+EBP],3932160
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_memman_free_4k
L132:
	MOV	EAX,DWORD [-16+EBP]
	MOV	DWORD [12+EBP],LC11
	MOV	DWORD [8+EBP],EAX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_cons_putstr0
[SECTION .data]
LC12:
	DB	"Hari",0x00
LC13:
	DB	0x0A,".hrb file format error.",0x0A,0x00
[SECTION .text]
	GLOBAL	_cmd_app
_cmd_app:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	XOR	EBX,EBX
	SUB	ESP,56
	MOV	ESI,DWORD [16+EBP]
	CALL	_task_now
	MOV	DWORD [-52+EBP],EAX
L140:
	MOV	AL,BYTE [EBX+ESI*1]
	CMP	AL,32
	JLE	L136
	MOV	BYTE [-44+EBP+EBX*1],AL
	INC	EBX
	CMP	EBX,12
	JLE	L140
L136:
	PUSH	224
	LEA	ESI,DWORD [-44+EBP]
	PUSH	1058304
	PUSH	ESI
	MOV	BYTE [-44+EBP+EBX*1],0
	CALL	_file_search
	ADD	ESP,12
	TEST	EAX,EAX
	MOV	DWORD [-48+EBP],EAX
	JE	L155
L154:
	MOV	EAX,DWORD [-48+EBP]
	PUSH	DWORD [28+EAX]
	PUSH	3932160
	CALL	_memman_alloc_4k
	MOV	EDX,DWORD [-48+EBP]
	PUSH	1064448
	PUSH	DWORD [12+EBP]
	PUSH	EAX
	MOV	DWORD [-68+EBP],EAX
	PUSH	DWORD [28+EDX]
	MOVZX	EAX,WORD [26+EDX]
	PUSH	EAX
	CALL	_file_loadfile
	MOV	EDI,DWORD [-48+EBP]
	ADD	ESP,28
	CMP	DWORD [28+EDI],35
	JBE	L144
	MOV	ESI,DWORD [-68+EBP]
	MOV	EAX,LC12
	CLD
	ADD	ESI,4
	MOV	ECX,4
	MOV	EDI,EAX
	REPE
	CMPSB
	JNE	L144
	MOV	EAX,DWORD [-68+EBP]
	CMP	BYTE [EAX],0
	JE	L156
L144:
	PUSH	LC13
	PUSH	DWORD [8+EBP]
	CALL	_cons_putstr0
	POP	ESI
	POP	EDI
L150:
	MOV	EDI,DWORD [-48+EBP]
	PUSH	DWORD [28+EDI]
	PUSH	DWORD [-68+EBP]
	PUSH	3932160
	CALL	_memman_free_4k
	PUSH	DWORD [8+EBP]
	CALL	_cons_newline
	MOV	EAX,1
L134:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L156:
	MOV	EDX,DWORD [EAX]
	MOV	EDI,DWORD [12+EAX]
	MOV	EBX,DWORD [16+EAX]
	MOV	DWORD [-56+EBP],EDX
	MOV	EAX,DWORD [20+EAX]
	MOV	DWORD [-60+EBP],EDI
	PUSH	EDX
	MOV	DWORD [-64+EBP],EAX
	PUSH	3932160
	CALL	_memman_alloc_4k
	MOV	EDX,DWORD [-48+EBP]
	MOV	DWORD [4072],EAX
	MOV	ESI,EAX
	PUSH	16634
	PUSH	DWORD [-68+EBP]
	MOV	EAX,DWORD [28+EDX]
	DEC	EAX
	PUSH	EAX
	PUSH	2563928
	CALL	_set_segmdesc
	MOV	EAX,DWORD [-56+EBP]
	PUSH	16626
	DEC	EAX
	PUSH	ESI
	PUSH	EAX
	PUSH	2563936
	CALL	_set_segmdesc
	ADD	ESP,40
	TEST	EBX,EBX
	JLE	L153
	MOV	EDX,DWORD [-64+EBP]
	MOV	ECX,EDI
	ADD	EDX,DWORD [-68+EBP]
	ADD	ECX,ESI
L149:
	MOV	AL,BYTE [EDX]
	INC	EDX
	MOV	BYTE [ECX],AL
	INC	ECX
	DEC	EBX
	JNE	L149
L153:
	MOV	EAX,DWORD [-52+EBP]
	ADD	EAX,44
	PUSH	EAX
	PUSH	8032
	PUSH	DWORD [-60+EBP]
	PUSH	8024
	PUSH	27
	CALL	_start_app
	PUSH	DWORD [-56+EBP]
	PUSH	ESI
	PUSH	3932160
	CALL	_memman_free_4k
	ADD	ESP,32
	JMP	L150
L155:
	CMP	BYTE [-45+EBP+EBX*1],46
	JE	L141
	PUSH	224
	PUSH	1058304
	PUSH	ESI
	MOV	BYTE [-44+EBP+EBX*1],46
	MOV	BYTE [-43+EBP+EBX*1],119
	MOV	BYTE [-42+EBP+EBX*1],97
	MOV	BYTE [-41+EBP+EBX*1],108
	MOV	BYTE [-40+EBP+EBX*1],0
	CALL	_file_search
	ADD	ESP,12
	MOV	DWORD [-48+EBP],EAX
L141:
	CMP	DWORD [-48+EBP],0
	JNE	L154
	CMP	BYTE [-45+EBP+EBX*1],46
	JE	L142
	PUSH	224
	PUSH	1058304
	PUSH	ESI
	MOV	BYTE [-44+EBP+EBX*1],46
	MOV	BYTE [-43+EBP+EBX*1],104
	MOV	BYTE [-42+EBP+EBX*1],114
	MOV	BYTE [-41+EBP+EBX*1],98
	MOV	BYTE [-40+EBP+EBX*1],0
	CALL	_file_search
	ADD	ESP,12
	MOV	DWORD [-48+EBP],EAX
L142:
	CMP	DWORD [-48+EBP],0
	JNE	L154
	XOR	EAX,EAX
	JMP	L134
	GLOBAL	_cons_putstr0
_cons_putstr0:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [12+EBP]
	MOV	ESI,DWORD [8+EBP]
	CMP	BYTE [EBX],0
	JNE	L162
L164:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L162:
	PUSH	1
	MOVSX	EAX,BYTE [EBX]
	PUSH	EAX
	INC	EBX
	PUSH	ESI
	CALL	_cons_putchar
	ADD	ESP,12
	CMP	BYTE [EBX],0
	JNE	L162
	JMP	L164
	GLOBAL	_cons_putstr1
_cons_putstr1:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [16+EBP]
	XOR	EBX,EBX
	MOV	EDI,DWORD [12+EBP]
	CMP	EBX,ESI
	JL	L170
L172:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L170:
	PUSH	1
	MOVSX	EAX,BYTE [EBX+EDI*1]
	PUSH	EAX
	INC	EBX
	PUSH	DWORD [8+EBP]
	CALL	_cons_putchar
	ADD	ESP,12
	CMP	EBX,ESI
	JL	L170
	JMP	L172
	GLOBAL	_wal_api
_wal_api:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	EDI,DWORD [24+EBP]
	MOV	EBX,DWORD [28+EBP]
	MOV	ESI,DWORD [4072]
	CALL	_task_now
	CMP	EBX,1
	MOV	EDX,DWORD [4076]
	JE	L182
	CMP	EBX,2
	JE	L183
	CMP	EBX,3
	JE	L184
	CMP	EBX,4
	JE	L185
L175:
	XOR	EAX,EAX
L173:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L185:
	ADD	EAX,44
	JMP	L173
L184:
	PUSH	DWORD [32+EBP]
	LEA	EAX,DWORD [ESI+EDI*1]
	PUSH	EAX
	PUSH	EDX
	CALL	_cons_putstr1
L181:
	ADD	ESP,12
	JMP	L175
L183:
	LEA	EAX,DWORD [ESI+EDI*1]
	PUSH	EAX
	PUSH	EDX
	CALL	_cons_putstr0
	POP	EAX
	POP	EDX
	JMP	L175
L182:
	PUSH	1
	MOVZX	EAX,BYTE [36+EBP]
	PUSH	EAX
	PUSH	EDX
	CALL	_cons_putchar
	JMP	L181
