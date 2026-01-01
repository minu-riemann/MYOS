#include "irq.h"
#include "isr.h"
#include "pic.h"
#include "pit.h"
#include "../../../drivers/serial/serial.h"

#define IRQ_BASE 32

static irq_handler_t g_irq_handlers[16] = {0};

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq < 16) g_irq_handlers[irq] = handler;
}

static void irq0_timer(regs_t* r) {
    (void)r;
    pit_on_tick();

    // 너무 자주 로그를 출력하면 안되므로, 100틱 처리
    if ((pit_ticks() % 100)  == 0) {
        serial_write("[TICK] 100 ticks\n");
    }
}

void irq_init(void) {
    // IRQ0(timer)만 우선 등록
    irq_register_handler(0, irq0_timer);

    // PIC remap: IRQ0 -> 32, IRQ8 -> 40
    pic_remap(0x20, 0x28);

    // 마스크: 타이머만 열고 나머지 닫기(안정화)
    for (uint8_t i = 0; i < 16; i++) pic_set_mask((uint8_t)i);

    pic_clear_mask(0); // IRQ0(timer) Enable
    pic_clear_mask(1);

    serial_write("[INFO] PIC remapped, IRQ0/IRQ1 unmasked\n");
}

// isr_handler에서 호출될 IRQ 공통 핸들러
void irq_dispatch(regs_t* r) {
    uint8_t irq = (uint8_t)(r->int_no - IRQ_BASE);

    if (irq < 16) {
       if (g_irq_handlers[irq]) {
        g_irq_handlers[irq](r);
       } else {
        serial_write("[WARN] Unhandled IRQ\n");
       }
    }

    pic_send_eoi(irq);
}