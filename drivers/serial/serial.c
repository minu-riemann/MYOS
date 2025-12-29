#include "serial.h"

#define COM1_PORT 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int serial_is_transmit_empty(void) {
    return inb(COM1_PORT + 5) & 0x20;
}

void serial_init(void) {
    outb(COM1_PORT + 1, 0x00); // Disable all interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) - 38400 baud / Baud rate divisor low (38400)
    outb(COM1_PORT + 1, 0x00); // Set divisor to 3 (hi byte)
    outb(COM1_PORT + 3, 0x03); // Disable DLAB (8 bits, no parity, one stop bit)
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them with a threshold of 14
    outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set (for completeness)
}

void serial_write_char(char c) {
    while (!serial_is_transmit_empty());
    outb(COM1_PORT, c);
}

void serial_write(const char* s) {
    if (!s) return;
    while (*s) {
        if (*s == '\n') serial_write_char('\r');
        serial_write_char(*s++);
    }
}