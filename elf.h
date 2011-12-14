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


#include <stdint.h>
#include <os.h>

typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_SWord;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Addr;

#define EI_NIDENT 16
/* ELF File Header */
typedef struct {

  unsigned char e_ident[EI_NIDENT];
  Elf32_Half    e_type;
  Elf32_Half    e_machine;
  Elf32_Word    e_version;
  Elf32_Addr    e_entry;
  Elf32_Off     e_phoff;
  Elf32_Off     e_shoff;
  Elf32_Word    e_flags;
  Elf32_Half    e_ehsize;
  Elf32_Half    e_phentsize;
  Elf32_Half    e_phnum;
  Elf32_Half    e_shentsize;
  Elf32_Half    e_shnum;
  Elf32_Half    e_shstrndx;

} Elf32_Ehdr;

/* Section header */
typedef struct {

  Elf32_Word   sh_name;
  Elf32_Word   sh_type;
  Elf32_Word   sh_flags;
  Elf32_Addr   sh_addr;
  Elf32_Off    sh_offset;
  Elf32_Word   sh_size;
  Elf32_Word   sh_link;
  Elf32_Word   sh_info;
  Elf32_Word   sh_addralign;
  Elf32_Word   sh_entsize;

} Elf32_Shdr;

/* Program header */
typedef struct {

  Elf32_Word   p_type;
  Elf32_Off    p_offset;
  Elf32_Addr   p_vaddr;
  Elf32_Addr   p_paddr;
  Elf32_Word   p_filesz;
  Elf32_Word   p_memsz;
  Elf32_Word   p_flags;
  Elf32_Word   p_align;

} Elf32_Phdr;