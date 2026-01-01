#include "panic.h"
#include "../console/kprintf.h"

__attribute__((noreturn))
void panic(const char* msg) {
    // 콘솔 초기화 및 메시지 출력
    kprintf_clear_console();
    kprintf_puts_at(10, 0, "KERNEL PANIC!!!");
    kprintf_puts_at(11, 0, "");
    
    // 상세 로그는 kprintf로 (Serial + VGA 모두)
    kprintf("\n[PANIC] %s\n", msg ? msg : "(no message)");

    for (;;) {
        __asm__ __volatile__("cli; hlt");
    }
}