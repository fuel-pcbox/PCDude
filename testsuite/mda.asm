kmain:
	mov dx,0x3B8
	mov al,0x1B
	out dx,al
	mov ax,0xB000
	mov ds,ax
	mov bx,0
	mov byte [bx], 'A'
	inc bx
	mov byte [bx], 0x09
	
	mov dx,0x3B8
	mov al,0x1B
	out dx,al
	mov ax,0xB000
	mov ds,ax
	mov bx,2
	mov byte [bx], 'B'
	inc bx
	mov byte [bx], 0x0A
	
	mov dx,0x3B8
	mov al,0x1B
	out dx,al
	mov ax,0xB000
	mov ds,ax
	mov bx,4
	mov byte [bx], 'C'
	inc bx
	mov byte [bx], 0x70
	
	nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90

