#!/bin/sh

arm-none-eabi-gcc -c test.c -I/Users/danieltang/ndless/trunk/include -fPIC
arm-none-eabi-ld test.o -o parseelf.elf.tns