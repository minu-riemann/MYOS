#pragma once
#include <stdint.h>

/**
 * 아키텍처별 레지스터 구조체 (공통 인터페이스)
 */
typedef struct arch_regs {
    uint32_t int_no;        // 인터럽트 번호
    uint32_t err_code;      // 에러 코드 (일부 예외에만 존재)
    uint32_t pc;            // x86: eip, ARM: pc
    uint32_t flags;         // x86: eflags, ARM: cpsr
    void* arch_specific;     // 아키텍처별 확장 데이터
} arch_regs_t;

/**
 * 인터럽트 핸들러 타입
 */
typedef void (*arch_irq_handler_t)(arch_regs_t*);

/**
 * 인터럽트 시스템 초기화
 * - x86: IDT 초기화 + PIC remap
 * - ARM: 벡터 테이블 설정 + GIC 초기화
 */
void arch_interrupt_init(void);

/**
 * 인터럽트 핸들러 등록
 * @param irq 인터럽트 번호
 * @param handler 핸들러 함수
 */
void arch_irq_register(uint8_t irq, arch_irq_handler_t handler);

/**
 * 인터럽트 활성화
 * @param irq 인터럽트 번호
 */
void arch_irq_enable(uint8_t irq);

/**
 * 인터럽트 비활성화
 * @param irq 인터럽트 번호
 */
void arch_irq_disable(uint8_t irq);

/**
 * EOI (End Of Interrupt) 전송
 * - x86: PIC에 EOI 전송
 * - ARM: GIC에 EOI 전송
 * @param irq 인터럽트 번호
 */
void arch_irq_eoi(uint8_t irq);