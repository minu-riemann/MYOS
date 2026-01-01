#include "../drivers/serial/serial.h"
#include "../drivers/keyboard/keyboard.h"

#include "vga.h"

#include "../kernel/panic/panic.h"

#include "../arch/x86/cpu/gdt.h"
#include "../arch/x86/interrupt/idt.h"
#include "../arch/x86/interrupt/irq.h"
#include "../arch/x86/interrupt/pit.h"
#include "memory/multiboot.h"

extern uint32_t __kernel_end;

void kernel_main(uint32_t magic, uint32_t mb_addr) {
    vga_clear();
    serial_init();
    serial_write("[INFO] kernel_main entered\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        serial_write("[MB] bad magic: ");
        char buf[11]; u32_to_hex(magic, buf); serial_write(buf);
        serial_write("\n");
        panic("Not booted by a Multiboot-compliant bootloader.");
    }

    serial_write("[MB] magic OK\n");
    serial_write("[MB] mb_addr="); 
    char buf[11]; u32_to_hex(mb_addr, buf); serial_write(buf);
    serial_write("\n");

    serial_write("[MEM] __kernel_end=");
    u32_to_hex((uint32_t)&__kernel_end, buf); serial_write(buf);
    serial_write("\n");

    multiboot_dump_memory_map(mb_addr);

    uint32_t base=0, end=0;
    if (multiboot_find_largest_usable(mb_addr, &base, &end)) {
        serial_write("[MEM] largest usable: base=");
        u32_to_hex(base, buf); serial_write(buf);
        serial_write(" end=");
        u32_to_hex(end, buf); serial_write(buf);
        serial_write("\n");
    } else {
        serial_write("[MEM] no usable region found\n");
    }

    // 이후: heap_init(base..end, &__kernel_end)로 넘어갈 준비 완료
    while (1) { __asm__ __volatile__("hlt"); }
}