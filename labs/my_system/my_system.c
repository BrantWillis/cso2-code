#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int my_system(const char *command) {
    pid_t child_id = fork();
    int status;

    if(child_id == 0) {
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
    }
    else {
        wait(&status);
    }

    return status;
}