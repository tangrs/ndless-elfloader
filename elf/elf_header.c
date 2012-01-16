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

Elf32_Ehdr elf_ehdr;
FILE* elf_fp;

void elf_set_file(FILE* fp) {
    elf_fp = fp;
    elf_ehdr = elf_get_header();
}

Elf32_Ehdr elf_get_header() {
    Elf32_Ehdr ehdr;
    rewind(elf_fp);
    fread(&ehdr, sizeof(Elf32_Ehdr), 1, elf_fp);
    return ehdr;
}

int elf_sanity_check() {
    if (memcmp("\x7f""ELF",elf_ehdr.e_ident,4) != 0) {
        console_printf("ELF magic header does not match\n");
        return -1;
    }
    if (elf_ehdr.e_type != ET_EXEC) {
        console_printf("ELF file not an executable\n");
        return -1;
    }
    if (elf_ehdr.e_machine != EM_ARM) {
        console_printf("ELF file is not for ARM architecture\n");
        return -1;
    }
    if (elf_ehdr.e_version != 1) {
        console_printf("ELF version is invalid\n");
        return -1;
    }
    if (elf_ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
        console_printf("ELF file is not 32bits\n");
        return -1;
    }
    return 0;
}