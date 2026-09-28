#include <stdio.h>
#include <stdlib.h>
#include "fork_run.h"

int main() {
    //printf("Hi!\n");
    //writeoutput("echo 1 2 3; sleep 2; echo 5 5", "out.txt", "err.txt");
    //printf("Bye!\n");

    // const char *argv_base[] = {
    //     "/bin/echo", "running", NULL
    // };
    // parallelwriteoutput(5, argv_base, "out.txt");

    //writeoutput("echo first; echo second; echo third", "out.txt", "err.txt");

    //writeoutput("echo stdout; echo stderr >&2", "out.txt", "err.txt");

    // printf("before\n");

    // writeoutput("echo child; sleep 3; echo child-done",
    //             "out.txt", "err.txt");

    // printf("after\n");

    // const char *argv_base[] = {
    //     "/bin/echo", NULL
    // };

    // parallelwriteoutput(5, argv_base, "out.txt");

    // const char *argv_base[] = {
    //     "/bin/echo", "a", "b", "c", "d", "e", NULL
    // };

    // parallelwriteoutput(3, argv_base, "out.txt");

    printf("before\n");

    const char *argv_base[] = {
        "/bin/echo", "child", NULL
    };

    parallelwriteoutput(2, argv_base, "out.txt");

    printf("after\n");

    /*printf("args = [");
    for (int i = 0; i < argc; i += 1)
        printf("'%s'%s", argv[i], i == argc - 1 ? "" : ", ");
    printf("]\n");*/
}