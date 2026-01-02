#include "cr.h"

uint32_t read_cr0(void) {
    uint32_t val;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(val));
    return val;
}

void write_cr0(uint32_t value) {
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(value));
}

uint32_t read_cr2(void) {
    uint32_t val;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(val));
    return val;
}

uint32_t read_cr3(void) {
    uint32_t val;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(val));
    return val;
}

void write_cr3(uint32_t value) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(value));
}

uint32_t read_cr4(void) {
    uint32_t val;
    __asm__ __volatile__("mov %%cr4, %0" : "=r"(val));
    return val;
}

void write_cr4(uint32_t value) {
    __asm__ __volatile__("mov %0, %%cr4" : : "r"(value));
}