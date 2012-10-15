kmain:
	
	mov dx,0x3BF
	mov al,0x3
	out dx,al
	
	mov dx,0x3B8
	mov al,0xAB
	out dx,al
	mov ax,0xB000
	mov ds,ax
	mov bx,0
	mov byte [bx], 'A'
	inc bx
	mov byte [bx], 0x09
	
	inc bx
	mov byte [bx], 'B'
	inc bx
	mov byte [bx], 0x0A
	
	inc bx
	mov byte [bx], 'C'
	inc bx
	mov byte [bx], 0x70
	
	inc bx
	mov byte [bx], 'D'
	inc bx
	mov byte [bx], 0xF0
	
	nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90

