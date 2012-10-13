kmain:
nop
jmp near kmain
times 8176 - ($-$$) db 90
jmp near kmain
times 8192 - ($-$$) db 90
