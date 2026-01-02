#include "time.h"
#include "arch/timer.h"
#include "../../kernel/console/kprintf.h"  
#include "../../kernel/panic/panic.h"

static volatile uint64_t g_ticks = 0;
static uint32_t g_hz = 0;

void time_on_tick(void) {
    g_ticks++;
    arch_timer_on_tick();  // 아키텍처별 타이머 tick
}

uint64_t timer_ticks(void) {
    // arch_timer_ticks()를 사용하거나, g_ticks를 사용
    // 일단 g_ticks를 유지하되, arch_timer_on_tick()을 호출하도록 수정
    return g_ticks;
}

void time_set_hz(uint32_t hz) {
    if (hz == 0) {
        panic("time_set_hz: hz=0");
    }
    g_hz = hz;
}


// Busy-wait sleep (Phase1)
void sleep_ms(uint32_t ms) {
    if (g_hz == 0) {
        panic("sleep_ms: time hz not set (call time_set_hz after pit_init)");
    }

    if (ms == 0) return;

    // 32비트 범위 내에서 계산
    // delta = ceil(ms * hz / 1000)
    uint32_t product = ms * g_hz;
    uint32_t delta = (product + 999) / 1000;  // 32비트 나눗셈 (컴파일러 최적화)

    if (delta == 0) delta = 1;

    uint64_t start = timer_ticks();
    uint64_t target = start + (uint64_t)delta;

    while (timer_ticks() < target) {
        // CPU 점유 줄이기: 인터럽트는 켜져 있어야 tick이 올라갑니다.
        __asm__ __volatile__("sti; hlt; cli");  // sti 추가
    }
}