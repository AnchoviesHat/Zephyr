#include "../common.h"
#include "libvga.h"
#include "../string/string.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

u16 *vga_mem = (u16*)0xB8000;
u32 col = 2; // Which column we're writing in
u32 row = 0; // Which row we're writing in

struct ScreenChar {
    char c;
    Color color;
};

void vga_print_new_line() {
    ++row;
    col = 2;
    if (row >= VGA_HEIGHT) {
        for (int i = 1; i < VGA_HEIGHT; ++i) {
            for (int j = 0; j < VGA_WIDTH; ++j) {
                vga_mem[j + (i-1 * VGA_WIDTH)] = vga_mem[j + (i * VGA_WIDTH)];
            }
        }
        for (int i = 0; i < VGA_WIDTH; ++i) {
            vga_mem[(VGA_HEIGHT-1) + (i-1 * VGA_WIDTH)] = (Black << 8) | ' ';
        }

        vga_draw_boot_screen();
    }
}

void vga_clear_screen() {
    for (int i = 0; i < VGA_HEIGHT; ++i) {
        for (int j = 0; j < VGA_WIDTH; ++j) {
            vga_mem[j + (i * VGA_WIDTH)] = (Black << 8) | ' ';
        }
    }
    col = 2;
    row = 0;
}

static void vga_puts_color(char *str, Color color) {
    u64 len = strlen(str);
    for (u64 i = 0; i < len; ++i){
        if (col >= VGA_WIDTH) {
            vga_print_new_line();
            col += 4;
        }
        if (str[i] == '\n') {
            vga_print_new_line();
            col = 2;
            continue;
        }
        vga_mem[col + (row * VGA_WIDTH)] = (color << 8) | str[i];
        ++col;
    }
}

void vga_puts(char *str) {
    vga_puts_color(str, White);
}

void vga_putc(char c) {
    if (col >= VGA_WIDTH) {
        vga_print_new_line();
        col += 4;
    }
    if (c == '\n') {
        vga_print_new_line();
        col = 2;
        return;
    }
    vga_mem[col + (row * VGA_WIDTH)] = (White << 8) | c;
    ++col;
}

void vga_put_num(u64 num, u8 base) {
    char base_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char tmp[64] = {0};

    if (num == 0) {
        vga_puts("0");
        return;
    }

    int i = 0;
    while (num != 0) {
        tmp[i++] = base_chars[num % base];
        num /= base;
    }
    vga_puts(tmp);
}

void vga_draw_sidewall(char *str, Color color) {
    u32 len = strlen(str);
    col = 0;
    row = 0;
    for (u32 i = 0; i < len; ++i) {
        vga_mem[col + (row * VGA_WIDTH)] = (color << 8) | str[i];
        ++row;
    }
    col = 2;
    row = 0;
}

void vga_draw_boot_screen() {
    vga_draw_sidewall("ZEPHYR ONLINE", LightGreen);
}

void vga_printf(char *fmt, ...) {
	__builtin_va_list args;
	__builtin_va_start(args, fmt);

	for (char *c = fmt; *c != 0; c++) {
		if (*c != '%') {
			vga_putc(*c);
			continue;
		}

		c++;
		switch (*c) {
			case 'c': {
				char i = __builtin_va_arg(args, int);
				vga_putc(i);
			} break;
			case 's': {
				char *s = __builtin_va_arg(args, char *);
				vga_puts(s);
			} break;
			case 'd': {
				i32 i = __builtin_va_arg(args, int);
				vga_put_num(i, 10);
			} break;
			case 'x': {
				i32 i = __builtin_va_arg(args, int);
				vga_put_num(i, 16);
			} break;
			case 'b': {
				i32 i = __builtin_va_arg(args, int);
				vga_put_num(i, 2);
			} break;
			case 'p': {
				i32 i = __builtin_va_arg(args, int);
				vga_putc('0');
				vga_putc('x');
				vga_put_num(i, 16);
			} break;
		}
	}

	__builtin_va_end(args);
}