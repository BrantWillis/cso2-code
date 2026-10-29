#include "tlb.h"
//#include "mlpt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#define WAYS 4
#define SETS 16

#define OFFSET_MASK ((1 << POBITS) - 1)

typedef struct {
    int tag;
    int valid;
    uint32_t pa;
    int lru_status;
} way;

typedef struct {
    way ways[WAYS];
    int lru_idx;
} set;

set cache[SETS];

size_t translate(size_t va) {
    if (va < 0x1234000)
        return va + 0x20000;
    else if (va > 0x2000000 && va < 0x2345000)
        return va + 0x100000;
    else
        return -1;
}

/** invalidate all cache lines in the TLB */
void tlb_clear() {
    for (int i = 0; i < SETS; i += 1) {
        for(int j = 0; j < WAYS; j += 1) {
            cache[i].ways[j].valid = 0;
        }
    }
}

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va) {
    uint32_t va_offset = va & OFFSET_MASK;
    uint32_t other_bits = (va & ~OFFSET_MASK) >> POBITS;
    uint32_t va_index = other_bits & (SETS - 1);
    uint32_t va_tag = (other_bits & ~(SETS - 1)) >> (int)log2(SETS);

    //printf("off %x, other %x, idx %x, tag %x \n", va_offset, other_bits, va_index, va_tag);
    
    for(int i = 0; i < WAYS; i += 1) {
        way curr_way = cache[va_index].ways[i];
        if(curr_way.valid == 1 && curr_way.tag == va_tag) return curr_way.lru_status;
    }

    return 0;
}

void update_lru(int va_idx, int way_idx) {
    int count = 0;
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid == 1) count += 1;
    }

   printf("set %d before (count %d) \n", 1, count);
    for(int j = 0; j < WAYS; j += 1) {
        if(cache[1].ways[j].valid) {
            printf("%x-(%d) ", cache[1].ways[j].pa, cache[1].ways[j].lru_status);
        }
        else printf("inv ");
    }
    puts("");

   int prev_status = cache[va_idx].ways[way_idx].lru_status;
   printf("prev stat %d\n", prev_status);

   for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid == 1) {
            int stat = cache[va_idx].ways[i].lru_status;
            if(stat < prev_status || stat < count) {
                cache[va_idx].ways[i].lru_status += 1;
            }
        }
   }
   cache[va_idx].ways[way_idx].lru_status = 1;

   printf("set %d after \n", 1);
    for(int j = 0; j < WAYS; j += 1) {
        if(cache[1].ways[j].valid) {
            printf("%x-(%d) ", cache[1].ways[j].pa, cache[1].ways[j].lru_status);
        }
        else printf("inv ");
    }
    puts("");puts("");
}

int find_lru(int va_idx) {
    /*int valid_count = 0;
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid) valid_count += 1;
    }*/
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid && cache[va_idx].ways[i].lru_status == 4) return i;
    }
}

int find_spot(int va_idx) {
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid == 0) return i;
    }
    return find_lru(va_idx);
}

/**
 * If this virtual address is in the TLB, return its
 * corresponding physical address. If not, use
 * `translate(va)` to find that address, store the result
 * in the TLB, and return it. In either case, make its
 * cache line the most-recently used in its set.
 *
 * As an exception, if translate(va) returns -1, do not
 * update the TLB: just return -1.
 */
size_t tlb_translate(size_t va) {

    uint32_t va_offset = va & OFFSET_MASK;
    uint32_t va_base = va & ~OFFSET_MASK;
    uint32_t other_bits = va_base >> POBITS;
    uint32_t va_index = other_bits & (SETS - 1);
    uint32_t va_tag = (other_bits & ~(SETS - 1)) >> (int)log2(SETS);

    //printf("off %x, other %x, idx %x, tag %x \n", va_offset, other_bits, va_index, va_tag);
    
    for(int i = 0; i < WAYS; i += 1) {
        way curr_way = cache[va_index].ways[i];

        //hit
        if(curr_way.valid == 1 && curr_way.tag == va_tag) {
            update_lru(va_index, i);
            return curr_way.pa | va_offset;
        };
    }

    //miss conditions
    int pa_base = translate(va_base);
    if (pa_base == -1) return -1;

    int addr = pa_base | va_offset;

    int found_index = find_spot(va_index);
    cache[va_index].ways[found_index] = (way){
        .tag = va_tag,
        .valid = 1,
        .pa = pa_base,
        .lru_status = 1
    };
    update_lru(va_index, found_index);

    return addr;
}

