#pragma once
#include <stdint.h>
#include <stddef.h>

void heap_init(uint32_t heap_start, uint32_t heap_end);
void* kmalloc(size_t size);
void* kmalloc_aligned(size_t size, uint32_t align);

uint32_t heap_used(void);
uint32_t heap_free(void);
uint32_t heap_start_addr(void);
uint32_t heap_end_addr(void);

