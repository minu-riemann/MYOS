# MYOS

A small educational x86 (i386) OS kernel project built from scratch.
Boots via GRUB (Multiboot), runs on QEMU, and provides a debuggable kernel platform for further RTOS/Linux-track development.


## Current Status

- [x] GRUB Multiboot bootable kernel
- [x] VGA text output (hello world)
- [x] Serial debug output (COM1)
- [x] Panic / assert 기반 정지 루틴
- [x] GDT (Null / Kernel Code / Kernel Data)
- [x] IDT + CPU exception handling (validated with `ud2` → #UD)

> Verified: `ud2` triggers #UD, ISR handler prints exception context and halts via panic.



## Development Roadmap

### Phase 1. Kernel Platform 안정화 (필수)
**목표: “디버깅 가능한 커널 플랫폼” 확보**
- Serial 로그(COM1) + panic + assert
- GDB 연결(QEMU `-s -S`) + 심볼(ELF) 유지
- IDT + 예외 처리(특히 #PF 페이지폴트)
- PIC 리맵 + IRQ 처리 기반
- PIT 타이머 tick

### Phase 2. RTOS 트랙 (MCU/자동차/실무 직결)
**목표: “FreeRTOS급 기본 커널 기능을 내 손으로”**
- 프리엠티브 스케줄러(우선순위 기반)
- 태스크 생성/삭제, 스택 관리
- SysTick 역할(PIT) 기반 time slicing
- IPC(세마포어/뮤텍스/큐) 최소 2개
- 타이머(소프트 타이머) + 지연(sleep(ms))

### Phase 3. Linux 트랙 (유저모드/프로세스/시스템콜)
**목표: “Linux-like 구조를 갖춘 미니 OS”**
- Paging 정착 + 커널 주소공간 설계
- 프로세스(주소공간 분리) + 컨텍스트 스위치
- 시스템콜 엔트리 (`int 0x80` 또는 `sysenter`)
- ELF 로더(유저 프로그램 실행)
- 간단 FS (RAMFS → FAT/EXT2 read-only)



## Project Structure

```text
boot/
  entry.asm            # 커널 엔트리 + 스택 설정 + C로 점프

arch/x86/
  gdt.c, gdt.h         # GDT 구성(Null/Code/Data)
  gdt_flush.asm        # lgdt + 세그먼트 재로딩
  idt.c, idt.h         # IDT 구성 및 로딩(lidt)
  isr.c, isr.h         # 예외 디스패치/핸들러
  isr_stub.asm         # ISR 스텁(ASM) → C 핸들러 브리지

drivers/
  serial/
    serial.c, serial.h # COM1(0x3F8) 기반 시리얼 로깅

kernel/
  kernel.c             # kernel_main()
  vga.c, vga.h         # VGA 텍스트 출력
  panic/
    panic.c, panic.h   # panic 루틴

linker.ld              # 링커 스크립트(메모리 레이아웃)
Makefile               # 빌드/ISO/QEMU 실행

```

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