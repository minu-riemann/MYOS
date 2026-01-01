#include "isr.h"
#include "../../../drivers/serial/serial.h"
#include "../../../kernel/lib/itoa.h" 
#include "../../../kernel/vga.h"
#include "../../../kernel/panic/panic.h"
#include "irq.h"

static void serial_hex(const char* key, uint32_t v) {
   char buf[11];
   serial_write(key);
   u32_to_hex(v, buf);
   serial_write(buf);
}

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

static void pf_print_reason(uint32_t err) {
   // err bit meanings:
    // bit0 P: 0=not-present, 1=protection violation
    // bit1 W/R: 0=read, 1=write
    // bit2 U/S: 0=kernel, 1=user
    // bit3 RSVD: 1=reserved bit violation
    // bit4 I/D: 1=instruction fetch

    int is_protection = (err & 0x1) != 0;
    int is_write      = (err & 0x2) != 0;
    int is_user       = (err & 0x4) != 0;
    int is_rsvd       = (err & 0x8) != 0;
    int is_ifetch     = (err & 0x10) != 0;

    serial_write("  cause: ");
    serial_write(is_protection ? "protection-violation" : "non-present");
    serial_write(", ");
    serial_write(is_write ? "write" : "read");
    serial_write(", ");
    serial_write(is_user ? "user" : "kernel");

    if (is_rsvd)  serial_write(", rsvd");
    if (is_ifetch) serial_write(", instr-fetch");
    serial_write("\n");
}

static void dump_regs(regs_t* r) {
   serial_hex("  eip=", r->eip);
   serial_hex(" cs=", r->cs);
   serial_hex(" eflags=", r->eflags);
   serial_write("\n");

   serial_hex("  eax=", r->eax);
   serial_hex(" ebx=", r->ebx);
   serial_hex(" ecx=", r->ecx);
   serial_hex(" edx=", r->edx);
   serial_write("\n");

   serial_hex("  esi=", r->esi);
   serial_hex(" edi=", r->edi);
   serial_hex(" ebp=", r->ebp);
   serial_hex(" esp=", r->esp);      // pusha가 저장한 esp(예외 시점의 esp 스냅샷)
   serial_write("\n");

   // ring3에서 들어온 경우에만 유의미하지만, 찍어두면 나중에 확장에 도움됨
   serial_hex("  useresp=", r->useresp);
   serial_hex(" ss=", r->ss);
   serial_write("\n");
}


static inline uint32_t read_cr2(void) {
   uint32_t val;
   __asm__ __volatile__("mov %%cr2, %0" : "=r"(val));
   return val;
}

void isr_handler(regs_t* r) {

   // Page Fault (#PF)
   if (r->int_no == 14) {
      uint32_t cr2 = read_cr2();

      // VGA는 간단히 안내만 (기존 유지)
      vga_clear();
      vga_puts_at(2, 2, "PAGE FAULT (#PF)");
      vga_puts_at(4, 2, "See serial for details.");

      // Serial은 표준 포맷으로 상세 출력
      serial_write("\n==============================\n");
      serial_write("[EXC] Page Fault (#PF)\n");
      serial_write("==============================\n");

      serial_hex("  cr2=", cr2);
      serial_hex(" err=", r->err_code);
      serial_write("\n");

      pf_print_reason(r->err_code);
      dump_regs(r);

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
