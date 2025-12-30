#include "gdt.h"

typedef struct __attribute__((packed)) {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t gran;
    uint8_t base_high;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} gdt_ptr_t;

static gdt_entry_t gdt[3];
static gdt_ptr_t gp;

extern void gdt_flush(uint32_t gdt_ptr_addr);

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (uint16_t)(base & 0xFFFF);
    gdt[num].base_mid  = (uint8_t)((base >> 16) & 0xFF);
    gdt[num].base_high = (uint8_t)((base >> 24) & 0xFF);

    gdt[num].limit_low = (uint16_t)(limit & 0xFFFF);
    gdt[num].gran = (uint8_t)((limit >> 16) & 0x0F);
    
    gdt[num].gran |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gp.base = (uint32_t)&gdt;

    // 0: null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment

    // 1: kernel code segment: base = 0, limit = 4GB, access = 0x9A, gran = 0xCF
    // access 0x9A = present, ring0, code segment, executable, readable
    // gran   0xCF = 4K granularity, 32-bit protected mode, limit high bits=0xF
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment

    // 2: kernel data segment: base=0, limit=4GB, access=0x92, gran=0xCF
    // access 0x92 = present, ring0, data segment, writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

    gdt_flush((uint32_t)&gp);

}