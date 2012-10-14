kmain:
	mov dx,0x3B8
	mov al,0x1B
	out dx,al
	mov ax,0xB000
	mov ds,ax
	mov bx,0
	mov byte [bx], 'A'
	inc bx
	mov byte [bx], 0x71
	nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90

