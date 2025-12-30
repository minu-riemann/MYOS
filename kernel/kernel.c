#include "../drivers/serial/serial.h"
#include "vga.h"
#include "../kernel/panic/panic.h"

#include "../arch/x86/cpu/gdt.h"
#include "../arch/x86/interrupt/idt.h"

static void trigger_ud2(void) {
    __asm__ __volatile__("ud2");
}

void kernel_main(void) {
    vga_clear();
    serial_init();
    serial_write("[INFO] kernel_main entered\n");

    serial_write("[INFO] loading GDT...\n");
    gdt_init();
    serial_write("[INFO] GDT loaded\n");

    serial_write("[INFO] loading IDT...\n");
    idt_init();
    serial_write("[INFO] IDT loaded\n");

    serial_write("[TEST] trigger #UD (ud2)\n");
    trigger_ud2();

    panic("Returned after exception (unexpected).");
}