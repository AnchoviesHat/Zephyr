OUTPUT_FORMAT("binary")

ENTRY(stub32)

SECTIONS {
    . = 0x7E00;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(.bss)
    }
}

