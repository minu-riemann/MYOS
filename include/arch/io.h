#pragma once
#include <stdint.h>

/**
 * 8비트 I/O 읽기
 * - x86: 포트 I/O (inb)
 * - ARM: MMIO 직접 접근
 */
uint8_t arch_io_read8(uint32_t addr);

/**
 * 16비트 I/O 읽기
 * - x86: 포트 I/O (inw)
 * - ARM: MMIO 직접 접근
 */
uint16_t arch_io_read16(uint32_t addr);

/**
 * 32비트 I/O 읽기
 * - x86: 포트 I/O (inl)
 * - ARM: MMIO 직접 접근
 */
uint32_t arch_io_read32(uint32_t addr);

/**
 * 8비트 I/O 쓰기
 * - x86: 포트 I/O (outb)
 * - ARM: MMIO 직접 접근
 */
void arch_io_write8(uint32_t addr, uint8_t value);

/**
 * 16비트 I/O 쓰기
 * - x86: 포트 I/O (outw)
 * - ARM: MMIO 직접 접근
 */
void arch_io_write16(uint32_t addr, uint16_t value);

/**
 * 32비트 I/O 쓰기
 * - x86: 포트 I/O (outl)
 * - ARM: MMIO 직접 접근
 */
void arch_io_write32(uint32_t addr, uint32_t value);

/**
 * I/O 완료 대기
 * - x86: io_wait() (포트 0x80에 쓰기)
 * - ARM: 메모리 배리어 또는 nop
 */
void arch_io_wait(void);