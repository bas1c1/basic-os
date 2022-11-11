bits 32
global entry
extern _kmain
section .text
entry:
	jmp start
    align 4
    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002 + 0x00)
start:
	cli
    mov esp, stack_space
    call _kmain
    hlt

section .bss
resb 8192
stack_space: