#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "../nvn/nvn_gpu_range.h"

int main(void) {
    uint64_t off = UINT64_MAX;
    assert(nvn_gpu_range_offset(0x1000, 0x100, 0x1040, 16, &off) && off == 0x40);
    assert(nvn_gpu_range_offset(0x1000, 0x100, 0x10ff, 1, &off) && off == 0xff);
    assert(!nvn_gpu_range_offset(0x1000, 0x100, 0x1100, 1, &off));
    assert(!nvn_gpu_range_offset(0x1000, 0x100, 0xfff, 1, &off));
    assert(!nvn_gpu_range_offset(0x1000, 0x100, 0x10f0, 17, &off));
    assert(!nvn_gpu_range_offset(0x1000, 0x100, 0x1040, UINT64_MAX, &off));
    assert(nvn_gpu_range_offset(UINT64_MAX - 15, 16, UINT64_MAX - 1, 2, &off) && off == 14);
    puts("GPU address bounds: passed");
}
