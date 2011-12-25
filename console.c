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
#include <os.h>
#include "console.h"
#include "console_font.h"

#define MAX_CONSOLE_STRLEN 256

#define CONSOLE_WIDTH  (SCREEN_WIDTH/FONT_WIDTH)
#define CONSOLE_HEIGHT (SCREEN_HEIGHT/FONT_HEIGHT)
#define GET_BIT(x,y)   ( ((y)>>(x)) & 0x1 )

static char screenbuffer[SCREEN_BYTES_SIZE];
static int x = 0, y = 0;

static inline void console_putc(char c);
static inline void console_newline();
static inline void console_shiftup();
static inline void console_write(uint32_t addr, char c);
static inline void console_flush();

void console_printf(const char *format, ...) {
    va_list arglist;
    char buffer[MAX_CONSOLE_STRLEN];

    va_start(arglist, format);
    vsprintf(buffer,format,arglist); //Argh, unsafe!
    va_end(arglist);

    char c;
    int i = 0;
    while (c = buffer[i]) {
        console_putc(c);
        i++;
    }
    console_flush();
}

static inline void console_putc(char c) {
    int i, k;
    uint32_t addr = (y*FONT_HEIGHT*SCREEN_WIDTH)+(x*FONT_WIDTH);
    char (*font)[FONT_HEIGHT] = consoleFont;

    if (c == '\n') {
        console_newline();
        return;
    }

    for (k=0; k<FONT_HEIGHT; k++) {
        for (i=FONT_WIDTH-1; i>=0; i--) {
            if (GET_BIT(i, font[c][k] )) {
                console_write(addr+(FONT_WIDTH-1-i), 0x0);
            }else{
                console_write(addr+(FONT_WIDTH-1-i), 0xf);
            }
        }
        addr+=SCREEN_WIDTH;
    }

    x++;
    if (x >= CONSOLE_WIDTH) {
        console_newline();
    }

}
static inline void console_newline() {
    y++;
    x = 0;
    if (y >= CONSOLE_HEIGHT) {
        console_shiftup();
    }
}
static inline void console_shiftup() {
    memmove(screenbuffer, screenbuffer+(SCREEN_WIDTH*FONT_HEIGHT/2), SCREEN_BYTES_SIZE-(SCREEN_WIDTH*FONT_HEIGHT/2));
    memset(screenbuffer+SCREEN_BYTES_SIZE-(SCREEN_WIDTH*FONT_HEIGHT/2), 0, (SCREEN_WIDTH*FONT_HEIGHT/2));
    y--;
}
static inline void console_write(uint32_t addr, char c) {
    char tmp = screenbuffer[addr/2];
    c &= 0xf;

    if (addr % 2 == 0) {
        c  <<= 0x4;
        tmp &= 0xf;
        tmp |= c;
    }else{
        tmp &= 0xf0;
        tmp |= c;
    }

    screenbuffer[addr/2] = tmp;
}

static inline void console_flush() {
    void* screenaddr = SCREEN_BASE_ADDRESS;
    memcpy(screenaddr, screenbuffer, SCREEN_BYTES_SIZE);
    clear_cache();
}