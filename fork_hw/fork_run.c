#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void writeoutput(const char *command, char *out_path, char *err_path) {
    pid_t pid = fork();

    if (pid == 0) {
        //open out and error paths, copy them to STD, then close
        int out_fd = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        int err_fd = open(err_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        dup2(out_fd, STDOUT_FILENO);
        dup2(err_fd, STDERR_FILENO);

        close(out_fd);
        close(err_fd);

        //run command
        execl("/bin/sh", "sh", "-c", command, NULL);

    } else {
        waitpid(pid, NULL, 0);
    }
}

void parallelwriteoutput(int count, const char **argv_base, const char *out_file) {
    //count arguments provided
    int argc = 0;
    while(argv_base[argc] != NULL) argc++;

    for(int i = 0; i < count; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            //open out path, copy to STDOUT, then close
            int out_fd = open(out_file, O_WRONLY | O_CREAT | O_APPEND, 0777);
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);

            //build argv for child execution
            char *argv[argc + 2];
            for(int j = 0; j < argc; j++) {
                argv[j] = argv_base[j];
            }

            //second to last index should be a string of the zero-indexed child number
            char index_string[50];
            snprintf(index_string, sizeof(index_string), "%d", i);
            argv[argc] = index_string;
            argv[argc + 1] = NULL;

            execv(argv_base[0], argv);
        }
    }

    //catch all children at the end
    for (int i = 0; i < count; i++) {
        waitpid(-1, NULL, 0);
    }
}