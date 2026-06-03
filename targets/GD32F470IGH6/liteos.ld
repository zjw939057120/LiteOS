/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * Linker Script for GigaDevice GD32F470IGH6 Microcontroller
 *
 * GD32F470IGH6 Memory Configuration:
 *   - FLASH:  512KB (0x08000000 - 0x08080000)
 *   - RAM:    192KB (0x20000000 - 0x20030000)
 *   - No CCMRAM
 *
 * This script defines how the linker arranges sections in memory.
 */

/* Program entry point: Reset Handler */
ENTRY(Reset_Handler)

/* Stack configuration */
_sstack = 0x20000000;           /* Stack start address (beginning of RAM) */
_estack = 0x20030000;           /* Stack end address (end of RAM: 0x20000000 + 192KB) */
__los_stack_size__ = 0x400;     /* LiteOS kernel stack size (1KB) */

/* Memory region definitions */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K   /* Program Flash (read/execute) */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K   /* SRAM (read/write) */
}

/* Section definitions */
SECTIONS
{
    /* Interrupt vector table - must be placed at the beginning of FLASH */
    .isr_vector :
    {
        . = ALIGN(4);            /* Align to 4-byte boundary */
        KEEP(*(.isr_vector))     /* Keep interrupt vectors (do not discard) */
        . = ALIGN(4);
    } >FLASH

    /* Code section - program instructions */
    .text :
    {
        . = ALIGN(4);
        __text_start = .;        /* Start of code section */
        *(.text)                 /* Regular code */
        *(.text*)                /* Code subsections */
        *(.glue_7)               /* ARM/Thumb interworking glue code */
        *(.glue_7t)              /* Thumb/ARM interworking glue code */
        *(.eh_frame)             /* Exception handling frames */

        KEEP (*(.init))          /* Initialization code */
        KEEP (*(.fini))          /* Finalization code */

        . = ALIGN(4);
        __text_end = .;          /* End of code section */
    } >FLASH

    /* Read-only data section - constants, strings */
    .rodata :
    {
        . = ALIGN(4);
        __rodata_start = .;      /* Start of read-only data */
        *(.rodata)               /* Read-only data */
        *(.rodata*)              /* Read-only data subsections */
        . = ALIGN(4);
        __rodata_end = .;        /* End of read-only data */
    } >FLASH

    /* Pre-initialization array - called before main() */
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    } >FLASH

    /* Initialization array - C++ constructors */
    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    } >FLASH

    /* Finalization array - C++ destructors */
    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } >FLASH

    /* LiteOS vector table in RAM (runtime) */
    _si_liteos_vector_data = LOADADDR(.vector_ram);  /* Load address in FLASH */
    .vector_ram :
    {
        . = ORIGIN(RAM);                          /* Start at beginning of RAM */
        _s_liteos_vector = .;                     /* Start of LiteOS vector */
        *(.data.vector)                           /* LiteOS vector data */
        _e_liteos_vector = .;                     /* End of LiteOS vector */
    } > RAM AT> FLASH                             /* Link to RAM, load from FLASH */

    /* Initialized data section - data with initial values */
    _sidata = LOADADDR(.data);                    /* Load address in FLASH */
    .data ALIGN(0x1000):                          /* Align to 4KB boundary */
    {
        __ram_data_start = .;                     /* Start of initialized data */
        . = ALIGN(4);
        *(.data)                                  /* Initialized data */
        *(.data*)                                 /* Initialized data subsections */
        KEEP(*( SORT (.liteos.table.*)));         /* LiteOS kernel tables */
        . = ALIGN(4);
        __ram_data_end = .;                       /* End of initialized data */
    } >RAM AT> FLASH                              /* Link to RAM, load from FLASH */

    /* Uninitialized data section - zero-initialized */
    . = ALIGN(4);
    .bss :
    {
        __bss_start = .;                          /* Start of BSS section */
        *(.bss .bss*)                             /* Uninitialized data */
        *(COMMON)                                 /* Common symbols */
        . = ALIGN(4);
        __bss_end = .;                            /* End of BSS section */
    } >RAM

    /* Heap configuration for LiteOS */
    . = ALIGN(8);
    __los_heap_addr_start__ = .;                  /* Heap start address */
    __los_heap_addr_end__ = ORIGIN(RAM) + LENGTH(RAM) - __los_stack_size__ - 1;  /* Heap end address */

    __fast_end = .;                               /* End of fast data section */

    /* Discard unused standard library sections */
    /DISCARD/ :
    {
        libc.a ( * )                              /* C standard library */
        libm.a ( * )                              /* Math library */
        libgcc.a ( * )                            /* GCC support library */
    }

    /* ARM attributes section */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}