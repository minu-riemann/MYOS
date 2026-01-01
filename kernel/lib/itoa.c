#include "itoa.h"

void u32_to_hex(uint32_t v, char out[11]) {
    static const char* hex = "0123456789ABCDEF";
    out[0] = '0';
    out[1] = 'x';
    for (int i = 0; i < 8; i++) {
        uint8_t nib = (v >> (28 - i * 4)) & 0xF;
        out[2 + i] = hex[nib];
    }
    out[10] = 0;
}