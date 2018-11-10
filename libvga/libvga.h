#pragma once
#include "../common.h"

typedef enum Color {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    Pink = 13,
    Yellow = 14,
    White = 15
} Color;

void vga_clear_screen();
void vga_draw_boot_screen();
void vga_puts(char *str);
void vga_put_num(u64 num, u8 base);
void vga_draw_sidewall(char *str, Color color);
void vga_printf(char *fmt, ...);