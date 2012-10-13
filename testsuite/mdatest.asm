kmain:
mov ax, 0xB0000
mov ds, ax
mov bx, 0
mov ds:bx , 'A'
kmain_l:
nop
jmp near kmain_l
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90
