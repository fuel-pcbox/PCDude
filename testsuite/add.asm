kmain:
	xor al,al
	loop1:
		daa
		inc al
		jnz loop1
	nop
jmp near kmain
align 8176, nop
jmp near kmain
align 8192, nop

