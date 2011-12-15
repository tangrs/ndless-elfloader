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
