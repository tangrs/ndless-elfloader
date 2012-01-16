# Elfloader for Ndless

This project is a ELF-loader for use with Ndless. It relocated and runs ELF executables on Ndless. Written by tangrs.

## Status

The loader is currently in a BETA-ish stage. It works and it's been made easy enough to use but can be buggy. It's also still carrying a lot of development related cruft.

It's only aimed at OS's from 1.7 to 2.1 at the moment but it's very easy to port to newer OS's (pretty much just add some function addresses to the hooking function).

## Compiling

Clone this repo to some location on your computer. Alternatively, download an archive and extract it.

Make sure you have a working Ndless SDK.

Change into the directory and run ```./make_loader.sh``` and it should compile everything.

## How to use

Compile the source and copy load.tns to your calculator. Run it and it will install the hook.

Make sure to only run the hook once per reboot or else it'll leak memory (TODO: fix)

This hook will run before Ndless's program loader and load ELF files if it encounters them. Your normal Ndless programs will continue to work.

## How do I develop apps to work on this?

Simply add ```--emit-relocs``` to your LDFLAGS. Then, grab the .elf file produced, add the .tns extension and copy it to your calculator.

## Why should I load an ELF file vs loading a traditional Ndless file?

At the moment, the way Ndless links and loads executables is:

1. Link the files together and add extra symbols so the global offset table can be found.
2. Slap on a startup file with position independent code that modifies the GOT on the fly then jumps to entry point.
3. Dump the compiled ELF file into a memory image. This is the executable.
4. The program loader on Ndless simply copies the binary into memory at runtime and executes it without doing any preparation since the PIC at the start of the program will handle all the GOT relocation.

While the method of fixing the GOT just in time for execution works, it doesn't update the function pointers in statically allocated variables. This is probably because during link time, the function addresses in static variables are hard coded into the .data section. As soon as it relocates however, the hardcoded function addresses become invalid. The relocation code doesn't (and can't because once it's dumped to a memory image, all the symbol definitions are gone) take in account of this. So, when you run code like

    void foo() {}

    int main() {
        static void (*var)() = foo;
         var();
        return 0;
    }

it crashes because the value of 'var' is hardcoded during compile time and not updated during runtime.

Although you can easily work around this in C, C++ is a different kettle of fish. Most of my C++ code with virtual functions compiled for Ndless also crash possibly because of the same problem except with vtables (i.e. function pointers in vtables not being changed to correct one during relocation at runtime).

In other words, you should use ELF file loading over the traditional program loading method if you like having the convinience of all your static function pointers relocated automatically or if you're working with C++ code.

## Why shouldn't I use this?

Because it has a huge performance impact on loading time. A complicated program with a lot of symbols can take several seconds to load.

If you don't normally load ELF files, the hook also has a small overhead on loading normal Ndless binaries because it has to check whether it's an ELF file.

## Licence

Licenced under GPLv3. See LICENSE.