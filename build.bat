@echo off
@nasm -f elf32 boot.asm -o kasm.o
@move kasm.o "build"
@gcc -m32 -c kernel.c -o kc.o
@move kc.o "build"
@copy link.ld "build"
@cd "build"
@ld -T link.ld -m i386pe -o kernel kasm.o kc.o -build-id=none
@objcopy -O elf32-i386 kernel kernel.elf
@del link.ld
