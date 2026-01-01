#include "isr.h"
#include "../../../drivers/serial/serial.h"
#include "../../../kernel/lib/itoa.h" 
#include "../../../kernel/console/kprintf.h"
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

    kprintf("  cause: %s, %s, %s",
      is_protection ? "protection-violation" : "non-present",
      is_write ? "write" : "read",
      is_user ? "user" : "kernel");

    if (is_rsvd)  kprintf(", rsvd");
    if (is_ifetch) kprintf(", instr-fetch");
    kprintf("\n");
}

static void dump_regs(regs_t* r) {
   kprintf("  eip=0x%x cs=0x%x eflags=0x%x\n",
      r->eip, r->cs, r->eflags);

  kprintf("  eax=0x%x ebx=0x%x ecx=0x%x edx=0x%x\n",
      r->eax, r->ebx, r->ecx, r->edx);

  kprintf("  esi=0x%x edi=0x%x ebp=0x%x esp=0x%x\n",
      r->esi, r->edi, r->ebp, r->esp);

  kprintf("  useresp=0x%x ss=0x%x\n",
      r->useresp, r->ss);
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

      // 화면에 심각한 오류 표시
      kprintf_clear_console();
      kprintf_puts_at(2, 2, "PAGE FAULT (#PF)");
      kprintf_puts_at(4, 2, "See console for details.");
      
      // 상세 로그는 kprintf로
      kprintf("\n==============================\n");
      kprintf("[EXC] Page Fault (#PF)\n");
      kprintf("==============================\n");
      kprintf("  cr2=0x%x err=0x%x\n", cr2, r->err_code);

      pf_print_reason(r->err_code);
      dump_regs(r);

      panic("Page fault trapped. System halted.");

   } else if (r->int_no < 32) {
      const char* name = exception_messages[r->int_no];

      // 화면에 심각한 오류 표시
      kprintf_clear_console();
      kprintf_puts_at(2, 2, "EXCEPTION!");
      kprintf_puts_at(4, 2, name);

      // 상세 로그는 kprintf로
      kprintf("[EXC] %s\n", name);
      kprintf("  int_no=0x%x err=0x%x\n", r->int_no, r->err_code);
      kprintf("  eip=0x%x cs=0x%x eflags=0x%x\n", r->eip, r->cs, r->eflags);

      panic("CPU exception trapped. System halted.");

   } else if (r->int_no >= 32 && r->int_no < 48) {
      // IRQ 처리
      irq_dispatch(r);
      return;
   } else {
      // 알 수 없는 인터럽트 (경고 로그)
      kprintf("[WARN] Unknown interrupt received: 0x%x\n", r->int_no);
   }

}
