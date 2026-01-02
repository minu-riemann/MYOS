#include "arch/timer.h"

static volatile uint64_t g_ticks = 0;

void arch_timer_init(uint32_t hz) {
    // ARM: Generic Timer 또는 하드웨어 타이머 초기화
    // 현재는 스텁만 구현
    (void)hz;
}

void arch_timer_on_tick(void) {
    g_ticks++;
}

uint64_t arch_timer_ticks(void) {
    return g_ticks;
}