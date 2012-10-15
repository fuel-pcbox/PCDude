kmain:

mov dx,0x3D8
mov al,0x1E
out dx,al

mov dx,0x3D4
mov al,1
out dx,al
inc dx
mov al,80
out dx,al
dec dx
mov al,6
out dx,al
inc dx
mov al,200
out dx,al
dec dx
mov al,9
out dx,al
inc dx
xor al,al
out dx,al

mov ax,0xB800
mov ds,ax
mov bx,0
mov cx,0x8000

xor al,al

loop1:
mov byte [bx],al
inc al
inc bx
loop loop1

jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90