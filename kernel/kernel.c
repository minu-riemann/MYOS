#include "../drivers/serial/serial.h"
#include "../drivers/keyboard/keyboard.h"

#include "panic/panic.h"
#include "memory/multiboot.h"
#include "memory/heap.h"

#include "../arch/x86/cpu/gdt.h"
#include "../arch/x86/interrupt/idt.h"
#include "../arch/x86/interrupt/irq.h"
#include "../arch/x86/interrupt/pit.h"

#include "console/kprintf.h"

extern uint32_t __kernel_end;

// ---------------------
// Helpers
// ---------------------
static inline uint32_t align_up(uint32_t v, uint32_t a) {
    uint32_t m = a - 1;
    return (v + m) & ~m;
}

// (선택) 페이지 폴트 테스트
static void trigger_pf_null_write(void) {
    volatile uint32_t* p = (uint32_t*)0x0;
    *p = 0xDEADBEEF;
}

static void trigger_pf_null_read(void) {
    volatile uint32_t* p = (uint32_t*)0x0;
    volatile uint32_t v = *p;
    (void)v;
}

// ---------------------
// kernel_main
// ---------------------
void kernel_main(uint32_t magic, uint32_t mb_addr) {
    // 화면/시리얼 준비
    kprintf_clear_console();
    serial_init();
    kprintf("[INFO] kernel_main entered\n");

    // 기본 플랫폼 초기화
    kprintf("[INFO] loading GDT...\n");
    gdt_init();
    kprintf("[INFO] GDT loaded\n");

    kprintf("[INFO] loading IDT...\n");
    idt_init();
    kprintf("[INFO] IDT loaded\n");

    kprintf("[INFO] init IRQ/PIC...\n");
    irq_init();

    kprintf("[INFO] init keyboard...\n");
    keyboard_init();

    kprintf("[INFO] init PIT...\n");
    pit_init(100); // 100Hz tick
    time_set_hz(100);

    // 인터럽트 활성화 (키보드 입력을 받기 위해 필요)
    kprintf("[INFO] Enabling interrupts (sti)\n");
    __asm__ __volatile__("sti");

    // -------------------------
    // STEP2: Multiboot mmap
    // -------------------------
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kprintf("[MB] bad magic=0x%x\n", magic);
        panic("Not booted by a Multiboot-compliant bootloader.");
    }

    kprintf("[MB] magic OK\n");
    kprintf("[MB] mb_addr=0x%x\n", mb_addr);
    kprintf("[MEM] __kernel_end=0x%x\n", (uint32_t)&__kernel_end);

    multiboot_dump_memory_map(mb_addr);

    uint32_t base = 0, end = 0;
    int ok = multiboot_find_largest_usable(mb_addr, &base, &end);

    kprintf("[MEM] usable_base=0x%x\n", base);
    kprintf("[MEM] usable_end=0x%x\n", end);

    if (!ok || end == 0 || end <= base) {
        panic("No valid usable memory region found");
    }

    // -------------------------
    // STEP3: heap/kmalloc
    // -------------------------
    uint32_t heap_start = align_up((uint32_t)&__kernel_end, 16);
    if (heap_start < base) heap_start = base;

    kprintf("[HEAP] heap_start=0x%x\n", heap_start);
    kprintf("[HEAP] heap_end=0x%x\n", end);

    if (end <= heap_start) {
        panic("Heap range invalid BEFORE heap_init");
    }

    heap_init(heap_start, end);

    void* a = kmalloc(16);
    void* b = kmalloc(256);
    void* c = kmalloc_aligned(64, 64);

    kprintf("[HEAP] test alloc\n");
    kprintf("  a=0x%x\n", (uint32_t)a);
    kprintf("  b=0x%x\n", (uint32_t)b);
    kprintf("  c=0x%x\n", (uint32_t)c);
    kprintf("  used=%u\n", heap_used());
    kprintf("  free=%u\n", heap_free());

    // -------------------------
    // STEP4: kprintf 테스트
    // -------------------------
    kprintf("kprintf test: dec=%d hex=%x str=%s %%\n", -123, 0xBEEF, "OK");

    // 부팅 화면 메시지 (일반 정보)
    kprintf_puts_at(2, 2, "MYOS Phase1 Test Kernel");
    kprintf_puts_at(4, 2, "See console for logs.");
    kprintf_puts_at(6, 2, "Type keys to test keyboard!");

    kprintf("[INFO] Phase1 platform up. Entering idle loop.\n");
    kprintf("[INFO] Keyboard ready - type keys to test input.\n");
    kprintf("[INFO] (Optional) enable PF test by uncommenting below.\n");

    // -------------------------
    // (선택) PF 테스트: 하나만 켜세요
    // -------------------------
    // trigger_pf_null_write();
    // trigger_pf_null_read();

    // -------------------------
    // idle loop (키보드 입력은 IRQ로 처리됨)
    // -------------------------
    while (1) {
        __asm__ __volatile__("hlt");
    }
}