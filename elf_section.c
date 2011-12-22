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

extern Elf32_Ehdr elf_ehdr;
extern FILE* elf_fp;

//Foreach section, call callback to process
void elf_process_sections(void (*callback)(Elf32_Shdr* shdr)) {
    int i;
    Elf32_Shdr shdr;

    for (i=0; elf_get_section(i, &shdr)==0; i++) {
        callback(&shdr);
    }
}

//Read the section header given at the index into a struct.
int elf_get_section(int index, Elf32_Shdr* write) {
    if (!(index < elf_ehdr.e_shnum)) return -1;

    fseek(elf_fp, elf_ehdr.e_shoff+(index*elf_ehdr.e_shentsize), SEEK_SET);
    fread(write, 1, elf_ehdr.e_shentsize, elf_fp);

    return 0;
}
//Find the symbol table section header and write it into a struct
int elf_get_symtab_section(Elf32_Shdr* write) {
    //Look for symtab section
    Elf32_Shdr shdr;
    int i;
    for (i=0; elf_get_section(i, &shdr)==0; i++) {
        if (shdr.sh_type == 2) { //2 is the identifier for symtab. Needs to be turned into a enum or #define
            *write = shdr;
            return 0;
        }
    }
    return -1;
}
//Find the got table section header and write it into a struct
int elf_get_got_section(Elf32_Shdr* write) {
    //Look for got section
    Elf32_Shdr shdr;
    int i;
    for (i=0; elf_get_section(i, &shdr)==0; i++) {
        if (strcmp(elf_resolve_section_string(shdr.sh_name),".got") == 0) {
            *write = shdr;
            return 0;
        }
    }
    return -1;
}
//Resolve a symbol index into a symbol struct
void elf_get_symbol(int index, Elf32_Sym *symbol) {
    Elf32_Shdr shdr;
    elf_get_symtab_section(&shdr);

    fseek(elf_fp, shdr.sh_offset+(index*shdr.sh_entsize), SEEK_SET);
    fread(symbol, 1, shdr.sh_entsize, elf_fp);
}

//Simply loads the section data into a specified address
void elf_load_section_to_addr(Elf32_Shdr *shdr, void* ptr, size_t max) {
    fseek(elf_fp, shdr->sh_offset, SEEK_SET);
    fread(ptr, 1, (max < shdr->sh_size ? max : shdr->sh_size), elf_fp);
}