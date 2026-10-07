#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>

#include "config.h"
#include "mlpt.h"

alignas(4096)
static size_t testing_page_table[512];

alignas(4096)
static char data_for_page_3[4096];

alignas(4096)
static char data_for_page_10[4096];

static void reset_page_table(void) {
    size_t index;

    for (index = 0; index < 512; index += 1) {
        testing_page_table[index] = 0;
    }

    ptbr = (size_t) &testing_page_table[0];
}

static size_t make_pte(void *page) {
    size_t page_address;

    page_address = (size_t) page;

    return (page_address & ~((1UL << POBITS) - 1)) | 1;
}

int main(void) {
    size_t invalid_address;
    size_t data_page_3_address;
    size_t data_page_10_address;

    printf("Testing translate()...\n");

    /*
     * Test 1: ptbr should initially be zero.
     */
    ptbr = 0;

    assert(ptbr == 0);

    /*
     * With no page table, every address should be invalid.
     */
    invalid_address = translate(0x0000);
    assert(invalid_address == ~(size_t) 0);

    invalid_address = translate(0x1234);
    assert(invalid_address == ~(size_t) 0);

    invalid_address = translate(0xABCDEF);
    assert(invalid_address == ~(size_t) 0);

    printf("Test 1 passed: ptbr == 0\n");

    /*
     * Test 2: create an empty page table.
     */
    reset_page_table();

    assert(ptbr != 0);

    /*
     * Every PTE should initially be invalid.
     */
    assert(translate(0x0000) == ~(size_t) 0);
    assert(translate(0x1000) == ~(size_t) 0);
    assert(translate(0x2000) == ~(size_t) 0);
    assert(translate(0x3000) == ~(size_t) 0);
    assert(translate(0x12345000) == ~(size_t) 0);

    printf("Test 2 passed: empty page table\n");

    /*
     * Test 3: map virtual page 3 to data_for_page_3.
     */
    data_page_3_address = (size_t) &data_for_page_3[0];

    testing_page_table[3] = make_pte(&data_for_page_3[0]);

    /*
     * 0x3000 is the beginning of virtual page 3.
     */
    assert(translate(0x3000) == data_page_3_address);

    printf("Test 3 passed: basic translation\n");

    /*
     * Test 4: make sure the page offset is preserved.
     */
    assert(translate(0x3001) == data_page_3_address + 0x1);
    assert(translate(0x3045) == data_page_3_address + 0x45);
    assert(translate(0x3123) == data_page_3_address + 0x123);
    assert(translate(0x3ABC) == data_page_3_address + 0xABC);
    assert(translate(0x3FFF) == data_page_3_address + 0xFFF);

    printf("Test 4 passed: page offsets preserved\n");

    /*
     * Test 5: map another virtual page to a different physical page.
     */
    data_page_10_address = (size_t) &data_for_page_10[0];

    testing_page_table[10] = make_pte(&data_for_page_10[0]);

    /*
     * Virtual page 10 begins at:
     *
     * 10 * 4096 = 0xA000
     */
    assert(translate(0xA000) == data_page_10_address);
    assert(translate(0xA001) == data_page_10_address + 0x1);
    assert(translate(0xA123) == data_page_10_address + 0x123);
    assert(translate(0xAFFF) == data_page_10_address + 0xFFF);

    printf("Test 5 passed: multiple mapped pages\n");

    /*
     * Test 6: make sure an unmapped page is still invalid.
     */
    assert(translate(0x0000) == ~(size_t) 0);
    assert(translate(0x1000) == ~(size_t) 0);
    assert(translate(0x2000) == ~(size_t) 0);
    assert(translate(0x4000) == ~(size_t) 0);
    assert(translate(0x9000) == ~(size_t) 0);

    printf("Test 6 passed: unmapped pages\n");

    /*
     * Test 7: test addresses around the boundary between
     * virtual page 3 and virtual page 4.
     */
    assert(translate(0x3FFE) == data_page_3_address + 0xFFE);
    assert(translate(0x3FFF) == data_page_3_address + 0xFFF);
    assert(translate(0x4000) == ~(size_t) 0);
    assert(translate(0x4001) == ~(size_t) 0);

    printf("Test 7 passed: page boundary\n");

    /*
     * Test 8: make sure the valid bit is actually checked.
     *
     * Store a physical address with valid bit 0.
     */
    testing_page_table[20] = data_page_3_address;

    assert((testing_page_table[20] & 1) == 0);
    assert(translate(0x14000) == ~(size_t) 0);

    printf("Test 8 passed: invalid PTE handling\n");

    printf("\nAll translate() tests passed!\n");

    return 0;
}