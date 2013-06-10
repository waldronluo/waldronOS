[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "a_nask.nas"]

    GLOBAL _api_putchar
    GLOBAL _api_end
    GLOBAL _api_putstr0
    GLOBAL _api_openwin 
    GLOBAL _api_putstrwin 
    GLOBAL _api_boxfilwin

[SECTION .text]

_api_putstr0:
    PUSH EBX
    MOV EDX,2
    MOV EBX,[ESP+8]
    INT 0x40
    POP EBX
    RET

_api_putchar:
    MOV EDX,1
    MOV AL,[ESP+4]
    INT 0x40
    RET

_api_end:
    MOV EDX,4
    INT 0x40

_api_openwin: ;int api_openwin (char *buf, int xsiz, int ysiz, int col_inv, char *title)
    PUSH EDI
    PUSH ESI
    PUSH EBX
    MOV EDX,5
    MOV EBX,[ESP+16]
    MOV ESI,[ESP+20]
    MOV EDI,[ESP+24]
    MOV EAX,[ESP+28]
    MOV ECX,[ESP+32]
    INT 0x40
    POP EBX
    POP ESI
    POP EDI
    RET

_api_putstrwin: ;void api_putstrwin (char *buf, int xsiz, int ysiz, int col_inv, char *title)
    PUSH EDI
    PUSH ESI
    PUSH EBX
    MOV EDX,5
    MOV EBX,[ESP+16]
    MOV ESI,[ESP+20]
    MOV EDI,[ESP+24]
    MOV EAX,[ESP+28]
    MOV ECX,[ESP+32]
    INT 0x40
    POP EBX
    POP ESI
    POP EDI
    RET

_api_boxfilwin: ;void api_boxfilwin (char *buf, int xsiz, int ysiz, int col_inv, char *title)
    PUSH EDI
    PUSH ESI
    PUSH EBX
    MOV EDX,5
    MOV EBX,[ESP+16]
    MOV ESI,[ESP+20]
    MOV EDI,[ESP+24]
    MOV EAX,[ESP+28]
    MOV ECX,[ESP+32]
    INT 0x40
    POP EBX
    POP ESI
    POP EDI
    RET
