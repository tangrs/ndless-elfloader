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

#include "elf.h"
#include <os.h>

#define RESOLVE_ADDR(x) (void*)( ((uint32_t)(x)) - ((uint32_t)base_addr) + ((uint32_t)baseptr) )

static void *baseptr;
static Elf32_Addr *base_addr;

void callback(unsigned char type, int a, Elf32_Addr offset, Elf32_Addr origval)  {
    if (type != 2) return;
    
    printf(" Patched Offset: %x Original Value: %x New value: %x\n", offset, origval, RESOLVE_ADDR(origval));
    
    *((uint32_t*)RESOLVE_ADDR(offset)) = RESOLVE_ADDR(origval);
    
}

int main() {
    FILE* fp = fopen("test/parseelf.elf.tns","rb");
    Elf32_Shdr* shdr;
    baseptr = NULL;
    void * sectionptr = NULL;
    size_t imagesize = 0;
    if (!fp) return 0;
    
    elf_set_file(fp);
    Elf32_Ehdr hdr = elf_get_header();
    
    elf_begin_read_section();
    
    unsigned int prev_addr = 0;
    unsigned int prev_size = 0;
    base_addr = 0xffffffff;
    
    while(shdr = elf_read_next_section()) {
        if (shdr->sh_flags & 0x2) {
            if (prev_addr == 0) prev_addr = shdr->sh_addr;
            if (shdr->sh_addr < base_addr) base_addr = shdr->sh_addr;
            
            size_t padding = (shdr->sh_addr-prev_addr)-prev_size;
            size_t newsize = imagesize+shdr->sh_size+padding;
            
            baseptr = realloc(baseptr, newsize);
            sectionptr = ((char*)baseptr)+imagesize;
            
            if (padding > 0) {
                memset(sectionptr, 0, padding);
                sectionptr = (char*)sectionptr+padding;
            }
            prev_size = shdr->sh_size;
            prev_addr = shdr->sh_addr;
            imagesize = newsize;
            
            switch (shdr->sh_type) {
                case 1:
                    rewind(fp);
                    fseek(fp, shdr->sh_offset,SEEK_SET);
                    memset(sectionptr, 0xbb, shdr->sh_size);
                    int bytes= fread(sectionptr, 1, shdr->sh_size, fp);
                    
                    printf("Read %dbytes from file to memory\n",bytes);
                    idle();
                    break;
                case 8:
                    memset(sectionptr, 0, shdr->sh_size);
                    printf("Set %dbytes of memory\n", shdr->sh_size);
                    idle();
                    break;
                default:
                    break;
            }
            printf("Copied section %s total size is %dbytes addr is %x\n"
                    "Total image size is now %dbytes\n",
            elf_resolve_string(shdr->sh_name),
            shdr->sh_size,shdr->sh_addr, imagesize);
        }
        
    }
    printf("The base address was determined to be 0x%x\n",base_addr);
    printf("The loaded base address was 0x%p\n",baseptr);
    
    //Now the process is loaded into RAM
    //We're going to look for a .got section to fill in
    
    elf_begin_read_section();
    while(shdr = elf_read_next_section()) {
        if (shdr->sh_flags & 0x2) {
            if (strcmp(elf_resolve_string(shdr->sh_name),".got") == 0) {
                //Relocate
                
                Elf32_Addr *addr = RESOLVE_ADDR(shdr->sh_addr);
                int entries = shdr->sh_size/sizeof(Elf32_Addr), i;
                for (i=0; i<entries; i++) {
                    printf("Patched GOT entry from %x",addr[i]);
                    if (addr[i] >= base_addr) {
                        addr[i] = RESOLVE_ADDR(addr[i]);
                        printf(" to %x\n", RESOLVE_ADDR(addr[i]) );
                    }else{
                        addr[i] = baseptr;
                        printf(" to %x\n", baseptr);
                    }
                }

                break;
            }
        }
    }
    
    elf_fix_reloc(callback);
    
    fclose(fp);
    
    char *argv[] = { "test/parseelf.elf.tns", 0 };
    
    clear_cache();
    idle();
    
    printf("Total size of image was %d. Now jumping to entry point\n\n", imagesize);
    
    ((int (*)(int, char*[]))(RESOLVE_ADDR(hdr.e_entry)))(1, argv);
    printf("\nImage (probably) ran successfully!\n");
    printf("Total size is %dbytes\n", imagesize);
    free(baseptr);
    
    return 0;
}