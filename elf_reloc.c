#include <os.h>
#include "elf.h"

extern Elf32_Ehdr elf_ehdr;
extern FILE* elf_fp;

void elf_fix_reloc(
    void (*callback)(unsigned char type, int a, Elf32_Addr offset, Elf32_Addr origval) 
) {
    //Look for relocs section
    Elf32_Sym symbol;
    Elf32_Shdr shdr;
    Elf32_Rel *ptr;
    
    int i;
    for (i=0; i<elf_ehdr.e_shnum; i++) {
        elf_read_section(i, &shdr);
        if (shdr.sh_type == 9) {
            int k, index;
            ptr = malloc(shdr.sh_size);
            elf_load_section_to_addr(&shdr, ptr, shdr.sh_size);
            for (k=0; k<shdr.sh_size/sizeof(Elf32_Rel); k++) {
                elf_get_symbol(ELF32_R_SYM(ptr[k].r_info), &symbol);
                callback(ELF32_R_TYPE(ptr[k].r_info), 
                         0, 
                         shdr.sh_addr+ptr[k].r_offset, 
                         symbol.st_value);
            }
            
            free(ptr);
        }
        if (shdr.sh_type == 4) {
            
        }
    }
}

Elf32_Addr elf_get_main() {
    Elf32_Shdr shdr;
    elf_get_symtab_section(&shdr);
    Elf32_Sym *symbols = malloc(shdr.sh_size);
    
    if (!symbols) asm("bkpt #69");
    elf_load_section_to_addr(&shdr, symbols, shdr.sh_size);
    
    Elf32_Addr rtn = elf_ehdr.e_entry;
    int i;
    for (i=0; i<shdr.sh_size/sizeof(Elf32_Sym); i++) {
        if (strncmp("main",elf_resolve_symbol_string(symbols[i].st_name, shdr.sh_link),4) == 0) {
            rtn = symbols[i].st_value;
            goto ret;
        }
    }
    //We didn't find a main. Let's look for a _start instead
    for (i=0; i<shdr.sh_size/sizeof(Elf32_Sym); i++) {
        if (strncmp("_start",elf_resolve_symbol_string(symbols[i].st_name, shdr.sh_link),6) == 0) {
            rtn = symbols[i].st_value;
            goto ret;
        }
    }
    
    ret:
    free(symbols);
    return rtn;
}