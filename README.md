# 개발 방향

## Phase 1. Kernel Platform 안정화 (필수)
### 목표: “디버깅 가능한 커널 플랫폼” 확보
+ Serial 로그(COM1) + panic + assert
+ GDB 연결(QEMU -s -S) + 심볼(ELF) 유지
+ IDT + 예외 처리(특히 #PF 페이지폴트)
+ PIC 리맵 + IRQ 처리 기반
+ PIT 타이머 tick


## Phase 2. RTOS 트랙(MCU/자동차/실무에 직결)
### 목표: “FreeRTOS급 기본 커널 기능을 내 손으로”
+ 프리엠티브 스케줄러(우선순위 기반)
+ 태스크 생성/삭제, 스택 관리
+ SysTick 역할(PIT) 기반 time slicing
+ IPC(세마포어/뮤텍스/큐) 최소 2개
+ 타이머(소프트 타이머) + 지연(sleep(ms))


## Phase 3. Linux 트랙(유저모드/프로세스/시스템콜)
### 목표: “Linux-like 구조를 갖춘 미니 OS”
+ Paging 정착 + 커널 주소공간 설계
+ 프로세스(주소공간 분리) + 컨텍스트 스위치
+ 시스템콜 엔트리 (int 0x80 또는 sysenter)
+ ELF 로더(유저 프로그램 실행)
+ 간단 FS (처음엔 RAMFS → 이후 FAT/EXT2 read-only)


---

# 파일구조

## boot/
+ entry.asm : 커널 엔트리 + 스택 + c로 점프

## kernel/
+ kernel.c : kernel_main() 엔트리
+ vga.c, vga.h : 화면 출력(printf 대체)

## drivers/
+ serial/ serial.c, serial.h

##  linker.ld              
+ 커널이 메모리에 올라갈 위치 정의

##  Makefile               
+ 빌드/ISO/QEMU 실행 자동화

---

# 실행방법