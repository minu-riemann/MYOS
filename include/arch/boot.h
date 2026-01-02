#pragma once
#include <stdint.h>

/**
 * 부팅 정보 구조체 (아키텍처별로 다름)
 */
typedef struct arch_boot_info {
    uint32_t magic;         // 부팅 매직 넘버
    void* info_ptr;         // 부팅 정보 포인터
    // 아키텍처별 확장 필드는 구현 시 추가
} arch_boot_info_t;

/**
 * 부팅 정보 파싱
 * - x86: Multiboot 정보 파싱
 * - ARM: Device Tree 또는 ATAG 파싱
 * @param boot_param 부팅 파라미터 (x86: eax, ARM: r0/r1)
 * @param out 파싱된 부팅 정보를 저장할 구조체
 * @return 0: 성공, -1: 실패
 */
int arch_boot_parse_info(uint32_t boot_param, arch_boot_info_t* out);

/**
 * 메모리 맵 정보 획득
 * - x86: Multiboot memory map
 * - ARM: Device Tree 또는 하드코딩
 * @param boot_info 부팅 정보 구조체
 * @param out_map 메모리 맵을 저장할 버퍼
 * @param out_size 메모리 맵 크기 (입력: 버퍼 크기, 출력: 실제 크기)
 */
void arch_boot_get_memory_map(arch_boot_info_t* boot_info, void* out_map, size_t* out_size);