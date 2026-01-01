#pragma once
#include <stdarg.h>

void kprintf(const char* fmt, ...);
void kvprintf(const char* fmt, va_list args);

// 옵션: 로그 레벨용(원하면 나중에 사용)
void kputs(const char* s);

void kprintf_set_cursor(int x, int y);
void kprintf_clear_console(void);

// 특정 위치에 문자열 출력 (vga_puts_at 대체)
void kprintf_puts_at(int row, int col, const char* s);