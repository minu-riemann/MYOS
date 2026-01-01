ENTRY(start)

SECTIONS
{
  . = 1M;

  __kernel_start = .;

  .text :
  {
    *(.multiboot)
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