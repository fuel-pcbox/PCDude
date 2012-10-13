kmain:
	xor al,al
	loop1:
		daa
		inc al
		jnz loop1
	nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90

