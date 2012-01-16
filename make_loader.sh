#!/bin/sh

GCC='nspire-gcc'
CFLAGS='-Wno-attributes'

$GCC $CFLAGS -c main.c
$GCC $CFLAGS -c patch.c
$GCC $CFLAGS -c elf/elf_header.c
$GCC $CFLAGS -c elf/elf_section.c
$GCC $CFLAGS -c elf/elf_string.c
$GCC $CFLAGS -c elf/elf_reloc.c
$GCC $CFLAGS -c elf/elf_load.c
$GCC $CFLAGS -c misc/console.c

nspire-ld patch.o main.o elf_load.o elf_header.o elf_reloc.o elf_section.o elf_string.o console.o -o load.elf
arm-none-eabi-objcopy -O binary load.elf load.tns