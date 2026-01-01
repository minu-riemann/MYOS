# MYOS

A small educational x86 (i386) OS kernel project built from scratch.

Boots via GRUB (Multiboot), runs on QEMU, and provides a **debuggable kernel platform**
designed as a foundation for both **RTOS-style kernels** and **Linux-like operating systems**.


## Current Status

- [x] GRUB Multiboot bootable kernel
- [x] VGA text output
- [x] Serial debug output (COM1)
- [x] Panic / assert 기반 커널 정지 루틴
- [x] GDT (Null / Kernel Code / Kernel Data)
- [x] IDT + CPU exception handling
- [x] PIC remap + IRQ handling
- [x] PIT timer interrupt (tick verified)

> Verified:
> - `ud2` triggers #UD → ISR handler prints CPU context and halts via panic  
> - PIT IRQ0 generates periodic ticks (`[TICK] 100 ticks`) after `sti`


## Development Roadmap

### Phase 1. Kernel Platform 안정화 (필수)
**목표: “디버깅 가능한 커널 플랫폼” 확보**
- Serial 로그(COM1) + panic + assert
- GDB 연결(QEMU `-s -S`) + ELF 심볼 유지
- IDT + 예외 처리 (#DE, #UD, #PF 등)
- PIC 리맵 + IRQ 처리 기반
- PIT 타이머 tick (시간 기반)

### Phase 2. RTOS Track (MCU / Automotive / Embedded)
**목표: “FreeRTOS급 커널을 직접 구현”**
- 프리엠티브 스케줄러 (우선순위 기반)
- 태스크 생성/삭제 및 스택 관리
- PIT 기반 time slicing
- IPC (세마포어 / 뮤텍스 / 큐 중 최소 2종)
- 소프트 타이머 + sleep(ms)

### Phase 3. Linux Track (Process / Syscall / User mode)
**목표: “Linux-like 구조를 갖춘 미니 OS”**
- Paging 정착 + 커널 주소공간 설계
- 프로세스 (주소공간 분리) + 컨텍스트 스위치
- 시스템콜 엔트리 (`int 0x80` 또는 `sysenter`)
- ELF 로더 (유저 프로그램 실행)
- 간단한 파일시스템 (RAMFS → FAT / EXT2 read-only)


## Project Structure

```text
boot/
  entry.asm                # 커널 엔트리, 스택 설정, C 진입

arch/x86/
  cpu/
    gdt.c, gdt.h           # GDT 구성 (Null / Code / Data)
    gdt_flush.asm          # lgdt + 세그먼트 리로드

  interrupt/
    idt.c, idt.h           # IDT 구성 및 로딩
    isr.c, isr.h           # 예외/IRQ 디스패치
    isr_stub.asm           # ASM ISR stub → C 핸들러 브리지
    pic.c, pic.h           # PIC 리맵 및 EOI
    pit.c, pit.h           # PIT 타이머 (IRQ0)

  io/
    ports.h                # inb/outb/io_wait 공통 I/O

drivers/
  serial/
    serial.c, serial.h     # COM1 (0x3F8) 시리얼 디버그 출력

kernel/
  kernel.c                 # kernel_main()
  vga.c, vga.h             # VGA 텍스트 출력
  panic/
    panic.c, panic.h       # panic 루틴

linker.ld                  # 링커 스크립트 (메모리 레이아웃)
Makefile                   # 빌드 / ISO 생성 / QEMU 실행

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