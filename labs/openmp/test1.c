#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    puts("I'm a thread");
    puts("non-threaded");
}
