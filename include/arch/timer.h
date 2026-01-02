#pragma once
#include <stdint.h>

/**
 * 타이머 초기화
 * @param hz 타이머 주파수 (Hz)
 * - x86: PIT 초기화
 * - ARM: Generic Timer 또는 하드웨어 타이머 초기화
 */
void arch_timer_init(uint32_t hz);

/**
 * 타이머 tick 증가 (IRQ 핸들러에서 호출)
 * - x86: PIT tick 증가
 * - ARM: Generic Timer tick 증가
 */
void arch_timer_on_tick(void);

/**
 * 현재 타이머 tick 값 반환
 * @return 현재 tick 값
 */
uint64_t arch_timer_ticks(void);