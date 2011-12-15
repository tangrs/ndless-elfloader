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