.section .text
.global _start

_start:
    // 스택 설정 (임시 주소)
    ldr sp, =0x8000
    
    // 인터럽트 비활성화
    cpsid i
    
    // C 함수 호출을 위한 준비
    // r0, r1에 부팅 파라미터 전달 (나중에 확장)
    mov r0, #0
    mov r1, #0
    
    // kernel_main 호출 (extern 선언 필요)
    bl kernel_main
    
    // 무한 루프
hang:
    wfi
    b hang