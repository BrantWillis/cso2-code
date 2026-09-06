#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    //initialize buffer and sep for function input
    char buffer[5001];
    char sep[5001] = "";

    //concatenate arguments, or set default sep
    int count = 1;
    while(count < argc) {
        strcat(sep, argv[count]);
        count += 1;
    }
    if (count == 1) {
        strcpy(sep, " \t");
    }

    while (1) {
        //read stdin to buffer
        fgets(buffer, sizeof(buffer), stdin);

        //if there is a newline, replace it with null terminator
        if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        //exit if input is "."
        if(strcmp(buffer, ".") == 0) break;

        //initialize variables and call function
        int amt = 0;
        char **result = string_split(buffer, sep, &amt);

        //print and free results
        for (int i = 0; i < amt; i++) {
            printf("[%s]", result[i]);
            free(result[i]);
        }
        free(result);
        printf("\n");
    }

    return 0;
}