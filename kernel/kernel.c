#include "../drivers/serial/serial.h"
#include "../drivers/keyboard/keyboard.h"

#include "vga.h"

#include "../arch/x86/cpu/gdt.h"
#include "../arch/x86/interrupt/idt.h"
#include "../arch/x86/interrupt/irq.h"
#include "../arch/x86/interrupt/pit.h"

#include "panic/panic.h"
#include "memory/multiboot.h"
#include "memory/heap.h"

extern uint32_t __kernel_end;

static inline uint32_t align_up(uint32_t v, uint32_t a) {
    uint32_t m = a - 1;
    return (v + m) & ~m;
}

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
        // 힙 시작은 커널 이미지 끝 이후
        uint32_t heap_start = align_up((uint32_t)&__kernel_end, 16);

        // 안전장치: usable base보다 낮으면 올려줌
        if (heap_start < base) heap_start = base;

        heap_init(heap_start, end);

        // 테스트: 여러 번 할당해보고 주소 증가 확인
        void* a = kmalloc(16);
        void* b = kmalloc(256);
        void* c = kmalloc_aligned(64, 64);

        char buf[11];
        serial_write("[HEAP] test alloc\n");
        serial_write("  a="); u32_to_hex((uint32_t)a, buf); serial_write(buf); serial_write("\n");
        serial_write("  b="); u32_to_hex((uint32_t)b, buf); serial_write(buf); serial_write("\n");
        serial_write("  c="); u32_to_hex((uint32_t)c, buf); serial_write(buf); serial_write("\n");
    } else {
        serial_write("[MEM] no usable region found\n");
    }

    // 이후: heap_init(base..end, &__kernel_end)로 넘어갈 준비 완료
    while (1) { __asm__ __volatile__("hlt"); }
}