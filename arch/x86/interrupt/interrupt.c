#include "arch/interrupt.h"
#include "idt.h"
#include "pic.h"
#include "irq.h"
#include "isr.h"

// regs_t를 arch_regs_t로 변환하는 헬퍼 함수
static arch_regs_t regs_to_arch_regs(regs_t* r) {
    arch_regs_t arch_regs;
    arch_regs.int_no = r->int_no;
    arch_regs.err_code = r->err_code;
    arch_regs.pc = r->eip;
    arch_regs.flags = r->eflags;
    arch_regs.arch_specific = r;  // 원본 regs_t 포인터 저장
    return arch_regs;
}

// arch_regs_t를 regs_t로 변환하는 헬퍼 함수
static regs_t* arch_regs_to_regs(arch_regs_t* arch_regs) {
    return (regs_t*)arch_regs->arch_specific;
}

// arch_irq_handler_t를 irq_handler_t로 래핑하는 어댑터
static void irq_adapter(regs_t* r) {
    arch_regs_t arch_regs = regs_to_arch_regs(r);
    // handler는 arch_regs_t를 받지만, 실제로는 regs_t를 사용
    // 이 부분은 나중에 수정 필요할 수 있음
}

void arch_interrupt_init(void) {
    idt_init();
    irq_init();  // IRQ 핸들러 등록 및 PIC 설정
}

void arch_irq_register(uint8_t irq, arch_irq_handler_t handler) {
    // 일단 기존 irq_register_handler를 사용
    // 타입 변환이 필요하므로 임시로 void* 캐스팅
    irq_register_handler(irq, (irq_handler_t)(void*)handler);
}

void arch_irq_enable(uint8_t irq) {
    pic_clear_mask(irq);
}

void arch_irq_disable(uint8_t irq) {
    pic_set_mask(irq);
}

void arch_irq_eoi(uint8_t irq) {
    pic_send_eoi(irq);
}