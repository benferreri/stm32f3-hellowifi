/* label for program's entry point */
ENTRY(reset_handler)

/* define the end of RAM and limit of stack memory */
/* (12 KB SRAM -> 0x4000) */
/* (RAM starts at address 0x20000000) */
_estack = 0x20003000;

/* min size for stack and dynamic memory */
/* (1 KB) */
_Min_Leftover_RAM = 0x400;

MEMORY
{
    FLASH ( rx )        : ORIGIN = 0x08000000, LENGTH = 64K
    RAM ( rxw )         : ORIGIN = 0x20000000, LENGTH = 12K
}

SECTIONS
{
    /* the vector table goes at the start of flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP (*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* the 'text' section contains the main program code */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } >FLASH

    /* the 'rodata' section contains read-only data, constants,
     * strings, information that won't change */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* the 'data' section is space set aside in RAM for things
     * like variables, which can change */
    _sidata = .;
    .data : AT(_sidata)
    {
        . = ALIGN(4);
        /* mark start/end locations for the 'data' section */
        _sdata = .;
        *(.data)
        *(.data*)
        _edata = .;
        . = ALIGN(4);
    } >RAM
    
    /* the 'bss' section is similar to the 'data' section, but
     * its space is initialized to all 0s at the start of the
     * program */
    .bss :
    {
        . = ALIGN(4);
        /* also mark the start/end of the BSS section */
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* space set aside for the application's heap/stack */
    .dynamic_allocations :
    {
        . = ALIGN(4);
        _ssystem_ram = .;
        . = . + _Min_Leftover_RAM;
        . = ALIGN(4);
        _esystem_ram = .;
    } >RAM
}
