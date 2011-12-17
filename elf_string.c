#include <os.h>
#include "elf.h"

extern Elf32_Ehdr elf_ehdr;
extern FILE* elf_fp;


//Resolve section header strings. Should be merged with below function
char* elf_resolve_string(int index) {
    static char string[128];
    if (!elf_ehdr.e_shstrndx) return NULL;
    
    Elf32_Shdr string_section;
    int ret = elf_read_section(elf_ehdr.e_shstrndx, &string_section);
    if (ret != 0) return NULL;
    
    fseek(elf_fp, string_section.sh_offset+index, SEEK_SET);
    fread(string, 1, sizeof(string)-1, elf_fp);
    
    string[sizeof(string)-1] = 0;
    
    return string;
}

//Resolve a string given a section index - needs renaming
char* elf_resolve_symbol_string(int index, int section_index) {
    static char string[128];
    Elf32_Shdr string_section;
    elf_read_section(section_index, &string_section);
    fseek(elf_fp, string_section.sh_offset+index, SEEK_SET);
    fread(string, 1, sizeof(string)-1, elf_fp);
    return string;
}