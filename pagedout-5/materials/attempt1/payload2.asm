section .init
payload:
	sysexit:
		xor rax,rax
		mov al,60
		mov rdi,1
		syscall
