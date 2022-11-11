bits 32
section .text
        ;multiboot spec
        align 4
        dd 0x1BADB002
        dd 0x00
        dd - (0x1BADB002 + 0x00)

global start
global _keyboard_handler
global _read_port
global _write_port
global _load_idt

extern _kmain
extern _keyboard_handler_main

_read_port:
	mov edx, [esp + 4]
	in al, dx
	ret

_write_port:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret

_load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

_keyboard_handler:                 
	call    _keyboard_handler_main
	iretd

start:
	cli
	mov esp, stack_space
	call _kmain
	hlt

section .bss
resb 8192; 8KB for stack
stack_space:
