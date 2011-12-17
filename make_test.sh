#!/bin/sh

arm-none-eabi-gcc -c test.c -I/Users/danieltang/ndless/trunk/include -fpic
arm-none-eabi-ld test.o -o parseelf.elf.tns --emit-relocs -T/Users/danieltang/ndless/trunk/system/ldscript -L/Users/danieltang/ndless/trunk/lib -lndls