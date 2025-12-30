BITS 32

; Multiboot header: GRUB가 “이 파일이 멀티부트 커널이다”라고 인식하게 하는 헤더
SECTION .multiboot
align 4
MULTIBOOT_MAGIC    equ 0x1BADB002
MULTIBOOT_FLAGS    equ 0x0
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM

SECTION .text
global start
extern kernel_main

; 커널의 진짜 시작점 (OS에는 일반적인 진입점(main함수)이 없음)
start:

    ; Interrupts Disable
    cli

    ; Set up stack
    mov esp, stack_top      ;esp: 스택 포인터 레지스터 설정 / 스택이 없으면 C함수 호출이 깨짐
    and esp, 0xFFFFFFF0     ; 16-byte alignment 보장 (권장)

    ; Call the kernel main function
    call kernel_main

.hang:
    cli
    hlt ; CPU 휴식 - 무한루프
    jmp .hang

SECTION .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:  