bits 32

extern kmain

section .text

global stub32
stub32:
	mov edx, 0xB8000 + 158
	mov al, "b"
	mov ah, 0xF
	mov [edx], ax

	mov esp, kernel_stack_top
	call kmain

die:
	cli
	hlt
	jmp die

section .bss

align 4
kernel_stack_bottom: equ $
	resb 8192
kernel_stack_top:
