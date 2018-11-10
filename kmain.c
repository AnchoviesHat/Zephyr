#include "common.h"
#include "libvga/libvga.h"

void outb(u16 port, u8 value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port) {
    u8 rv;
    asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
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

void halt() {
    vga_draw_sidewall("ZEPHYR HALTED", Red);
    for (;;) {
        asm ("hlt");
    }
}

void kmain() {
    vga_clear_screen();
    vga_draw_boot_screen();
    vga_printf("Booting Zephyr...\n");

    mem_map_t *mem_map = (mem_map_t *)0x7F00;
    u16 mem_map_size = *((u16 *)0x7E00);

    u64 total_usable_memory = 0;

    for (int i = 0; i < mem_map_size; i++) {
        if (mem_map[i].type == 1) {
            total_usable_memory += mem_map[i].size;
        }
    }

    vga_printf("Found %dM of usable memory...\n", total_usable_memory / 1024 / 1024);

    while(1){}

    halt();
}
