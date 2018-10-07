#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t u64;

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

void draw_boot_screen() {
	int i;

	for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		vga_mem[i] = (0xF << 8) | ' ';
	}

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
	set_cursor(cur + i);
}

// Mask out the 8259A so we can use the APIC instead
void disable_pic() {
	outb(0x21, 0xFF);
	outb(0xa1, 0xFF);
}

void kmain() {
    cur = get_cursor();
	puts("Booting Zephyr...");

	//draw_boot_screen();

	for (;;) {
		asm ("hlt");
	}
}
