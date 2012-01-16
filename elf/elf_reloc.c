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
#include "../misc/debug.h"

extern Elf32_Ehdr elf_ehdr;
extern FILE* elf_fp;

static inline int strfind(char* haystack, char* needle) {
    int haystacksize = strlen(haystack);
    int needlesize = strlen(needle);
    if (!needlesize) return -1;

    int i;
    for (i=0; i<haystacksize-needlesize+1; i++) {
        if (haystack[i] == needle[0]) { //If first character matches
            int k;
            for (k=1; k<needlesize; k++) { //Start matching from the second character
                if (haystack[i+k] != needle[k])    goto notfound;
            }
            return i;
            notfound:
            continue;
        }
    }
    return -1;
}

#define DEBUG_STR ".debug"
static inline int is_debug_section(int index) {
    char* sectionstr = elf_resolve_section_string(index);
    console_printf("%s\n",sectionstr);
    assert(sectionstr);

    return (strfind(sectionstr, DEBUG_STR) != -1);
}

//Function to fix symbol relocations
//Call back is type = type of relocation, a (unimplemented atm), offset = fixup address, original value (should be ignored?)
void elf_fix_reloc(
    void (*callback)(unsigned char type, int a, Elf32_Addr offset, Elf32_Addr origval)
) {
    //Look for relocs section
    Elf32_Sym symbol;
    Elf32_Shdr shdr;
    Elf32_Rel *rel;

    int i;
    for (i=0; i<elf_ehdr.e_shnum; i++) {
        assert(elf_get_section(i, &shdr) == 0);
        if (!is_debug_section(shdr.sh_name)) {
            if (shdr.sh_type == SHT_REL) {
    			int k, index;
    			rel = malloc(shdr.sh_size);
    			assert(rel);
    			elf_load_section_to_addr(&shdr, rel, shdr.sh_size);

    			//For each reloc entry, call the callback to handle it
    			for (k=0; k<shdr.sh_size/sizeof(Elf32_Rel); k++) {
    				elf_get_symbol(ELF32_R_SYM(rel[k].r_info), &symbol);
    				callback(ELF32_R_TYPE(rel[k].r_info),
    						0,
    						rel[k].r_offset,
    						symbol.st_value);
    			}
    			free(rel);
    		}
    		if (shdr.sh_type == SHT_RELA) {
    			//Unimplemented
    		}
    	}
    }
}

//We'd rather keep as much compatibility with executables created with nspire-ld already
//End goal is really keep 99.95% compatibility. The user should only need to add --emit-relocs to
//their LDFLAGS and compile normally except without using objcopy at the end.
//Ndless adds startup files which relocates again (redundant). Not only that,
//the ELF entry point says that execution should begin at address 0x0. Unfortunately, for
//nspire-ld linked files, that's actually wrong. It really should begin at 0x4 because the first
//few bytes is a magic number, not instructions.

//In short, we can't trust the ELF's entry point. We want to skip straight to main() without calling
//all that redundant startup code. This function finds the address of main() and returns it.
Elf32_Addr elf_get_main() {
    Elf32_Shdr shdr;
    elf_get_symtab_section(&shdr);
    Elf32_Sym *symbols = malloc(shdr.sh_size);
    elf_load_section_to_addr(&shdr, symbols, shdr.sh_size);

    Elf32_Addr rtn = elf_ehdr.e_entry;
    int i;
    //Iterate the symbol list for a "main"
    for (i=0; i<shdr.sh_size/sizeof(Elf32_Sym); i++) {
        if (strncmp("main",elf_resolve_string(symbols[i].st_name, shdr.sh_link),4) == 0) {
            rtn = symbols[i].st_value;
            goto ret; //Found it, return it.
        }
    }
    //We didn't find a main. Let's look for a _start instead
    for (i=0; i<shdr.sh_size/sizeof(Elf32_Sym); i++) {
        if (strncmp("_start",elf_resolve_string(symbols[i].st_name, shdr.sh_link),6) == 0) {
            rtn = symbols[i].st_value;
            goto ret; //Found it, return it.
        }
    }
    //We didn't find anything, return entry point specified in ELF header by default.
    ret:
    free(symbols);
    return rtn;
}