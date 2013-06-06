[INSTRSET "i486p"]
[BITS 32]
    mov edx,1
    mov ebx,msg
    int 0x40
    retf
msg:
    db "helloa",0
