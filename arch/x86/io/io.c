#include "arch/io.h"
#include "ports.h"

// ports.h에 없는 함수들 추가
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t arch_io_read8(uint32_t addr) {
    return inb((uint16_t)addr);
}

uint16_t arch_io_read16(uint32_t addr) {
    return inw((uint16_t)addr);
}

uint32_t arch_io_read32(uint32_t addr) {
    return inl((uint16_t)addr);
}

void arch_io_write8(uint32_t addr, uint8_t value) {
    outb((uint16_t)addr, value);
}

void arch_io_write16(uint32_t addr, uint16_t value) {
    outw((uint16_t)addr, value);
}

void arch_io_write32(uint32_t addr, uint32_t value) {
    outl((uint16_t)addr, value);
}

void arch_io_wait(void) {
    io_wait();
}