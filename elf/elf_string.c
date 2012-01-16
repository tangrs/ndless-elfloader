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


//Resolve section header strings.
char* elf_resolve_section_string(int index) {
    return elf_resolve_string(index, elf_ehdr.e_shstrndx);
}

//Resolve a string given a section index - needs renaming
char* elf_resolve_string(int index, int section_index) {
    static char string[128];
    Elf32_Shdr string_section;
    elf_get_section(section_index, &string_section);
    fseek(elf_fp, string_section.sh_offset+index, SEEK_SET);
    fread(string, 1, sizeof(string)-1, elf_fp);
    return string;
}