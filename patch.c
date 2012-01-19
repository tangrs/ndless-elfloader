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

#include "elf/elf.h"
#include "misc/console.h"
#include "misc/debug.h"
#include <os.h>
//If using a modified hook, this must be included after os.h
//as it overrides the HOOK_DEFINE macro
#include "magic_hook.h"

//Following function was ripped off Ndless arm/utils.c
static int ut_read_os_version_index(void) {
    switch (*(unsigned*)(OS_BASE_ADDRESS + 0x20)) {
        // OS-specific
        case 0x10211290:  // 1.7 non-CAS
            return 0;
        case 0x102132A0:  // 1.7 CAS
            return 1;
        case 0x10266030:  // 2.0.1 non-CAS
            return 2;
        case 0x10266900:  // 2.0.1 CAS
            return 3;
        case 0x10279D70:  // 2.1.0 non-CAS
            return 4;
        case 0x1027A640:  // 2.1.0 CAS
            return 5;
        default:
            return -1;
    }
}

static int is_elf(FILE* fp) {
    char magic[4];
    fread(magic, 1, 4, fp);
    return (memcmp("\x7f""ELF",magic,4) == 0);
}

HOOK_DEFINE(elf_hook) {
    //Most of this hook uses code borrowed from Ndless arm/ploaderhook.c

    char *halfpath; // [docfolder/]file.tns
    char docpath[100];

    int ut_os_version_index = ut_read_os_version_index();
    if (ut_os_version_index < 0) {
        show_msgbox("ELF Loader","Could not work out version index.\nYour calculator version is probably not supported\nIgnoring");
        HOOK_RESTORE_RETURN(elf_hook);
    }

    if (ut_os_version_index < 2) {
        halfpath = (char*)(HOOK_SAVED_REGS(elf_hook)[11] /* r11 */ - 0x124); // on the stack
    }else{
        halfpath = (char*)(HOOK_SAVED_REGS(elf_hook)[11] /* r11 */ + 12); // on the stack of the caller
    }
    // TODO use snprintf
    sprintf(docpath, "/documents/%s", halfpath);
    FILE *fp = fopen(docpath, "rb");
    if (!fp) {
        show_msgbox("ELF Loader","Could not open executable file.\nIgnoring");
        HOOK_RESTORE_RETURN(elf_hook);
    }

    if (is_elf(fp)) {
        //We found an elf file. Launch it!
        int intmask = TCT_Local_Control_Interrupts(-1); /* TODO workaround: disable the interrupts to avoid the clock on the screen */
        void *savedscr = malloc(SCREEN_BYTES_SIZE);
        int ret, success;
        memcpy(savedscr, SCREEN_BASE_ADDRESS, SCREEN_BYTES_SIZE);

        success = elf_execute(fp, &ret, 1, (char*[]){ docpath, NULL });

        if (success != 0) {
            show_msgbox("ELF Loader","Failed to launch ELF file\nSanity check possibly failed");
        }

        memcpy(SCREEN_BASE_ADDRESS, savedscr, SCREEN_BYTES_SIZE);
        free(savedscr);
        TCT_Local_Control_Interrupts(intmask);
        fclose(fp);
        HOOK_RESTORE_RETURN_SKIP(elf_hook, 4); //Skip Ndless program loader
    }

    //Not an ELF file - send to Ndless to deal with
    fclose(fp);
    HOOK_RESTORE_RETURN(elf_hook);
}
static unsigned const ins_ploader_hook_addrs[] = {0x1000921C, 0x100091F0, 0x10008BCC, 0x10008B9C, 0x10008D94, 0x10008D64};


//This hook detection code only works if nothing else is hooking over the ELF program loader
//(i.e. it is the 'top level' hook and not part of a chain)
int is_hook_already_installed(unsigned _addr) {
#ifdef ELF_MAGIC_HOOK
    unsigned *hook = ((unsigned **)_addr)[1];

    //addr[0]   = ldr pc, [pc, #4]
    //addr[1]   = pointer to hook
    //  |
    //  +-> hook[0] = add pc, pc, #0
    //      hook[1] = magic number

    if (hook[1] == ELF_MAGIC_HOOK) {
        return 1;
    }else{
        return 0;
    }
#else
    return 0;
#endif
}

int do_patch() {
    int index = ut_read_os_version_index();
    if (index < 0) return -1;

    if (!is_hook_already_installed(ins_ploader_hook_addrs[index])) {
        HOOK_INSTALL(ins_ploader_hook_addrs[index], elf_hook);
    }else{
        show_msgbox("ELF Loader", "Hook is already installed!");
        return -1;
    }
    return 0;
}