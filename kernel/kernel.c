#include "../drivers/serial/serial.h"
#include "vga.h"

void kernel_main(void) {
    vga_clear();
    vga_puts_at(10, 20, "Hello, World!");
    serial_init();
    serial_write("[INFO] kernel_main entered\n");

    vga_puts_at(4, 2, "Serial should print to host console.");
    serial_write("[INFO] VGA online, serial online\n");
   
    for(;;){
        __asm__ __volatile__("hlt");
    }
}