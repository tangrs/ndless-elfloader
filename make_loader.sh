#!/bin/sh

nspire-gcc -c load.c -I/Users/danieltang/ndless/trunk/include
nspire-ld load.o -o load.elf
arm-none-eabi-objcopy -O binary load.elf load.tns