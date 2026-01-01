#pragma once
#include <stdint.h>

// PIT tick마다 1회 호출 (IRQ0에서 호출)
void time_on_tick(void);

// 현재 tick 값 반환 (monotonic)
uint64_t timer_ticks(void);

// PIT 주파수 설정값(Hz)을 time 모듈에 알려줌 (pit_init 이후 1회 호출)
void time_set_hz(uint32_t hz);

// ms 단위 sleep (Phase1: busy-wait)
void sleep_ms(uint32_t ms);