set -e

nasm boot.asm -o boot.bin
nasm -felf64 kstub.asm -o kstub.o
clang -Os -ffreestanding -fno-pie -fno-pic -mno-sse -target x86_64-unknown-gnu-linux -c kmain.c
ld -T link.ld -o kernel.bin kstub.o kmain.o

dd if=/dev/zero of=zephyr.bin bs=512 count=70
dd if=boot.bin of=zephyr.bin conv=notrunc
dd if=kernel.bin of=zephyr.bin conv=notrunc bs=512 seek=1

rm kmain.o boot.bin kernel.bin kstub.o
