#define _XOPEN_SOURCE 700

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;

#define PAGE_SIZE ((size_t) 1 << POBITS)
#define ENTRY_COUNT ((size_t) 1 << (POBITS - 3))
#define OFFSET_MASK (PAGE_SIZE - 1)

//helper function made static
static size_t index_for_level(size_t va, int level) {
    size_t index_bits = POBITS - 3;
    size_t shift = POBITS + index_bits * (LEVELS - 1 - level);

    // each level uses POBITS - 3 bits of the virtual address.
    return (va >> shift) & (ENTRY_COUNT - 1);
}

//helper function made static
static void *allocate_zeroed_page(void) {
    void *page = NULL;

    posix_memalign(&page, PAGE_SIZE, PAGE_SIZE);
    memset(page, 0, PAGE_SIZE);

    return page;
}

size_t translate(size_t va) {
    size_t *table;
    size_t pte;

    // unset base register returns 0xF...F
    if (ptbr == 0) {
        return ~(size_t) 0;
    }

    table = (size_t *) ptbr;

    for (int level = 0; level < LEVELS; level += 1) {
        size_t index = index_for_level(va, level);

        pte = table[index];

        // check validity bit
        if ((pte & 1) == 0) {
            return ~(size_t) 0;
        }

        // code review: should describe process
        // 
        if (level < LEVELS - 1) {
            table = (size_t *) (pte & ~OFFSET_MASK);
        }
    }

    return (pte & ~OFFSET_MASK) | (va & OFFSET_MASK);
}

int allocate_page(size_t start_va) {
    size_t *table;
    size_t pte;

    if ((start_va & OFFSET_MASK) != 0) {
        return -1;
    }

    if (ptbr == 0) {
        void *root = allocate_zeroed_page();
        ptbr = (size_t) root;
    }

    table = (size_t *) ptbr;

    for (int level = 0; level < LEVELS; level += 1) {
        size_t index = index_for_level(start_va, level);

        pte = table[index];

        if (level == LEVELS - 1) {
            if ((pte & 1) != 0) {
                return 0;
            }

            void *data_page = allocate_zeroed_page();
            table[index] = ((size_t) data_page) | 1;
            return 1;
        }

        // if valid, continue traversing
        if ((pte & 1) == 1) {
            table = (size_t *) (pte & ~OFFSET_MASK);
        }
        else {
            void *next_table = allocate_zeroed_page();
            table[index] = ((size_t) next_table) | 1;
            table = (size_t *) next_table;
        }
    }

    return -1;
}