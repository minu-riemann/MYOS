#include "../drivers/serial/serial.h"
#include "../drivers/keyboard/keyboard.h"

#include "vga.h"

#include "../kernel/panic/panic.h"

#include "../arch/x86/cpu/gdt.h"
#include "../arch/x86/interrupt/idt.h"
#include "../arch/x86/interrupt/irq.h"
#include "../arch/x86/interrupt/pit.h"

static void trigger_pf_null(void) {
    volatile uint32_t *p = (uint32_t*)0x0;
    *p = 0xDEADBEEF;
}

void kernel_main(void) {
    vga_clear();
    vga_puts_at(2, 2, "MyOS: IRQ/PIT/KBD OK");
    vga_puts_at(4, 2, "Type on keyboard - see serial logs");
    serial_init();
    serial_write("[INFO] kernel_main entered\n");

    serial_write("[INFO] loading GDT...\n");
    gdt_init();
    serial_write("[INFO] GDT loaded\n");

    serial_write("[INFO] loading IDT...\n");
    idt_init();
    serial_write("[INFO] IDT loaded\n");

    serial_write("[INFO] init IRQ/PIC...\n");
    irq_init();

    serial_write("[INFO] init keyboard...\n");
    keyboard_init();

    serial_write("[INFO] init PIT...\n");
    pit_init(100); // 100Hz

    serial_write("[INFO] sti\n");
    __asm__ volatile ("sti");

    trigger_pf_null();

    for (;;)
        __asm__ volatile ("hlt");
}