#include "../../../include/arch/interrupt.h"

void arch_interrupt_init(void) {
    // ARM: 벡터 테이블 설정 + GIC 초기화
    // 현재는 스텁만 구현
}

void arch_irq_register(uint8_t irq, arch_irq_handler_t handler) {
    // ARM: GIC에 핸들러 등록
    // 현재는 스텁만 구현
    (void)irq;
    (void)handler;
}

void arch_irq_enable(uint8_t irq) {
    // ARM: GIC에서 인터럽트 활성화
    // 현재는 스텁만 구현
    (void)irq;
}

void arch_irq_disable(uint8_t irq) {
    // ARM: GIC에서 인터럽트 비활성화
    // 현재는 스텁만 구현
    (void)irq;
}

void arch_irq_eoi(uint8_t irq) {
    // ARM: GIC에 EOI 전송
    // 현재는 스텁만 구현
    (void)irq;
}