#pragma once
#include <stdint.h>

void u32_to_hex(uint32_t v, char out[11]);   // "0x" + 8 hex + '\0' = 11