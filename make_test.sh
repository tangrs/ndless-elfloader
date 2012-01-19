#!/bin/sh

GCC='nspire-gcc'
CFLAGS='-Wno-attributes'

echo Building test
$GCC $CFLAGS -c tests/test.c
$GCC $CFLAGS -c elf/elf_header.c
$GCC $CFLAGS -c elf/elf_section.c
$GCC $CFLAGS -c elf/elf_string.c
$GCC $CFLAGS -c elf/elf_reloc.c
$GCC $CFLAGS -c elf/elf_load.c
$GCC $CFLAGS -c misc/console.c
# using nspire-ld cause I'm lazy
nspire-ld elf_load.o elf_header.o elf_reloc.o elf_section.o elf_string.o console.o test.o -o test.elf.tns -Wl,--emit-relocs

echo Building unittest
$GCC $CFLAGS -c tests/unittest.c -fpic
arm-none-eabi-ld unittest.o -o unittest.elf.tns --emit-relocs -T/Users/danieltang/ndless/trunk/system/ldscript

echo Building benchmark
arm-none-eabi-gcc -c tests/benchmark.c -fpic
arm-none-eabi-ld benchmark.o -o benchmark.elf.tns --emit-relocs -T/Users/danieltang/ndless/trunk/system/ldscript