#include "arch/io.h"

uint8_t arch_io_read8(uint32_t addr) {
    return *(volatile uint8_t*)addr;
}

uint16_t arch_io_read16(uint32_t addr) {
    return *(volatile uint16_t*)addr;
}

uint32_t arch_io_read32(uint32_t addr) {
    return *(volatile uint32_t*)addr;
}

void arch_io_write8(uint32_t addr, uint8_t value) {
    *(volatile uint8_t*)addr = value;
}

void arch_io_write16(uint32_t addr, uint16_t value) {
    *(volatile uint16_t*)addr = value;
}

void arch_io_write32(uint32_t addr, uint32_t value) {
    *(volatile uint32_t*)addr = value;
}

void arch_io_wait(void) {
    // ARM: 메모리 배리어 또는 nop
    __asm__ __volatile__("dsb sy");
}