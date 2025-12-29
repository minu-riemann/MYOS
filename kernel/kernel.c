#include "vga.h"

void kernel_main(void) {
    vga_clear();
    vga_puts_at(10, 20, "Hello, World!");
   
    for(;;){
        __asm__ __volatile__("hlt");
    }
}