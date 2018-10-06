OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)

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

