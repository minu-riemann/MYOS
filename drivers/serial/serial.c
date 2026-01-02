#include "serial.h"
#include "arch/io.h"

// 아키텍처별 시리얼 포트 주소
#ifdef ARCH_X86
#define SERIAL_BASE 0x3F8
#elif defined(ARCH_ARM)
#define SERIAL_BASE 0x101F1000  // Versatile Express UART0
#else
#define SERIAL_BASE 0x3F8  // 기본값 (x86)
#endif


static int serial_is_transmit_empty(void) {
    return arch_io_read8(SERIAL_BASE + 5) & 0x20;
}

void serial_init(void) {
    arch_io_write8(SERIAL_BASE + 1, 0x00); // Disable all interrupts
    arch_io_write8(SERIAL_BASE + 3, 0x80); // Enable DLAB (set baud rate divisor)
    arch_io_write8(SERIAL_BASE + 0, 0x03); // Set divisor to 3 (lo byte) - 38400 baud
    arch_io_write8(SERIAL_BASE + 1, 0x00); // Set divisor to 3 (hi byte)
    arch_io_write8(SERIAL_BASE + 3, 0x03); // Disable DLAB (8 bits, no parity, one stop bit)
    arch_io_write8(SERIAL_BASE + 2, 0xC7); // Enable FIFO, clear them with a threshold of 14
    arch_io_write8(SERIAL_BASE + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void serial_write_char(char c) {
    while (!serial_is_transmit_empty());
    arch_io_write8(SERIAL_BASE, c);
}

void serial_write(const char* s) {
    if (!s) return;
    while (*s) {
        if (*s == '\n') serial_write_char('\r');
        serial_write_char(*s++);
    }
}