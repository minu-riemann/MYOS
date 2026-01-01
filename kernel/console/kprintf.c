#include "kprintf.h"
#include <stdint.h>
#include <stdarg.h>
#include "../../drivers/serial/serial.h"
#include "../panic/panic.h"
#include "../lib/itoa.h"


// -------------------------
// Minimal VGA console backend
// -------------------------
static uint16_t* const VGA_MEM = (uint16_t*)0xB8000;
static const int VGA_W = 80;
static const int VGA_H = 25;

static int cur_x = 0;
static int cur_y = 0;
static uint8_t vga_attr = 0x07; // light grey on black

static volatile int kprintf_lock = 0;

static void lock(void) {
    while (__sync_lock_test_and_set(&kprintf_lock, 1)) { }
}

static void unlock(void) {
    __sync_lock_release(&kprintf_lock);
}

static void vga_scroll_if_needed(void) {
    if (cur_y < VGA_H) return;

    // scroll up by one line
    for (int y = 1; y < VGA_H; y++) {
        for (int x = 0; x < VGA_W; x++) {
            VGA_MEM[(y - 1) * VGA_W + x] = VGA_MEM[y * VGA_W + x];
        }
    }

    // clear last line
    for (int x = 0; x < VGA_W; x++) {
        VGA_MEM[(VGA_H - 1) * VGA_W + x] = ((uint16_t)vga_attr << 8) | ' ';
    }

    cur_y = VGA_H - 1;
}

static void vga_putc_console(char c) {
    if (c == '\n') {
        cur_x = 0;
        cur_y++;
        vga_scroll_if_needed();
        return;
    }
    if (c == '\r') {
        cur_x = 0;
        return;
    }
    if ( c == '\t') {
        int next = (cur_x + 4) & ~3;
        while (cur_x < next) vga_putc_console(' ');
        return;
    }

    VGA_MEM[cur_y * VGA_W + cur_x] = ((uint16_t)vga_attr << 8) | (uint8_t)c;
    cur_x++;

    if (cur_x >= VGA_W) {
        cur_x = 0;
        cur_y++;
        vga_scroll_if_needed();
    }
}

// Unified output: VGA + Serial
static void kout_char(char c) {
    vga_putc_console(c);

    char s[2] = {c, 0};
    serial_write(s);
}

static void kout_str(const char* s) {
    if (!s) s = "(null)";
    while (*s) kout_char(*s++);
}


// -------------------------
// Formatting helpers
// -------------------------
static void kout_u32_hex(uint32_t v) {
    char buf[11];
    u32_to_hex(v, buf);
    kout_str(buf);
}

static void kout_u32_dec(uint32_t v) {
    // 최소 구현
    char tmp[11];
    int i = 0;
    if (v == 0) {
        kout_char('0');
        return;
    }
    while (v && i < 10) {
        tmp[i++] = '0' + (v % 10);
        v /= 10;
    }
    for (int j = i - 1; j >= 0; j--) kout_char(tmp[j]);
}

static void kout_i32_dec(int32_t v) {
    if (v < 0) {
        kout_char('-');
        // INT32_MIN도 안전하게 처리
        uint32_t uv = (uint32_t)(~(uint32_t)v) + 1; // two's complement abs
        kout_u32_dec(uv);
    } else {
        kout_u32_dec((uint32_t)v);
    }
}

void kvprintf(const char* fmt, va_list args) {
    for (const char* p = fmt; *p; p++) {
        if (*p != '%') {
            kout_char(*p);
            continue;
        }

        p++; // skip '%'
        if (*p == 0) break;

        switch (*p) {
            case '%':
                kout_char('%');
                break;
            case 'c': {
                int c = va_arg(args, int);
                kout_char((char)c);
                break;
            }
            case 's': {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                kout_str(s);
                break;
            }
            case 'x': {
                uint32_t v = va_arg(args, uint32_t);
                kout_u32_hex(v);
                break;
            }
            case 'u': {
                uint32_t v = va_arg(args, uint32_t);
                kout_u32_dec(v);
                break;
            }
            case 'd': {
                int32_t v = va_arg(args, int32_t);
                kout_i32_dec(v);
                break;
            }
            default:
                // 알 수 없는 포맷은 그대로 출력해 디버깅 가능하게
                kout_char('%');
                kout_char(*p);
                break;
        }
    }
}

void kprintf(const char* fmt, ...) {
    lock();
    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);
    unlock();
}

void kprintf_set_cursor(int x, int y) {
    if (x < 0) x = 0; if (x >= VGA_W) x = VGA_W - 1;
    if (y < 0) y = 0; if (y >= VGA_H) y = VGA_H - 1;
    cur_x = x;
    cur_y = y;
}

void kprintf_clear_console(void) {
    for (int y = 0; y < VGA_H; y++) {
        for (int x = 0; x < VGA_W; x++) {
            VGA_MEM[y * VGA_W + x] = ((uint16_t)vga_attr << 8) | ' ';
        }
    }
    cur_x = 0;
    cur_y = 0;
}

void kprintf_puts_at(int row, int col, const char* s) {
    // 커서 위치 저장
    int old_x = cur_x;
    int old_y = cur_y;
    
    // 새 위치 설정
    kprintf_set_cursor(col, row);
    
    // 문자열 출력
    kout_str(s);
    
    // 원래 커서 위치 복원 (선택적, 필요하면 주석 처리)
    // cur_x = old_x;
    // cur_y = old_y;
}