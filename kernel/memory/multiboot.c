#include "multiboot.h"
#include "../console/kprintf.h"
#include "../panic/panic.h"

void multiboot_dump_memory_map(uint32_t mb_addr) {
    multiboot_info_t* mb = (multiboot_info_t*)mb_addr;

    kprintf("[MMAP] Found =0x%x\n", mb->flags);

    // bit6: mmap_* fields are valid
    if ((mb->flags & (1 << 6)) == 0) {
        kprintf("[MB] mmap not available (flags bit6 not set)\n");
        return;
    }

    kprintf("[MB] mmap_addr=0x%x mmap_length=0x%x\n", mb->mmap_addr, mb->mmap_length);

    uint32_t mmap_end = mb->mmap_addr + mb->mmap_length;
    multiboot_mmap_entry_t* e = (multiboot_mmap_entry_t*)mb->mmap_addr;

    while ((uint32_t)e < mmap_end) {
        // 64-bit 주소를 출력하기 위해 상/하위 32비트 분리
        uint32_t addr_hi = (uint32_t)(e->addr >> 32);
        uint32_t addr_lo = (uint32_t)(e->addr & 0xFFFFFFFF);
        uint32_t len_hi = (uint32_t)(e->len >> 32);
        uint32_t len_lo = (uint32_t)(e->len & 0xFFFFFFFF);

        if (addr_hi == 0 && len_hi == 0) {
            // 32-bit 범위 내면 간단히 출력
            kprintf("  [MB] entry addr=0x%x len=0x%x type=0x%x %s\n",
                addr_lo, len_lo, e->type,
                e->type == 1 ? "(usable)" : "(reserved)");
        } else {
            // 64-bit 주소면 상/하위 분리 출력
            kprintf("  [MB] entry addr=0x%x%08x len=0x%x%08x type=0x%x %s\n",
                addr_hi, addr_lo, len_hi, len_lo, e->type,
                e->type == 1 ? "(usable)" : "(reserved)");
        }

        // advance: size field + entry body
        e = (multiboot_mmap_entry_t*)((uint32_t)e + e->size + sizeof(e->size));
    }
}

int multiboot_find_largest_usable(uint32_t mb_addr, uint32_t* out_base, uint32_t* out_end) {
    multiboot_info_t* mb = (multiboot_info_t*)mb_addr;

    if ((mb->flags & (1 << 6)) == 0) {
        return 0;
    }

    uint32_t mmap_end = mb->mmap_addr + mb->mmap_length;
    multiboot_mmap_entry_t* e = (multiboot_mmap_entry_t*)mb->mmap_addr;

    uint64_t best_len = 0;
    uint64_t best_base = 0;

    while ((uint32_t)e < mmap_end) {
        if (e->type == 1 && e->len > best_len) {
            best_len = e->len;
            best_base = e->addr;
        } 
        e = (multiboot_mmap_entry_t*)((uint32_t)e + e->size + sizeof(e->size));
    }

    if (best_len == 0) return 0;

    // Phase 1은 32-bit 커널이므로 4GB 미만 usable만 우선 사용
    if ((best_base >> 32) != 0) return 0;
    if (((best_base + best_len) >> 32) != 0 ) {
        best_len =(0x100000000ULL - best_base); // clampt to 4GB
    }

    *out_base = (uint32_t)best_base;
    *out_end = (uint32_t)(best_base + best_len);
    return 1;

}