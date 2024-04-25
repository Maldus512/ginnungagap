MEMORY {
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
}

ENTRY(_debugger_entry_point)

SECTIONS {
    .text : {
        . = ORIGIN(FLASH);
        __boot2_start__ = .;
        KEEP (*(.boot2));
        __boot2_end__ = .;
        KEEP(*(.vector_table));
        *(.text*);
        . = ALIGN(4);
    } > FLASH

    ASSERT(__boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size")

    .rodata : {
        KEEP(*(.rodata*))
        . = ALIGN(4);
    } > FLASH

    .data : {
        __data_start__ = .;
        *(.data*)
        . = ALIGN(4);
    } > RAM AT> FLASH
    __data_init__ = LOADADDR(.data);
    __data_size__ = SIZEOF(.data);

    .bss (NOLOAD) : {
        __bss_start__ = .;
        *(.bss*)
        . = ALIGN(4);
    } > RAM
    __bss_size__ = SIZEOF(.bss);

    __stack_top = ORIGIN(RAM) + 8k;
}
