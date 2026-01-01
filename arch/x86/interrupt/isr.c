#include "isr.h"
#include "../../../drivers/serial/serial.h"
#include "../../../kernel/vga.h"
#include "../../../kernel/panic/panic.h"
#include "irq.h"

static const char* exception_messages[32] = {
    "Division By Zero (#DE)",
    "Debug (#DB)",
    "Non Maskable Interrupt",
    "Breakpoint (#BP)",
    "Overflow (#OF)",
    "Bound Range Exceeded (#BR)",
    "Invalid Opcode (#UD)",
    "Device Not Available (#NM)",
    "Double Fault (#DF)",
    "Coprocessor Segment Overrun",
    "Invalid TSS (#TS)",
    "Segment Not Present (#NP)",
    "Stack-Segment Fault (#SS)",
    "General Protection Fault (#GP)",
    "Page Fault (#PF)",
    "Reserved",
    "x87 Floating-Point Exception (#MF)",
    "Alignment Check (#AC)",
    "Machine Check (#MC)",
    "SIMD Floating-Point Exception (#XM/#XF)",
    "Virtualization Exception (#VE)",
    "Control Protection Exception (#CP)",
    "Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved"
};

static void u32_to_hex(uint32_t v, char out[11]) {
    // "0x" + 8 hex digits + null terminator ('\0')
    const char* h = "0123456789ABCDEF";
    out[0] = '0';
    out[1] = 'x';
    for (int i =0; i < 8; i++) {
        out[2 + i] = h[(v >> (28 - i*4)) & 0xF];
    }
    out[10] = '\0';
}

void isr_handler(regs_t* r) {

     if (r->int_no < 32) {
        const char* name = exception_messages[r->int_no];

        // VGA 출력
        vga_clear();
        vga_puts_at(2, 2, "EXCEPTION!");
        vga_puts_at(4, 2, name);

        // Serial: 상세로그
        serial_write("[EXC] ");
        serial_write(name);
        serial_write("\n");

        char buf[11];

        serial_write("  int_no=");
        u32_to_hex(r->int_no, buf);  serial_write(buf);
        serial_write("  err=");
        u32_to_hex(r->err_code, buf);  serial_write(buf);
        serial_write("\n");

        serial_write("  eip=");
        u32_to_hex(r->eip, buf);  serial_write(buf);
        serial_write("  cs=");
        u32_to_hex(r->cs, buf);  serial_write(buf);
        serial_write("  eflags=");
        u32_to_hex(r->eflags, buf);  serial_write(buf);
        serial_write("\n");

        // 최종적으로 panic으로 정지 (상태 통일)
        panic("CPU exception trapped. System halted.");
     } else if (r->int_no >= 32 && r->int_no < 48) {
        // IRQ 처리
        irq_dispatch(r);
        return;
     } else {
        // 알 수 없는 인터럽트
        serial_write("[WARN] Unknown interrupt received: ");
        char buf[11];
        u32_to_hex(r->int_no, buf);
        serial_write(buf);
        serial_write("\n");
     }

}