#pragma once
#include <stdint.h>

static inline int nvn_gpu_range_offset(uint64_t base, uint64_t size,
                                       uint64_t address, uint64_t length, uint64_t *offset) {
    if (address < base) return 0;
    uint64_t delta = address - base;
    if (delta >= size || length > size - delta) return 0;
    *offset = delta;
    return 1;
}
