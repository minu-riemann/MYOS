#pragma once
#include <stdint.h>

typedef struct regs regs_t;

typedef void (*irq_handler_t)(regs_t* r);

void irq_init(void);
void irq_register_handler(uint8_t irq, irq_handler_t handler);
void irq_dispatch(regs_t* r);

