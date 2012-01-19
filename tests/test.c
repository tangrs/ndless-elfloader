#include <os.h>
#include "../elf/elf.h"
#include "../misc/console.h"
#include "../misc/debug.h"

#define MIN(x,y)            ((x) < (y) ? (x) : (y))


int dirname(char* filename, char* buff, size_t maxlen) {
    char *current = filename;
    // We calculate dirname for the path
    current += strlen(filename) - 1; //Ignore a trailing slash

    while (*current != '/') {
        if (current > filename) current--; else break;
    }

    if (current > filename) {
        strncpy(buff, filename, (size_t)MIN(maxlen, current-filename));
        buff[MIN(maxlen, current-filename)] = 0;
        return 0;
    } else {
        return -1;
    }
}


static char *exec_name = NULL;
char* get_full_fname(char* file) {
    static char buffer[128];
    char tmpbuffer[128];

    if (!exec_name) return 0;
    if (dirname(exec_name, tmpbuffer, 128) != 0) return 0;
    sprintf(buffer, "%s/%s", tmpbuffer, file);
    return buffer;
}

int main(int argc, char *argv[]) {
    exec_name = argv[0];
    FILE *fp;
    int ret;
    char *fname;
    volatile unsigned *timer_val = (unsigned*)0x900D0000;
	volatile unsigned *timer_divider = (unsigned*)0x900D0004;
	volatile unsigned *timer_control = (unsigned*)0x900D0008;

    fname = get_full_fname("benchmark.elf.tns");
    assert(fname);
    if (fp = fopen(fname, "rb")) {
    unsigned timer_original_val = *timer_val;
    unsigned timer_original_divider = *timer_divider;
    unsigned timer_original_control = *timer_control;
    *timer_control = (*timer_control & 0xFFFFFFE0) | 0xF; //Indefinitely increasing timer
    *timer_divider = 31; //Ticks every 1ms.
    *timer_val = 0;
    console_printf("Testing speed\n");

    elf_execute(fp, &ret, 1, (char*[]){ fname, NULL });

    console_printf("Time taken to load was %dms\nSpeed test finished\n",*timer_val);
    fclose(fp);

    *timer_control = timer_original_control;
    *timer_divider = timer_original_divider;
    *timer_val = timer_original_val;
    }

    fname = get_full_fname("unittest.elf.tns");
    assert(fname);
    if (fp = fopen(fname, "rb")) {
    //First, test speed
    console_printf("Running unittest\n");
    elf_execute(fp, &ret, 2, (char*[]){ fname, NULL });

    console_printf("Unit test returned %d", ret);
    fclose(fp);
    }
    sleep(10000);
    return 0;
}