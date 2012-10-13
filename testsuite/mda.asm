kmain:
	mov ax,0xB000
	mov ds,ax
	mov bx,0
	mov byte [bx], 'A'
	nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90

