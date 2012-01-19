#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE* fp;
    long int length, i;
    //Size needed to fit representation of a long int + size needed to longest fit string + null
    char writeBuffer[(sizeof(long int)*2) + 16], *str;

    if (argc < 2) {
        printf( "Generates benchmarking program for ELF loader\n\n"
                "Usage: %s symbols [file]\n", argv[0]);
        return 0;
    }

    length = atol(argv[1]);
    if (length < 1) {
        printf("Invalid number\n");
        return 0;
    }
    if (argc == 3) {
        fp = fopen(argv[2], "w");
    }else{
        fp = fopen("benchmark.c", "w");
    }

    if (!fp) {
        printf("Unable to open file for writing\n");
        return -1;
    }
    for (i = 0; i < length; i++) {
        sprintf(writeBuffer, "void foo%lx() {}\n", i);
        fwrite(writeBuffer, 1, strlen(writeBuffer), fp);
    }

    str =   "typedef void (foofunc)();\n"
            "static foofunc *ptrs[] = {\n";
    fwrite(str , 1, strlen(str), fp);

    for (i = 0; i < length; i++) {
        sprintf(writeBuffer, "    foo%lx,\n", i);
        fwrite(writeBuffer, 1, strlen(writeBuffer), fp);
    }
    str =   "};\n"
            "int i = 0; //Hack to force gcc to generate a .got section\n"
            "int _start() { i++; return 0; }\n";
    fwrite(str, 1, strlen(str), fp);

    fclose(fp);
    return 0;
}