#include "time.h"
#include "../console/kprintf.h"  
#include "../panic/panic.h"

static volatile uint64_t g_ticks = 0;
static uint32_t g_hz = 0;

void time_on_tick(void) {
    g_ticks++;
}

uint64_t timer_ticks(void) {
    // 32-bit 환경에서 64-bit 읽기 경쟁을 피하려면 원칙적으로 IRQ disable이 필요하지만,
    // Phase1 busy-wait 용도로는 대부분 충분합니다.
    // 더 안전하게 하려면 arch 레벨에서 IRQ off/on을 제공한 뒤 보호하면 됩니다.
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