kmain:
mov ax, 0xB0000
mov ds, ax
mov bx, 0
mov ds:bx , 'A'
kmain_l:
nop
jmp near kmain_l
align 8176, nop
jmp near kmain
align 8192, nop
