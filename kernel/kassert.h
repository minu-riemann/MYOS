#pragma once
#include "panic/panic.h"

#define KAASSERT(cond, msg)               \
    do {                                 \
        if (!(cond)) {                   \
            panic(msg);                  \
        }                                \
    } while (0)