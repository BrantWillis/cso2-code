#include <stddef.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;

size_t translate(size_t va) {
    size_t *page_table;
    size_t index;
    size_t offset;
    size_t pte;

    // unset base register returns 0xFFF...FFF
    if (ptbr == 0) {
        return ~(size_t) 0;
    }

    page_table = (size_t *) ptbr;

    // index goes from 0-511
    index = (va >> POBITS) & ((1UL << (POBITS - 3)) - 1);

    // offset is lower POBITS bits of va
    offset = va & ((1UL << POBITS) - 1);

    pte = page_table[index];

    // check validity bit
    if ((pte & 1) == 0) {
        return ~(size_t) 0;
    }

    return (pte & ~((1UL << POBITS) - 1)) | offset;
}