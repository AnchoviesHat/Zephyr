#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

u8 *mem = 0;
u16 *vga_mem = (u16 *)0xB8000;
u16 cur = 0;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void outb(u16 port, u8 value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port) {
    u8 rv;
    asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

void set_cursor(u16 pos) {
    outb(0x3D4, 0xF);
    outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0xE);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

u16 get_cursor() {
    outb(0x3D4, 0xF);
    u8 pos_lo = inb(0x3D5);
    outb(0x3D4, 0xE);
    u8 pos_hi = inb(0x3D5);

    return pos_hi << 8 | pos_lo;
}

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_mem[i] = (0xF << 8) | ' ';
    }
}

void draw_boot_screen() {
    int i;

    clear_screen();

    i = 0;
    char *prompt = "  >";
    while (prompt[i] != '\0') {
        vga_mem[i] = (0xA << 8) | prompt[i];
        i++;
    }
    set_cursor(i + 1);

    char *sidewall = "ZEPHYR ONLINE";
    i = 0;
    while (sidewall[i] != '\0') {
        vga_mem[VGA_WIDTH * i] = (0xA << 8) | sidewall[i];
        i++;
    }
}

void puts(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        vga_mem[cur + i] = (0x7 << 8) | str[i];
        i++;
    }

    cur += i;
    set_cursor(cur);
}

char base_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void put_num(u64 num, u8 base) {
    char tmp[64] = {0};

    if (num == 0) {
        vga_mem[cur] = (0x7 << 8) | '0';
        set_cursor(cur + 1);
        return;
    }

    int i = 0;
    while (num != 0) {
        tmp[i++] = base_chars[num % base];
        num /= base;
    }

    i--;
    for (; i >= 0; i--) {
        vga_mem[cur++] = (0x7 << 8) | tmp[i];
    }
    set_cursor(cur);
}

// Mask out the 8259A so we can use the APIC instead
void disable_pic() {
    outb(0x21, 0xFF);
    outb(0xa1, 0xFF);
}

typedef struct __attribute__((packed)) {
    u32 base;
    u32 base_hi;
    u32 size;
    u32 size_hi;
    u32 type;
} mem_map_t;

void kmain() {
    cur = get_cursor();
    puts("Booting Zephyr...");

    mem_map_t *mem_map = (mem_map_t *)0x7F00;
    u16 mem_map_size = *((u16 *)0x7E00);

    u64 total_usable_memory = 0;

    for (int i = 0; i < mem_map_size; i++) {
        if (mem_map[i].type == 1) {
            total_usable_memory += mem_map[i].size;
        }
    }

    cur = ((cur / VGA_WIDTH) + 1) * VGA_WIDTH;
    puts("Found ");
    put_num(total_usable_memory / 1024 / 1024, 10);
    puts("M of usable memory...");

    //draw_boot_screen();

    for (;;) {
        asm ("hlt");
    }
}
