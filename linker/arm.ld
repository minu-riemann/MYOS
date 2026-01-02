ENTRY(_start)

SECTIONS
{
  . = 0x8000;

  __kernel_start = .;

  .text :
  {
    *(.text*)
  }

  .rodata : { *(.rodata*) }
  .data   : { *(.data*) }

  .bss :
  {
    *(.bss*)
    *(COMMON)
  }

  __kernel_end = .;
}