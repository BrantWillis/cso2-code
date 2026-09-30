#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>

alignas(4096) volatile char global_array[4096 * 32];

void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
    } else if (which == 1) {
        global_array[0] = 'h';
        global_array[1] = 'h';
        global_array[4096] = 'h';
    } else if (which == 2) {
        char *area = malloc(1000000);
        printf("%c", area[100]);
    } else if (which == 3) {
        //char *area2 = malloc(1048576 - 3520);

        char *ptr;
        ptr = mmap(NULL, 1048576, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        for (int i = 0; i < 131072; i += 4096) {
            global_array[i] = 'A';
        }
    } else if (which == 4) {
        char *ptr;
        ptr = mmap((void*) 0x000000467000 + 0x200000,
                4096,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1,
                    0
            );

        ptr[0] = 'B';
        printf("%c", ptr[0]);
    } else if (which == 5) {
        char *ptr;
        ptr = mmap((void*) 0x000000467000 + 0x10000000000,
                4096,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1,
                    0
            );

        ptr[0] = 'B';
        printf("%c", ptr[0]);
    }
}

int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
