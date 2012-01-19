/*
    ndless-elfloader Loads ELF files
    Copyright (C) 2011  Daniel Tang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <os.h>
#include "elf.h"


static void *sectionptr;
static void *baseptr; //this is the real, absolute address where the image is loaded to.
static Elf32_Addr baseaddr; //this is the lowest virtual address that is specified in the ELF header
static Elf32_Addr prevaddr;
static size_t imagesize, prevsize;

#define RESOLVE_ADDR(x) (void*)( ((uint32_t)(x)) - ((uint32_t)baseaddr) + ((uint32_t)baseptr) )

static void reloc_callback(unsigned char type, int a, Elf32_Addr offset, Elf32_Addr origval)  {
    if (type == R_ARM_ABS32) { //Only process R_ARM_ABS32 entries. I think that's all we really need to fix up anyway
#ifdef DEBUG
        console_printf("Patched offset: %x from %x", offset, *((uint32_t*)RESOLVE_ADDR(offset)));
#endif
        //This thing does:
        // 1. Resolve the offset to a real memory address
        // 2. Dereferences that address to get the address stored at that address
        // 3. Resolve that address to the real memory address.
        // 4. ...and stores it back into the resolved offset.
        *((void**)RESOLVE_ADDR(offset)) = RESOLVE_ADDR(*((void**)RESOLVE_ADDR(offset)));
#ifdef DEBUG
        console_printf(" to %x\n", *((uint32_t*)RESOLVE_ADDR(offset)));
#endif
    }
}

static void load_section_callback(Elf32_Shdr* shdr) {
    if (shdr->sh_flags & SHF_ALLOC) { //Only copy sections marked with "ALLOC"
        if (prevaddr == 0xffffffff) prevaddr = shdr->sh_addr;
        if (shdr->sh_addr < baseaddr) baseaddr = shdr->sh_addr;

        size_t padding = (shdr->sh_addr-prevaddr)-prevsize;
        size_t newsize = imagesize+shdr->sh_size+padding;

        baseptr = realloc(baseptr, newsize);
        sectionptr = ((char*)baseptr)+imagesize;

        if (padding > 0) { //Do we really need to memset padding?
            memset(sectionptr, 0, padding);
            sectionptr = (char*)sectionptr+padding;
        }
        prevsize = shdr->sh_size;
        prevaddr = shdr->sh_addr;
        imagesize = newsize;

        switch (shdr->sh_type) {
                case SHT_PROGBITS: //PROGBITS
                    elf_load_section_to_addr(shdr, sectionptr, shdr->sh_size);
                    break;
                case SHT_NOBITS: //NOBITS
                    memset(sectionptr, 0, shdr->sh_size);
                    break;
                default:
                    break;
        }
#ifdef DEBUG
        console_printf("Loaded section %s\n",
        elf_resolve_section_string(shdr->sh_name));
#endif
    }
}

int elf_execute(FILE* fp, int *ret, int argc, char *argv[]) {
    if (!fp) return -1;
    sectionptr = NULL;
    baseptr = NULL;
    imagesize = 0;
    prevsize = 0;
    baseaddr = 0xffffffff;
    prevaddr = 0xffffffff;

    Elf32_Shdr shdr;

    elf_set_file(fp);

    if (elf_sanity_check() != 0) return -1;
    Elf32_Ehdr hdr = elf_get_header();

#ifdef DEBUG
    console_printf("---Begin loading ELF file---\n");
    console_printf("Loading sections to memory\n");
#endif
    elf_process_sections(load_section_callback);
#ifdef DEBUG
    console_printf("Finished loading image\n");

    console_printf("---Begin relocating---\n");
#endif
    elf_get_got_section(&shdr);
    Elf32_Addr *got = RESOLVE_ADDR(shdr.sh_addr); //Get address of where the GOT is loaded in memory.

    //Begin patching GOT
    int i;
    for (i=0; i<shdr.sh_size/sizeof(Elf32_Addr); i++) { //Iterate through and fix up entries.
#ifdef DEBUG
        console_printf("Patched GOT entry from %x",got[i]);
#endif
        if (got[i] >= baseaddr) {
            got[i] = (Elf32_Addr)RESOLVE_ADDR(got[i]);
#ifdef DEBUG
            console_printf(" to %p\n", RESOLVE_ADDR(got[i]) );
#endif
        }else{
            //0x0 is special. Not sure what is exactly meant to be filled in
            //Fill in baseptr by default. Must double check in ELF specs
            got[i] = (Elf32_Addr)baseptr;
#ifdef DEBUG
            console_printf(" to %p\n", baseptr);
#endif
        }
    }

    //Fix up static initliazers and global variables
    elf_fix_reloc(reloc_callback);

    clear_cache();

    //Now we're done!
#ifdef DEBUG
    console_printf("Finished relocating\n");
#endif

    void* entrypoint = RESOLVE_ADDR(elf_get_main()); //Get the address of main or _start

#ifdef DEBUG
    console_printf("Total size of image was %d.\n"
           "Now jumping to entry point at 0x%p. It's bye bye from now\n\n", imagesize, entrypoint);
#endif
    //Woot, run the executable!
    *ret = ((int (*)(int, char*[]))(entrypoint))(argc, argv);
#ifdef DEBUG
    console_printf("\nImage (probably) ran successfully!\n"
           "Freeing memory and exiting ELF loader\n");
#endif
    free(baseptr);
    return 0;
}