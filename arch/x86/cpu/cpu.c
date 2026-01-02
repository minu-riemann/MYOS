#include "arch/cpu.h"
#include "gdt.h"
#include "cr.h"

void arch_cpu_init(void) {
    gdt_init();
}

uint32_t arch_read_fault_addr(void) {
    return read_cr2();
}

void arch_disable_interrupts(void) {
    __asm__ __volatile__("cli");
}

void arch_enable_interrupts(void) {
    __asm__ __volatile__("sti");
}

void arch_halt(void) {
    __asm__ __volatile__("cli; hlt");
}

void arch_idle(void) {
    __asm__ __volatile__("sti; hlt; cli");
}