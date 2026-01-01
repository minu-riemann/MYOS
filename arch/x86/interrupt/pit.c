#pragma once
#include "../io/ports.h"

#define PIT_CH0    0x40
#define PIT_CMD         0x43
#define PIT_BASE_HZ    1193182

static volatile uint64_t g_ticks = 0;

uint64_t pit_ticks(void) { return g_ticks; }

// IRQ0에서 호출할 tick 증가 함수(irq.c에서 사용)
void pit_on_tick(void) { g_ticks++; }

void pit_init(uint32_t hz) { 

    if (hz == 0) hz = 100;

    uint32_t divisor = PIT_BASE_HZ / hz;

    outb(PIT_CMD, 0x36); // channel 0, lobyte/hibyte, mode 3, binary
    outb(PIT_CH0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CH0, (uint8_t)((divisor >> 8)& 0xFF));
}