#include "panic.h"
#include "../../drivers/serial/serial.h"
#include "../vga.h"

__attribute__((noreturn))
void panic(const char* msg) {
    // VGA
    vga_clear();
    vga_puts_at(10, 10, "KERNEL PANIC");
    if (msg) {
        vga_puts_at(12, 10, msg);
    }
    
    // Serial
    serial_write("[PANIC]");
    serial_write(msg ? msg : "(no message)");
    serial_write("\n");

    for (;;) {
        __asm__ __volatile__("cli; hlt");
    }
}