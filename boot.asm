org 0x7C00
bits 16

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    mov bp, sp

set_a20:
; Wait for port not busy, write D1 to 0x64
.1:
    in al, 0x64    
    test al, 0x2
    jnz set_a20.1
    
    mov al, 0xD1
    out 0x64, al

; Wait for port not busy, write DF to 0x64
.2:
    in al, 0x64
    test al, 0x2
    jnz set_a20.2
    
    mov al, 0xDF
    out 0x60, al

read_disk:
    mov ah, 0x2    ; Read sectors
    mov al, 20     ; Sectors to read
    mov ch, 0      ; cylinder idx
    mov dh, 0      ; head idx
    mov cl, 2      ; sector idx
    mov bx, 0x7E00 ; target addr
    int 0x13
    jc panic

    mov cx, boot_str_sz
    mov si, boot_str
    call print_str

to_protected:
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x8:flush

print_str:
    lodsb
    mov ah, 0xE
    int 0x10
    loop print_str
    ret

panic:
    cli
    hlt
    jmp panic

bits 32
flush:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp start32

start32:
    mov edx, 0xB8000 + 154
    mov al, "a"
    mov ah, 0xF
    mov [edx], ax

    jmp 0x7E00

panic32:
    cli
    hlt
    jmp panic32

boot_str: db "Bootloader loaded...", 0xa, 0xd
boot_str_sz: equ $-boot_str

gdt:
.null:
    dw 0
    dw 0
    db 0
    db 0
    db 0
    db 0
.code:
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0
.data:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0
.end:

gdtr:
    dw (gdt.end - gdt) - 1
    dd gdt

times 510-($-$$) db 0
dw 0xAA55
