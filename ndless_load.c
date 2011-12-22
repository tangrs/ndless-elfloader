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
#include "ndlsbin.h"

#define FILE_READ_BLOCKS 1024
#define PRGMSIG "PRG"

int ndless_execute(FILE* fp, int *ret, int argc, char *argv[]) {
    printf("Loading file as ndless binary\n");

    //Ndless lacks a ftell() function so we can't tell the size of the file
    //We'll have to load it bit by bit

    void* ptr = malloc(FILE_READ_BLOCKS);
    if (!ptr) goto malloc_error;
    size_t size = FILE_READ_BLOCKS;

    while (fread(ptr, 1, FILE_READ_BLOCKS, fp) == FILE_READ_BLOCKS) {
        size += FILE_READ_BLOCKS;
        ptr = realloc(ptr, size);
        if (!ptr) goto malloc_error;
    }

    clear_cache();
    *ret = ((int (*)(int argc, char *argv[]))(ptr + sizeof(PRGMSIG)))(argc, argv);
    free(ptr);

    return 0;

    malloc_error:
    printf("Malloc returned NULL\n");
    return -1;
}