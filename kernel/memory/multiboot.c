#include "multiboot.h"
#include "../../drivers/serial/serial.h"
#include "../lib/itoa.h"
#include "../panic/panic.h"

static void serial_hex32(uint32_t v) {
    char buf[11];
    u32_to_hex(v, buf);
    serial_write(buf);
}

static void serial_hex64(uint64_t v) {
    // Phase1은 32-bit 커널이므로 보통 4GB 미만만 쓰게 됩니다.
    // 그래도 mmap이 64-bit를 쓰므로 상/하를 나눠 출력.

    uint32_t hi = (uint32_t)(v >> 32);
    uint32_t lo = (uint32_t)(v & 0xFFFFFFFF);

    serial_write("0x");
    char buf[11];
    u32_to_hex(hi, buf); serial_write(buf+2); // "0x" 중복 제거용(유틸에 맞춰 조정 가능)
    u32_to_hex(lo, buf); serial_write(buf+2);
}

void multiboot_dump_memory_map(uint32_t mb_addr) {
    multiboot_info_t* mb = (multiboot_info_t*)mb_addr;

    serial_write("[MMAP] Found ="); serial_hex32(mb->flags); serial_write("\n");

    // bit6: mmap_* fields are valid
    if ((mb->flags & (1 << 6)) == 0) {
        serial_write("[MB] mmap not available (flags bit6 not set)\n");
        return;
    }

    serial_write("[MB] mmap_addr="); serial_hex32(mb->mmap_addr);
    serial_write(" mmap_length="); serial_hex32(mb->mmap_length);
    serial_write("\n");

    uint32_t mmap_end = mb->mmap_addr + mb->mmap_length;
    multiboot_mmap_entry_t* e = (multiboot_mmap_entry_t*)mb->mmap_addr;

    while ((uint32_t)e < mmap_end) {
        serial_write("  [MB] entry addr=");
        serial_hex64(e->addr);
        serial_write(" len="); 
        serial_hex64(e->len);
        serial_write(" type="); 
        serial_hex32(e->type);
        serial_write(e->type == 1 ? " (usable)\n" : " (reserved)\n");

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