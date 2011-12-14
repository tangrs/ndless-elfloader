
#include "elf.h"
#include <os.h>


Elf32_Ehdr elf_get_header(FILE* fp) {
    Elf32_Ehdr ehdr;
    
    rewind(fp);
    fread(&ehdr, sizeof(Elf32_Ehdr), 1, fp);
    
    return ehdr;
}

static int sh_index = 0;
static Elf32_Ehdr* sh_ehdr;
static FILE* sh_fp;

void elf_begin_read_section(FILE* fp, Elf32_Ehdr* ehdr) {
    sh_index = 0;
    sh_fp = fp;
    sh_ehdr = ehdr;
}

int elf_read_section(int index, Elf32_Ehdr* ehdr, Elf32_Shdr* write) {
    if (!(index < ehdr->e_shnum)) return -1;
    
    fseek(sh_fp, ehdr->e_shoff+(index*ehdr->e_shentsize), SEEK_SET);
    fread(write, 1, ehdr->e_shentsize, sh_fp);
    
    return 0;
}

Elf32_Shdr* elf_read_next_section() {
    static Elf32_Shdr shdr;
    int ret = elf_read_section(sh_index, sh_ehdr, &shdr);
    if (ret == 0) sh_index++;
    
    return (ret == 0 ? &shdr : NULL);
}

char* elf_resolve_string(FILE* fp, int index, Elf32_Ehdr* ehdr) {
    static char string[128];
    if (!ehdr->e_shstrndx) return NULL;
    
    Elf32_Shdr string_section;
    int ret = elf_read_section(ehdr->e_shstrndx, ehdr, &string_section);
    if (ret != 0) return NULL;
    
    fseek(fp, string_section.sh_offset+index, SEEK_SET);
    fread(string, 1, sizeof(string)-1, fp);
    
    string[sizeof(string)-1] = 0;
    
    return string;
}

int main() {
    FILE* fp = fopen("test/parseelf.elf.tns","rb");
    Elf32_Shdr* shdr;
    void * baseptr = NULL;
    void * sectionptr = NULL;
    size_t imagesize = 0;
    if (!fp) return 0;
    
    Elf32_Ehdr hdr = elf_get_header(fp);
    
    elf_begin_read_section(fp, &hdr);
    
    unsigned int prev_addr = 0;
    unsigned int prev_size = 0;
    unsigned int base_addr = 0xffffffff;
    
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
                    " Total image size is now %dbytes\n",
            elf_resolve_string(fp, shdr->sh_name, &hdr),
            shdr->sh_size,shdr->sh_addr, imagesize);
        }
        
    }
    printf("The base address was determined to be 0x%x\n",base_addr);
    
    #define RESOLVE_ADDR(x) (void*)(((uint32_t)x)-base_addr+((uint32_t)baseptr))
    
    //Now the process is loaded into RAM
    //We're going to look for a .got section to fill in
    
    elf_begin_read_section(fp, &hdr);
    while(shdr = elf_read_next_section()) {
        if (shdr->sh_flags & 0x2) {
            if (strcmp(elf_resolve_string(fp, shdr->sh_name, &hdr),".got") == 0) {
                //Relocate
                
                Elf32_Addr *addr = RESOLVE_ADDR(shdr->sh_addr);
                int entries = shdr->sh_size/sizeof(Elf32_Addr), i;
                for (i=0; i<entries; i++) {
                    addr[i] = RESOLVE_ADDR(addr[i]);
                }
                                    printf("Patched GOT\n");
                    idle();
                break;
            }
        }
    }
    
    fclose(fp);
    
    char *argv[] = { "test/parseelf.elf.tns", 0 };
    
    clear_cache();
    idle();
    
    /*char buffer[100];
    sprintf(buffer, "%p", baseptr);
    show_msgbox("Debug", buffer);*/
    
    printf("Total size of image was %d. Now jumping to entry point\n\n", imagesize);
    
    ((int (*)(int, char*[]))(RESOLVE_ADDR(hdr.e_entry)))(1, argv);
    printf("Image (probably) ran successfully!\n");
    printf("Total size is %dbytes\n", imagesize);
    free(baseptr);
    
    return 0;
}