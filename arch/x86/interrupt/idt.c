#include "idt.h"

typedef struct __attribute__((packed)) {
    uint16_t base_low;
    uint16_t sel; // kernel code segment selector
    uint8_t allways0;
    uint8_t flags; // type and attributes
    uint16_t base_high;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (uint16_t)(base & 0xFFFF);
    idt[num].base_high = ((uint16_t) (base >> 16) & 0xFFFF);
    idt[num].sel = sel;
    idt[num].allways0 = 0;
    idt[num].flags = flags;
}

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);         
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void isr32(void);
extern void isr33(void);
extern void isr34(void);
extern void isr35(void);
extern void isr36(void);
extern void isr37(void);
extern void isr38(void);
extern void isr39(void);
extern void isr40(void);
extern void isr41(void);
extern void isr42(void);
extern void isr43(void);
extern void isr44(void);
extern void isr45(void);
extern void isr46(void);
extern void isr47(void);

void idt_init(void) {
    idt_ptr.limit = (uint16_t)(sizeof(idt_entry_t)*256 -1);
    idt_ptr.base = (uint32_t)&idt;

    // zero-init
    for (int i = 0; i < 256; i++) {
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;
        idt[i].allways0 = 0;
        idt[i].flags = 0;
    }

    // 0x08: kernel code segment selector (GRUB 환경에서 일반적으로 0x08)
    // 0x8E: present=1, DPL=0, type=0xE (32-bit interrupt gate)
    const uint16_t KERNEL_CS = 0x08;
    const uint8_t FLAGS_INTGATE = 0x8E; 

    idt_set_gate(0, (uint32_t)isr0, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(1, (uint32_t)isr1, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(2, (uint32_t)isr2, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(3, (uint32_t)isr3, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(4, (uint32_t)isr4, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(5, (uint32_t)isr5, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(6, (uint32_t)isr6, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(7, (uint32_t)isr7, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(8, (uint32_t)isr8, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(9, (uint32_t)isr9, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(10, (uint32_t)isr10, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(11, (uint32_t)isr11, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(12, (uint32_t)isr12, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(13, (uint32_t)isr13, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(14, (uint32_t)isr14, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(15, (uint32_t)isr15, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(16, (uint32_t)isr16, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(17, (uint32_t)isr17, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(18, (uint32_t)isr18, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(19, (uint32_t)isr19, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(20, (uint32_t)isr20, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(21, (uint32_t)isr21, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(22, (uint32_t)isr22, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(23, (uint32_t)isr23, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(24, (uint32_t)isr24, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(25, (uint32_t)isr25, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(26, (uint32_t)isr26, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(27, (uint32_t)isr27, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(28, (uint32_t)isr28, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(29, (uint32_t)isr29, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(30, (uint32_t)isr30, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(31, (uint32_t)isr31, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(32, (uint32_t)isr32, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(33, (uint32_t)isr33, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(34, (uint32_t)isr34, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(35, (uint32_t)isr35, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(36, (uint32_t)isr36, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(37, (uint32_t)isr37, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(38, (uint32_t)isr38, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(39, (uint32_t)isr39, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(40, (uint32_t)isr40, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(41, (uint32_t)isr41, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(42, (uint32_t)isr42, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(43, (uint32_t)isr43, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(44, (uint32_t)isr44, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(45, (uint32_t)isr45, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(46, (uint32_t)isr46, KERNEL_CS, FLAGS_INTGATE);
    idt_set_gate(47, (uint32_t)isr47, KERNEL_CS, FLAGS_INTGATE);


    // Load the IDT
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_ptr));

}

