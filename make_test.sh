#!/bin/sh

arm-none-eabi-gcc -c test.c -I/Users/danieltang/ndless/trunk/include -fpic
arm-none-eabi-ld test.o -o parseelf.elf.tns --emit-relocs