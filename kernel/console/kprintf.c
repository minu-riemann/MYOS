#include "kprintf.h"
#include <stdint.h>
#include <stdarg.h>
#include "../../drivers/serial/serial.h"
#include "../panic/panic.h"
#include "../lib/itoa.h"


// -------------------------
// Minimal VGA console backend
// -------------------------
// 아키텍처별 VGA 메모리 주소
#ifdef ARCH_X86
#define VGA_MEM_BASE 0xB8000
#elif defined(ARCH_ARM)
#define VGA_MEM_BASE 0  // ARM은 VGA 없음, UART/LCD만
#else
#define VGA_MEM_BASE 0xB8000  // 기본값 (x86)
#endif

// VGA 메모리 포인터 (x86에서만 사용)
#if VGA_MEM_BASE != 0
static uint16_t* const VGA_MEM = (uint16_t*)VGA_MEM_BASE;
#else
static uint16_t* const VGA_MEM = NULL;  // ARM에서는 NULL
#endif

static const int VGA_W = 80;   // 추가: VGA 너비
static const int VGA_H = 25;   // 추가: VGA 높이

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
    #if VGA_MEM_BASE != 0
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
    #endif
    }

static void vga_putc_console(char c) {
#if VGA_MEM_BASE != 0
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

    if (c == '\t') {
        cur_x = (cur_x + 8) & ~7;
        if (cur_x >= VGA_W) {
            cur_x = 0;
            cur_y++;
            vga_scroll_if_needed();
        }
        return;
    }

    VGA_MEM[cur_y * VGA_W + cur_x] = ((uint16_t)vga_attr << 8) | (uint8_t)c;
    cur_x++;
    if (cur_x >= VGA_W) {
        cur_x = 0;
        cur_y++;
        vga_scroll_if_needed();
    }
#endif  // VGA_MEM_BASE != 0
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
#if VGA_MEM_BASE != 0
    for (int y = 0; y < VGA_H; y++) {
        for (int x = 0; x < VGA_W; x++) {
            VGA_MEM[y * VGA_W + x] = ((uint16_t)vga_attr << 8) | ' ';
        }
    }
    cur_x = 0;
    cur_y = 0;
#endif
}

void kprintf_puts_at(int y, int x, const char* s) {
#if VGA_MEM_BASE != 0
    if (y < 0 || y >= VGA_H || x < 0 || x >= VGA_W || !s) return;
    
    int old_x = cur_x;
    int old_y = cur_y;
    cur_x = x;
    cur_y = y;
    
    while (*s && cur_x < VGA_W) {
        VGA_MEM[cur_y * VGA_W + cur_x] = ((uint16_t)vga_attr << 8) | (uint8_t)(*s);
        cur_x++;
        s++;
    }
    
    cur_x = old_x;
    cur_y = old_y;
#endif
}