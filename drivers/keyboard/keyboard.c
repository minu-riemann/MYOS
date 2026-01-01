#include "keyboard.h"
#include "../../kernel/lib/itoa.h"
#include "../../arch/x86/interrupt/irq.h"
#include "../../arch/x86/io/ports.h"
#include "../serial/serial.h"

static const char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    // 0-9
    '9', '0', '-', '=', '\b',    // Backspace
    '\t',        // Tab
    'q', 'w', 'e', 'r',    // 10-19
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    // Enter key
    0,        // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    // 20-29
    '\'', '`',
    0,        // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',        // 30-39
    'm', ',', '.', '/',
    0,        // Right shift
    '*',
    0,        // Alt
    ' ',    // Space bar
    0,        // Caps lock
    // Remaining keys are not mapped
};

static void keyboard_irq(regs_t* r) {
    (void)r;

    uint8_t sc = inb(0x60);

    // break code (키 떼기)는 상위비트 1(0x80) set
    if (sc & 0x80) {
        // 키 떼기 이벤트 무시
        return;
    }   

    char c = 0;
    if (sc < 128) c = scancode_to_ascii[sc];

    serial_write("[KBD] sc=");
    char buf[11];
    u32_to_hex(sc, buf);
    serial_write(buf);

    if (c) {
        serial_write(" '");
        char s[2] = {c, 0};
        serial_write(s);
        serial_write("'");
    }

    serial_write("\n");
}

void keyboard_init(void) {
    irq_register_handler(1, keyboard_irq);
    serial_write("[INFO] Keyboard IRQ handler registered\n");
}