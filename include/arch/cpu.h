#pragma once
#include <stdint.h>

/**
 * CPU 초기화
 * - x86: GDT 초기화
 * - ARM: MMU 초기화, 캐시 설정
 */
void arch_cpu_init(void);

/**
 * 페이지 폴트 주소 읽기
 * - x86: CR2 레지스터 읽기
 * - ARM: DFAR (Data Fault Address Register) 읽기
 */
uint32_t arch_read_fault_addr(void);

/**
 * 인터럽트 비활성화
 * - x86: cli 명령
 * - ARM: CPSR 수정
 */
void arch_disable_interrupts(void);

/**
 * 인터럽트 활성화
 * - x86: sti 명령
 * - ARM: CPSR 수정
 */
void arch_enable_interrupts(void);

/**
 * CPU 정지 (무한 루프)
 * - x86: hlt 명령
 * - ARM: wfi 명령 또는 무한 루프
 */
void arch_halt(void);

/**
 * CPU 유휴 상태 (인터럽트 대기)
 * - x86: sti; hlt; cli
 * - ARM: wfi (Wait For Interrupt)
 */
void arch_idle(void);