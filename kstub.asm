bits 32

extern kmain

section .text

global stub32
stub32:
    mov edx, 0xB8000 + 156
    mov al, "b"
    mov ah, 0xF
    mov [edx], ax

    mov esp, stack_top

    call set_up_page_tables

    call enable_paging

    lgdt [gdt64.pointer]
    jmp gdt64.code:stub64

set_up_page_tables:
    ; map first p4 entry to p3 table
    mov eax, p3_table
    or eax, 0b11       ; present + writable
    mov [p4_table], eax

    ; map first p3 entry to p2 table
    mov eax, p2_table
    or eax, 0b11       ; present + writable
    mov [p3_table], eax

    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000   ; 2 MB
    mul ecx
    or eax, 0b10000011  ; present + writable + huge
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table

    ret

enable_paging:
    ; Load p4
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set long mode bit?
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

bits 64
stub64:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov edx, 0xB8000 + 158
    mov al, "c"
    mov ah, 0xF
    mov [edx], ax

    call kmain
    call die

die:
    cli
    hlt
    jmp die

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .bss

align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

stack_bottom:
    resb 8192
stack_top:
