[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "helloa.nas"]
    global _HariMain
[SECTION .text]

_HariMain:
    mov edx,2
    mov ebx,msg
    int 0x40
    mov edx,4
    int 0x40
[SECTION .data]
msg:
    db "helloa",0
