#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define ITERATIONS 1000000
#define ITERATIONS_SHORT 1000
#define TRIALS 20

long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

long long overhead_time(void) {
    long long start = nsecs();

    for (int i = 0; i < ITERATIONS; i++) {
        __asm__("");
    }

    return nsecs() - start;
}

__attribute__((noinline)) void empty_function(void) {
    __asm__("");
}

void scenario1(void) {
    long long sum = 0;

    for (int i = 0; i < TRIALS; i++) {
        long long overhead = overhead_time();

        //measuring 1 million calls to empty function
        long long start = nsecs();
        for (int j = 0; j < ITERATIONS; j++) {
            empty_function();
        }
        long long elapsed = nsecs() - start;
        printf("%lld\n", elapsed - overhead);

        sum += elapsed - overhead;
    }

    printf("avg: %lld\n", (sum / TRIALS));
}

void scenario2(void) {
    volatile double x;
    long long sum = 0;

    for (int i = 0; i < TRIALS; i++) {
        long long overhead = overhead_time();

        //measuring 1 million calls to drand48
        long long start = nsecs();
        for (int j = 0; j < ITERATIONS; j++) {
            x = drand48();
        }
        long long elapsed = nsecs() - start;
        printf("%lld\n", elapsed - overhead);

        sum += elapsed - overhead;
    }

    printf("avg: %lld\n", (sum / TRIALS));
}

void scenario3(void) {
    volatile pid_t x;
    long long sum = 0;

    for (int i = 0; i < TRIALS; i++) {
        long long overhead = overhead_time();

        //measuring 1 million calls to getppid
        long long start = nsecs();
        for (int i = 0; i < ITERATIONS; i++)
            x = getppid();
        long long elapsed = nsecs() - start;
        printf("%lld\n", elapsed - overhead);

        sum += elapsed - overhead;
    }

    printf("avg: %lld\n", (sum / TRIALS));
}

//no overhead calculation after this
void scenario4(void) {
    long long sum = 0;

    for (int i = 0; i < ITERATIONS_SHORT; i++) {
        //measuring only fork action
        long long start = nsecs();
        pid_t p = fork();
        long long elapsed = nsecs() - start;

        if (p == 0) {
            _exit(0);
        }

        waitpid(p, NULL, 0);
        sum += elapsed;
        printf("%lld\n", elapsed);
    }

    printf("avg: %lld\n", sum / ITERATIONS_SHORT);
}

void scenario5(void) {
    long long sum = 0;

    for (int i = 0; i < ITERATIONS_SHORT; i++) {
        pid_t p = fork();

        if (p == 0) {
            _exit(0);
        }

        struct timespec ts = {0, 5000000};
        nanosleep(&ts, NULL);

        //measuring only waitpid
        long long start = nsecs();
        waitpid(p, NULL, 0);
        long long elapsed = nsecs() - start;
        sum += elapsed;

        printf("%lld\n", elapsed);
    }

    printf("avg: %lld\n", sum / ITERATIONS_SHORT);
}

void scenario6(void) {
    long long sum = 0;

    for (int i = 0; i < ITERATIONS_SHORT; i++) {
        //measuring whole process creation and end
        long long start = nsecs();

        pid_t p = fork();

        if (p == 0) {
            _exit(0);
        }

        waitpid(p, NULL, 0);

        long long elapsed = nsecs() - start;
        sum += elapsed;
        printf("%lld\n", elapsed);
    }

    printf("avg: %lld\n", sum / ITERATIONS_SHORT);
}

void scenario7(void) {
    long long sum = 0;

    for (int i = 0; i < ITERATIONS_SHORT; i++) {
        //measuring system call
        long long start = nsecs();

        system("/bin/true");

        long long elapsed = nsecs() - start;
        sum += elapsed;
        printf("%lld\n", elapsed);
    }

    printf("avg: %lld\n", sum / ITERATIONS_SHORT);
}

void scenario8(void) {
    long long sum = 0;

    for (int i = 0; i < ITERATIONS_SHORT; i++) {
        //measuring directory add/removal
        long long start = nsecs();

        mkdir("/tmp/timing", 0700);
        rmdir("/tmp/timing");

        long long elapsed = nsecs() - start;
        printf("%lld\n", elapsed);
        sum += elapsed;
    }

    printf("avg: %lld\n", sum / ITERATIONS_SHORT);
}

int main(int argc, char *argv[]) {
    int scenario = atoi(argv[1]);

    switch (scenario) {
        case 1:
            scenario1();
            break;
        case 2:
            scenario2();
            break;
        case 3:
            scenario3();
            break;
        case 4:
            scenario4();
            break;
        case 5:
            scenario5();
            break;
        case 6:
            scenario6();
            break;
        case 7:
            scenario7();
            break;
        case 8:
            scenario8();
            break;
    }

    return 0;
}