void print_all_sets() {
    for(int i = 0; i < SETS; i += 1) {
        printf("set %d \n", i);
        for(int j = 0; j < WAYS; j += 1) {
            if(cache[i].ways[j].valid) {
                printf("%x-(%d) ", cache[i].ways[j].pa, cache[i].ways[j].lru_status);
            }
            else printf("inv ");
        }
        puts("");
    }
}

int main(int argc, char *args[]) {
    tlb_peek(0x665a123);

    tlb_clear();
    assert(tlb_peek(0) == 0);
    assert(tlb_translate(0) == 0x0020000);
    assert(tlb_peek(0) == 1);
    assert(tlb_translate(0x200) == 0x20200);
    assert(tlb_translate(0x400) == 0x20400);
    assert(tlb_peek(0) == 1);
    assert(tlb_peek(0x200) == 1);
    assert(tlb_translate(0x2001200) == 0x2101200);
    assert(tlb_translate(0x0005200) == 0x0025200);
    assert(tlb_translate(0x0008200) == 0x0028200);
    assert(tlb_translate(0x0002200) == 0x0022200);
    assert(tlb_peek(0x2001000) == 1);
    assert(tlb_peek(0x0001000) == 0);
    assert(tlb_peek(0x0004000) == 0);
    assert(tlb_peek(0x0005000) == 1);
    assert(tlb_peek(0x0008000) == 1);
    assert(tlb_peek(0x0002000) == 1);
    assert(tlb_peek(0x0000000) == 1);
    tlb_clear();
    assert(tlb_peek(0x2001000) == 0);
    assert(tlb_peek(0x0005000) == 0);
    assert(tlb_peek(0x0008000) == 0);
    assert(tlb_peek(0x0002000) == 0);
    assert(tlb_peek(0x0000000) == 0);
    assert(tlb_translate(0) == 0x20000);
    assert(tlb_peek(0) == 1);





    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0501200) == 0x0521200);
    assert(tlb_translate(0x0A01200) == 0x0A21200);
    assert(tlb_peek(0x0001200) == 0);
    assert(tlb_peek(0x2101200) == 0);
    assert(tlb_peek(0x2301200) == 3);
    assert(tlb_peek(0x0501200) == 2);
    assert(tlb_peek(0x0801200) == 4);
    assert(tlb_peek(0x0A01200) == 1);
    assert(tlb_translate(0x2301800) == 0x2401800);
    assert(tlb_peek(0x0001000) == 0);
    assert(tlb_peek(0x2101000) == 0);
    assert(tlb_peek(0x2301000) == 1);
    assert(tlb_peek(0x0501000) == 3);
    assert(tlb_peek(0x0801000) == 4);
    assert(tlb_peek(0x0A01000) == 2);
    for (int i = 0; i < 16; i += 1) {
        if (i != 1) {
            assert(tlb_translate(0x400000 + (i << 12)) == (0x420000 + (i << 12)));
            assert(tlb_translate(0x500000 + (i << 12)) == (0x520000 + (i << 12)));
            assert(tlb_translate(0x600000 + (i << 12)) == (0x620000 + (i << 12)));
            assert(tlb_translate(0x700000 + (i << 12)) == (0x720000 + (i << 12)));
            assert(tlb_translate(0x800000 + (i << 12)) == (0x820000 + (i << 12)));
            assert(tlb_peek(0x800000 + (i << 12)) == 1);
            assert(tlb_peek(0x400000 + (i << 12)) == 0);
            assert(tlb_peek(0x0001000) == 0);
            assert(tlb_peek(0x2101000) == 0);
            assert(tlb_peek(0x2301000) == 1);
            assert(tlb_peek(0x0501000) == 3);
            assert(tlb_peek(0x0801000) == 4);
            assert(tlb_peek(0x0A01000) == 2);
        }
    }
    tlb_clear();
    assert(tlb_peek(0x301000) == 0);
    assert(tlb_peek(0x501000) == 0);
    assert(tlb_peek(0x801000) == 0);
    assert(tlb_peek(0xA01000) == 0);
    assert(tlb_translate(0xA01200) == 0xA21200);

    tlb_clear();
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_peek(0xA0001000) == 0);
    assert(tlb_translate(0x1200) == 0x21200);
    assert(tlb_peek(0xA0001200) == 0);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_translate(0xB0001200) == -1);
    assert(tlb_translate(0xC0001200) == -1);
    assert(tlb_translate(0xD0001200) == -1);
    assert(tlb_translate(0xE0001200) == -1);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0x1200) == 0x21200);

    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    tlb_clear();
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0011200) == 0x0031200);
    assert(tlb_peek(0x0001200) == 4);
    assert(tlb_peek(0x2101200) == 3);
    assert(tlb_peek(0x2301200) == 2);
    assert(tlb_peek(0x0011200) == 1);

    return 0;
}