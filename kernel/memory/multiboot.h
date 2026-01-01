#pragma once
#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

typedef struct multiboot_info {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;
    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    uint32_t syms[4]; // a.out or ELF sections header (unused here)

    uint32_t mmap_length;
    uint32_t mmap_addr;

    // The rest exists in spec but not used in this phase

} __attribute__((packed)) multiboot_info_t;

typedef struct multiboot_mmap_entry {
    uint32_t size; // size of the entry excludeing this field
    uint64_t addr; // base address
    uint64_t len; // length
    uint32_t type; // 1=usualbe, other reserved
} __attribute__((packed)) multiboot_mmap_entry_t;

void multiboot_dump_memory_map(uint32_t mb_addr);
int multiboot_find_largest_usable(uint32_t mb_addr, uint32_t* out_base, uint32_t* out_end);
