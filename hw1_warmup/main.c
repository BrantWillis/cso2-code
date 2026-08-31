//#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char buffer[5000];

    char sep[100];

    int count = 1;
    while(count < argc) {
        strcat(sep, argv[count]);
        count += 1;
    }

    if (count == 1) {
        strcpy(sep, " \t");
    }

    puts(sep);

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        puts(buffer);

        //printf("%d %d\n", strncmp(buffer, "."), strcmp(buffer, "what"));
        if(strncmp(buffer, ".", 1) == 0) break;

        int *num;
        //char **result = string_split(buffer, sep, num);
        //then print **result
    }

    return 0;
}