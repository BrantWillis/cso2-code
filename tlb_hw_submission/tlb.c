#include "tlb.h"
#include "mlpt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#define WAYS 4
#define SETS 16

#define OFFSET_MASK ((size_t)((1 << POBITS) - 1))

typedef struct {
    int tag;
    int valid;
    size_t pa;
    int lru_status;
} way;

typedef struct {
    way ways[WAYS];
} set;

set cache[SETS];

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
    size_t va_offset = va & OFFSET_MASK;
    size_t other_bits = (va & ~OFFSET_MASK) >> POBITS;
    size_t va_index = other_bits & (SETS - 1);
    size_t va_tag = (other_bits & ~(SETS - 1)) >> (int)log2(SETS);
    
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

    //previous status of way being accessed
   int prev_status = cache[va_idx].ways[way_idx].lru_status;

    //increment all statuses if they're less than the previous, stopping if full
   for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid == 1) {
            int stat = cache[va_idx].ways[i].lru_status;
            if(stat < prev_status || stat < count) {
                cache[va_idx].ways[i].lru_status += 1;
            }
        }
   }

   //end by setting most recently accessed to 1
   cache[va_idx].ways[way_idx].lru_status = 1;
}

int find_lru(int va_idx) {
    //finds the least recently used
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid && cache[va_idx].ways[i].lru_status == 4) return i;
    }
}

int find_spot(int va_idx) {
    //take first open spot
    for(int i = 0; i < WAYS; i += 1) {
        if(cache[va_idx].ways[i].valid == 0) return i;
    }
    //if full, find lru
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
    size_t va_offset = va & OFFSET_MASK;
    size_t va_base = va & ~OFFSET_MASK;
    size_t other_bits = va_base >> POBITS;
    size_t va_index = other_bits & (SETS - 1);
    size_t va_tag = (other_bits & ~(SETS - 1)) >> (int)log2(SETS);
    
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

    //put new way into open or least used spot
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