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
#include "console.h"
#include "debug.h"
#include <os.h>

enum {
    BIN_ELF,
    BIN_NDLS,
    BIN_UNKNOWN = -1
};

static int file_type(FILE* fp) {
    char magic[4];
    fread(magic, 1, 4, fp);
    if (memcmp("\x7f""ELF",magic,4) == 0) return BIN_ELF;
    if (memcmp("PRG",magic,3) == 0)  return BIN_NDLS;
    return BIN_UNKNOWN;
}

int main() {
    char *filename = "/documents/test/parseelf.elf.tns";
    FILE* fp = fopen(filename,"rb");
    int ret;
    char *argv[] = {filename, 0};

    elf_execute(fp, &ret, 1, argv);

    fclose(fp);
    return 0;
}