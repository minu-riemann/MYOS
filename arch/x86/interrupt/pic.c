#include "pic.h"
#include "../io/ports.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

static void pic_wait(void) { io_wait(); }

void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); pic_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); pic_wait();

    outb(PIC1_DATA, offset1); pic_wait(); // master vector offset
    outb(PIC2_DATA, offset2); pic_wait(); // slave vector offset

    outb(PIC1_DATA, 0x04); pic_wait(); // tell Master about Slave at IRQ2
    outb(PIC2_DATA, 0x02); pic_wait(); // tell Slave its cascade identity

    outb(PIC1_DATA, ICW4_8086); pic_wait();
    outb(PIC2_DATA, ICW4_8086); pic_wait();

    // restore masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}

void pic_set_mask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    if (irq >= 8) irq -= 8;
    uint8_t value = inb(port) | (1 << irq);
    outb(port, value);
}

void pic_clear_mask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    if (irq >= 8) irq -= 8;
    uint8_t value = inb(port) & ~(1 << irq);
    outb(port, value);
}
