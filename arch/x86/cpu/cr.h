#pragma once
#include <stdint.h>

/**
 * CR0 레지스터 읽기
 * - 페이징 활성화, 보호 모드 등 제어
 */
uint32_t read_cr0(void);

/**
 * CR0 레지스터 쓰기
 */
void write_cr0(uint32_t value);

/**
 * CR2 레지스터 읽기
 * - 페이지 폴트 발생 시 선형 주소 저장
 */
uint32_t read_cr2(void);

/**
 * CR3 레지스터 읽기
 * - 페이지 디렉토리 베이스 주소
 */
uint32_t read_cr3(void);

/**
 * CR3 레지스터 쓰기
 * - 페이지 디렉토리 베이스 주소 설정
 */
void write_cr3(uint32_t value);

/**
 * CR4 레지스터 읽기
 * - 추가 기능 제어 (PAE, PSE 등)
 */
uint32_t read_cr4(void);

/**
 * CR4 레지스터 쓰기
 */
void write_cr4(uint32_t value);