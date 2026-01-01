#include "heap.h"
#include "../panic/panic.h"
#include "../console/kprintf.h"

static uint32_t g_heap_start = 0;
static uint32_t g_heap_end   = 0;
static uint32_t g_heap_cur   = 0;

static inline uint32_t align_up(uint32_t v, uint32_t align) {
    if (align == 0) return v;
    uint32_t mask = align - 1;
    return (v + mask) & ~mask;
}

void heap_init(uint32_t heap_start, uint32_t heap_end) {
    // 기본 정렬
    heap_start = align_up(heap_start, 16);

    if (heap_end <= heap_start) {
        panic("heap_init: invalid range");
    }

    g_heap_start = heap_start;
    g_heap_end = heap_end;
    g_heap_cur = heap_start;

    kprintf("[HEAP] init\n");
    kprintf("  start=0x%x\n", g_heap_start);
    kprintf("  end  =0x%x\n", g_heap_end);
}

void *kmalloc(size_t size) {
    return kmalloc_aligned(size, 16);
}

void *kmalloc_aligned(size_t size, uint32_t align) {
    if (g_heap_start == 0) {
        panic("kmalloc: heap not initialized");
    }

    if (size == 0) {
        return (void*)0;
    }

    uint32_t cur = align_up(g_heap_cur, align);
    uint32_t next = cur + (uint32_t)size;

    // overflow + bounds check
    if (next < cur || next > g_heap_end) {
        kprintf("[HEAP] OOM\n");
        kprintf("  cur=0x%x\n", g_heap_cur);
        kprintf("  req=0x%x\n", (uint32_t)size);
        kprintf("  end=0x%x\n", g_heap_end);
        panic("kmalloc: out of memory");
    }

    g_heap_cur = next;
    return (void*)cur;
}

uint32_t heap_used(void) {
    if (g_heap_start == 0) return 0;
    return g_heap_cur - g_heap_start;
}

uint32_t heap_free(void) {
    if (g_heap_start == 0) return 0;
    return g_heap_end - g_heap_cur;
}

uint32_t heap_start_addr(void) { return g_heap_start; }
uint32_t heap_end_addr(void) { return g_heap_end; }


