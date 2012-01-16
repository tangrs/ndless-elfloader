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

#include "patch.h"
#include "elf/elf.h"
#include "misc/console.h"
#include "misc/debug.h"
#include <os.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "patch") == 0) {
            //We have been called by ourselves to install the hook
            return do_patch();
        }
    }

    //TODO: detect if hook has already been installed

    assert(argc);
    assert(argv[0]);
    //If we are here, we have been called by the main program loader
    //Since the hook needs to stay in a safe place in memory, we need to
    //copy our executable to somewhere where we won't be free'd

    struct stat docstat;
    stat(argv[0], &docstat);

    FILE* fp = fopen(argv[0],"rb");
    assert(fp);

    void* reloc = malloc(docstat.st_size);
    if (!reloc) {
        show_msgbox("ELF Loader","Unable to allocate memory");
        return 0;
    }
    //Load ourself into memory that won't be free'd
    if (!fread(reloc, docstat.st_size, 1, fp)) {
        show_msgbox("ELF Loader","Unable to relocate hook");
        free(reloc);
        return 0;
    }
    fclose(fp);

    clear_cache();
    //Call our clone to install the hook
    int ret = ((int (*)(int argc, char *argv[]))(reloc + sizeof(PRGMSIG)))(2, (char*[]){argv[0], "patch", NULL});

    if (ret == 0) {
        show_msgbox("ELF Loader","Successfully installed hook");
    }else{
        show_msgbox("ELF Loader","Failed to install loader hook");
        free(reloc); //We failed so we should free the memory used
    }

    //Remember, don't free the reloc pointer if it was successful!!
    //We can safely end our execution without affecting the hook now
    return 0;
}