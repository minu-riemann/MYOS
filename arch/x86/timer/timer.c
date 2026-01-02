#include "arch/timer.h"
#include "../interrupt/pit.h"

void arch_timer_init(uint32_t hz) {
    pit_init(hz);
}

void arch_timer_on_tick(void) {
    pit_on_tick();
}

uint64_t arch_timer_ticks(void) {
    return pit_ticks();
}