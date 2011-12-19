#!/bin/sh

GCC='nspire-gcc'
CFLAGS='-Wno-attributes'

$GCC $CFLAGS -c load.c
$GCC $CFLAGS -c elf_header.c
$GCC $CFLAGS -c elf_section.c
$GCC $CFLAGS -c elf_string.c
$GCC $CFLAGS -c elf_reloc.c
$GCC $CFLAGS -c elf_load.c
$GCC $CFLAGS -c ndless_load.c

nspire-ld elf_load.o ndless_load.o load.o elf_header.o elf_reloc.o elf_section.o elf_string.o -o load.elf
arm-none-eabi-objcopy -O binary load.elf load.tns