#include <os.h>
#include "elf.h"

static int sh_index = 0;
extern Elf32_Ehdr elf_ehdr;
extern FILE* elf_fp;

//Begin looping though the list of sections - 
//should eventually be replaced with callback type thing
void elf_begin_read_section() {
    sh_index = 0;
}
//Read the next section
Elf32_Shdr* elf_read_next_section() {
    static Elf32_Shdr shdr;
    int ret = elf_read_section(sh_index, &shdr);
    if (ret == 0) sh_index++;
    
    return (ret == 0 ? &shdr : NULL);
}

//Read the section header given at the index into a struct.
int elf_read_section(int index, Elf32_Shdr* write) {
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
    for (i=0; i<elf_ehdr.e_shnum; i++) {
        elf_read_section(i, &shdr);
        if (shdr.sh_type == 2) { //2 is the identifier for symtab. Needs to be turned into a enum or #define
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