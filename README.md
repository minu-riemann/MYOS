# MYOS

A small educational x86 (i386) OS kernel built from scratch.

Boots via **GRUB (Multiboot)**, runs on **QEMU**, and provides a **debuggable kernel platform**
designed as a foundation for both **RTOS-style kernels** and **Linux-like operating systems**.

This project focuses on understanding low-level OS fundamentals through hands-on
implementation of CPU setup, interrupt handling, and hardware timers

## Current Status

- [x] GRUB Multiboot bootable kernel
- [x] VGA text-mode output
- [x] Serial debug output (COM1)
- [x] Panic / assert-based kernel halt
- [x] GDT (Null / Kernel Code / Kernel Data)
- [x] IDT + CPU exception handling
- [x] PIC remap + IRQ handling
- [x] PIT timer interrupt (tick verified)
- [x] Paging enabled (minimal identity mapping)
- [x] Page Fault (#PF) handler (CR2 + error code logging)
- [x] Multiboot memory map parsing
- [x] Kernel memory layout detection
- [x] Kernel heap allocator (bump allocator)
- [x] kmalloc/kmalloc_aligned implementation
- [x] kprintf console (VGA + Serial unified output)
- [x] Unified logging system (all logs via kprintf)
- [x] Error display system (panic/exceptions via kprintf_puts_at)
- [x] Time management (PIT-based tick counter)
- [x] sleep(ms) implementation (busy-wait)

**Verified behavior**
- `ud2` triggers **#UD (Invalid Opcode)**  
  → ISR prints CPU context and halts via `panic()`
- PIT IRQ0 generates periodic ticks  
  → `[TICK] 100 ticks` after `sti`
- Heap allocator initializes from Multiboot memory map
  → `kmalloc()` returns aligned addresses, OOM detection works
- kprintf supports %d, %x, %s, %u, %c formats
  → Unified output to both VGA console and Serial port
- Panic and exceptions display on screen via kprintf_puts_at
  → Critical errors visible on VGA, detailed logs via kprintf
- sleep(ms) works with PIT tick counter
  → Time-based delays functional for driver/application use

## Development Roadmap

### Phase 1. Kernel Platform Stabilization ✅ COMPLETE
**Goal: A fully debuggable kernel platform**
- [x] Serial logging (COM1) + panic + assert
- [x] GDB debugging via QEMU (`-s -S`) with ELF symbols
- [x] CPU exception handling (#DE, #UD, #PF, etc.)
- [x] PIC remap and IRQ dispatch framework
- [x] PIT-based timer tick (time foundation)
- [x] sleep(ms) implementation
- [x] Unified logging system (kprintf)
- [x] Memory management (heap allocator)

### Phase 2. RTOS Track (MCU / Automotive / Embedded)
**Goal: Implement a FreeRTOS-class kernel**
- Preemptive scheduler (priority-based)
- Task creation/destruction and stack management
- PIT-based time slicing
- IPC primitives (semaphore / mutex / queue, at least two)
- Software timers and `sleep(ms)`

### Phase 3. Linux Track (Processes / Syscalls / User mode)
**Goal: A Linux-like minimal OS**
- Paging and kernel address space design
- Processes with isolated address spaces
- Context switching
- System call entry (`int 0x80` or `sysenter`)
- ELF loader for user programs
- Simple filesystem (RAMFS → FAT / EXT2 read-only)


## Project Structure

```text
boot/
  entry.asm                # Kernel entry, stack setup, transition to C

arch/x86/
  cpu/
    gdt.c, gdt.h           # Global Descriptor Table
    gdt_flush.asm          # lgdt + segment reload

  interrupt/
    idt.c, idt.h           # Interrupt Descriptor Table
    isr.c, isr.h           # Exception / IRQ dispatch
    isr_stub.asm           # ASM ISR stubs → C handlers
    pic.c, pic.h           # PIC remap and EOI
    pit.c, pit.h           # PIT timer (IRQ0)

  io/
    ports.h                # inb / outb / io_wait helpers

drivers/
  serial/
    serial.c, serial.h     # COM1 (0x3F8) serial debug output
  keyboard/
    keyboard.c, keyboard.h # Keyboard IRQ (IRQ1)

kernel/
  kernel.c                 # kernel_main()
  console/
    kprintf.c, kprintf.h   # Formatted output (VGA + Serial)
  time/
    time.c, time.h         # Time management, sleep(ms)
  memory/
    multiboot.c, multiboot.h  # Multiboot info parsing, memory map
    heap.c, heap.h           # Kernel heap allocator (bump allocator)
  panic/
    panic.c, panic.h       # panic() implementation
  lib/
    itoa.c, itoa.h         # Integer → hex conversion utilities

linker.ld                  # Linker script (memory layout)
Makefile                   # Build / ISO / QEMU automation

```

## Theory Desc

### GRUB = Boot loader
+ GRUB는 BIOS 이후 단계에서 실행되는 부트로더
+ 커널 바이너리를 메모리에 로드하고 CPU를 보호모드(32-bit)로 전환한 뒤, 커널의 엔트리 포인트(start)로 제어권을 넘긴다.
+ 이 프로젝트에서는 멀티부트 규격을 사용하여 GRUB가 커널을 인식하고 실행할 수 있도록 구성되어 있다.
+ 본 프로젝트는 Multiboot 규격을 사용한다.

### GDT = Global Descriptor Table
+ x86 보호모드에서 메모리 접근 규칙을 정의
+ 코드/데이터 세그먼트의 권한, 범위, 속성을 지정
+ x86 보호모드에서는 CPU가 메모리에 단순한 물리 주소로 접근하지 않는다.
+ 모든 메모리 접근은 세그먼트 디스크립터를 통해 이루어진다.
+ GDT는 다음을 정의한다:
    + 코드 세그먼트(Code Segment)
    + 데이터 세그먼트(Data Segment)
    + 각 세그먼트의 권한(Ring level), 범위, 속성
+ 커널은 Ring 0 세그먼트에서 실행됨


### IDT = Interrupt Descriptor Table
+ IDT는 인터럽트 또는 CPU 예외가 발생했을 때 CPU가 점프해야 할 코드 세그먼트와 함수 주소를 정의하는 테이블이다.
+ Divide by Zero (#DE), Invalid Opcode (#UD), Page Fault (#PF)와 같은 예외가 발생하면,
CPU는 IDT를 참조하여 해당 예외를 처리할 핸들러로 제어를 이동한다.

### ISR Stub (Interrupt Service Routine Stub)
+ CPU가 인터럽트나 예외를 발생시키면, 레지스터와 스택 상태는 C 함수 호출 규약과 호환되지 않는 형태로 전달된다.
+ SR Stub은 이 간극을 메우는 어셈블리 코드로, 다음 역할을 수행한다:
    + CPU가 저장한 레지스터 상태를 보존
    + C 코드가 처리할 수 있는 형태로 스택을 정리
    + C 기반 예외/인터럽트 핸들러 호출
    + iret 명령을 통해 원래 실행 흐름으로 복귀
+ 이를 통해 하드웨어 이벤트를 C 코드에서 안전하게 처리할 수 있다.

### PIC / PIT
+ PIC: 하드웨어 IRQ를 CPU 인터럽트 벡터로 매핑
+ PIT: 주기적인 IRQ0 발생 → 커널 시간 기반 제공
+ 타이머 tick은 이후 스케줄링과 sleep의 기반이 됨

### Panic / Serial Output (디버깅 기반)
+ 커널은 크래시 발생 시 운영체제의 도움을 받을 수 없기 때문에, 자체적인 디버깅 수단이 필수적이다.
+ 본 프로젝트에서는:
    +Serial(COM1)을 이용한 로그 출력
    +치명적 오류 발생 시 panic()을 통한 시스템 정지
+ 를 구현하여,QEMU 환경에서 커널 내부 상태를 관찰할 수 있도록 구성하였다.

### #PF(Page Fault)
+ 페이징 변환/권한 위반 시 발생
+ CR2: fault가 난 선형주소(linear address) 저장 레지스터
+ err_code: not-present / write / user / reserved-bit / instruction-fetch 등의 원인 비트

### Multiboot Memory Map
+ GRUB이 제공하는 Multiboot 정보 구조체에서 물리 메모리 맵을 파싱
+ 사용 가능한 메모리 영역과 예약된 영역을 식별
+ 가장 큰 사용 가능한 메모리 영역을 찾아 힙 할당 준비
+ 커널 끝 주소(`__kernel_end`)를 기준으로 힙 영역 설정 가능

### Kernel Heap Allocator
+ Bump allocator 방식의 간단한 커널 힙 할당자 구현
+ Multiboot 메모리 맵에서 가장 큰 사용 가능한 영역을 찾아 힙으로 사용
+ 커널 이미지 끝(`__kernel_end`) 이후부터 힙 영역 시작
+ `kmalloc(size)`: 16바이트 정렬 기본 할당
+ `kmalloc_aligned(size, align)`: 사용자 지정 정렬 할당
+ 메모리 부족 시 OOM(Out Of Memory) 감지 및 패닉
+ Phase 1에서는 단순한 bump allocator로, Phase 2에서 free() 지원 예정

### Unified Logging System
+ 모든 로그 출력을 kprintf로 통일
+ 일반 로그: kprintf() 사용 (VGA + Serial 동시 출력)
+ 심각한 오류(panic, 예외): kprintf_puts_at()으로 화면 표시 + kprintf()로 상세 로그
+ IRQ/예외 중첩 안전성: kprintf 내부 lock/unlock 메커니즘
+ 디버깅 생산성 향상: 포맷 문자열 지원으로 일관된 로그 형식
+ 모든 출력 경로가 kprintf로 통일됨

### Time Management and sleep(ms)
+ PIT 기반 monotonic tick 카운터 구현
+ `timer_ticks()`: 현재 tick 값 반환 (64-bit, 오버플로우 안전)
+ `time_set_hz()`: PIT 주파수 설정 (pit_init 이후 호출 필요)
+ `sleep_ms(ms)`: 밀리초 단위 대기 함수 (busy-wait 방식)
+ Phase 1에서는 간단한 busy-wait 구현, Phase 2에서 인터럽트 기반 개선 예정
+ 32비트 나눗셈만 사용하여 freestanding 환경 호환성 보장
+ hlt 명령으로 CPU 전력 소비 최소화

## Build & Run
### Requirements (WSL/Ubuntu)
+ gcc (i386 freestanding build)
+ nasm
+ grub-mkrescue
+ qemu-system-i386

### Install (Ubuntu):
```
sudo apt update
sudo apt install -y build-essential gcc-multilib nasm grub-pc-bin xorriso qemu-system-x86
```

### Build ISO
```
make
```

### Run on QEMU
```
make run
```

### Run with logs / debugging options
+ Serial output is routed to host console via QEMU -serial stdio.
+ For GDB:
    + Start QEMU paused: add -s -S options.
    + Connect: gdb build/myos.bin then target remote :1234

### Notes
+ This project targets 32-bit protected mode (i386).
+ The kernel is built as a freestanding binary and packaged into a bootable ISO via GRUB.