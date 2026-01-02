#include "arch/cpu.h"

void arch_cpu_init(void) {
    // ARM: MMU 초기화, 캐시 설정 등
    // 현재는 스텁만 구현
}

uint32_t arch_read_fault_addr(void) {
    // ARM: DFAR (Data Fault Address Register) 읽기
    uint32_t dfar;
    __asm__ __volatile__("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));
    return dfar;
}

void arch_disable_interrupts(void) {
    __asm__ __volatile__("cpsid i");
}

void arch_enable_interrupts(void) {
    __asm__ __volatile__("cpsie i");
}

void arch_halt(void) {
    __asm__ __volatile__("cpsid i");
    for (;;) {
        __asm__ __volatile__("wfi");
    }
}

void arch_idle(void) {
    __asm__ __volatile__("wfi");
}