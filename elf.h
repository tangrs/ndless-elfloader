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

#define EM_ARM 40
#define ET_EXEC 2
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

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value; 
    Elf32_Word st_size; 
    unsigned char st_info; 
    unsigned char st_other; 
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info; 
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_SWord r_addend; 
} Elf32_Rela;

enum {
    ELF_SANITY_OK = 0,
    ELF_SANITY_FAIL = -1
};

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

void elf_set_file(FILE* fp);
Elf32_Ehdr elf_get_header();
int elf_get_section(int index, Elf32_Shdr* write);
int elf_get_symtab_section(Elf32_Shdr* write);
void elf_load_section_to_addr(Elf32_Shdr *shdr, void* ptr, size_t max);

char* elf_resolve_section_string(int index);
char* elf_resolve_string(int index, int section_index);
void elf_get_symbol(int index, Elf32_Sym *symbol);

void elf_fix_reloc(
    void (*callback)(unsigned char type, int a, Elf32_Addr offset, Elf32_Addr origval) 
);

int elf_execute(FILE* fp, int *ret, int argc, char *argv[]);

Elf32_Addr elf_get_main();