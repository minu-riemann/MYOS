#include <stdint.h>
#include "vga.h"


// 0xB8000: x86 PC의 전통적인 VGA 텍스트 모드 메모리 주소
static volatile uint16_t* const VGA = (uint16_t *)0xB8000;

// 화면은 80x25, 각 칸이 2바이트(문자 + 색상)
static const int COLS = 80;
static const int ROWS = 25;

static inline uint16_t vga_entry(char ch, uint8_t color) {
    return (uint16_t)ch | ((uint16_t)color << 8);
}

void vga_clear(void) {
    uint16_t blank = vga_entry(' ', 0x07); // Light grey on black
    for (int i = 0; i < COLS * ROWS; i++) {
        VGA[i] = blank;
    }
}

void vga_puts_at(int row, int col, const char *s) {
    int idx = row * COLS + col;
    while (*s) {
        VGA[idx++] = vga_entry(*s++, 0x07); // Light grey on black
    }
}