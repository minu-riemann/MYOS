#include "isr.h"
#include "../../../drivers/serial/serial.h"
#include "../../../kernel/lib/itoa.h" 
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


static inline uint32_t read_cr2(void) {
   uint32_t val;
   __asm__ __volatile__("mov %%cr2, %0" : "=r"(val));
   return val;
}

void isr_handler(regs_t* r) {

   // Page Fault (#PF)
   if (r->int_no == 14) {
      uint32_t cr2 = read_cr2();

      // Decode error code
      int present    = (r->err_code & 0x1) != 0; // 1: protection violation
      int write      = (r->err_code & 0x2) != 0; // 1: write, 0: read
      int user       = (r->err_code & 0x4) != 0; // 1: user, 0: kernel
      int rsvd       = (r->err_code & 0x8) != 0; // 1: reserved bit set
      int instr      = (r->err_code & 0x10) != 0; // 1: instruction fetch

         // VGA (간단 요약)
      vga_clear();
      vga_puts_at(2, 2, "PAGE FAULT (#PF)");
      vga_puts_at(4, 2, "See serial for details.");

      // Serial (상세)
      serial_write("[EXC] Page Fault (#PF)\n");

      char buf[11];

      serial_write("  cr2=");
      u32_to_hex(cr2, buf); serial_write(buf);

      serial_write("  eip=");
      u32_to_hex(r->eip, buf); serial_write(buf);

      serial_write("  err=");
      u32_to_hex(r->err_code, buf); serial_write(buf);
      serial_write("\n");

      serial_write("  reason: ");
      serial_write(present ? "protection" : "not-present");
      serial_write(", ");
      serial_write(write ? "write" : "read");
      serial_write(", ");
      serial_write(user ? "user" : "kernel");
      if (rsvd)  serial_write(", rsvd");
      if (instr) serial_write(", instr-fetch");
      serial_write("\n");

      panic("Page fault trapped. System halted.");
      

   } else if (r->int_no < 32) {
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
