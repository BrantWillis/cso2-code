#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **string_split(const char *input, const char *sep, int *num_words) {
    char **output = NULL;
    char *input_copy[100];
    strcpy(input, input_copy);
    
    char *substring = malloc(sizeof(char) * 5);
    //*num_words += 1;

    int sep_length = 1;
    int input_length = 5;

    int first_comp_flag = 0;

    int i = 0;
    int j = 0;

    char *temp = malloc(sizeof(char));

    while (strcmp(input_copy[i], '\0') != 0) {
        if (strcmp(input_copy[i], sep[0]) != 0) {
            first_comp_flag = 0;
            temp = realloc(temp, sizeof(temp) + sizeof(char));
            strcat(temp, input_copy[i]);
        }
        else if (first_comp_flag == 0) {
            first_comp_flag = 1;
            output[j] = malloc(sizeof(temp));
            strcpy(output[0], temp);
            free(temp);
            temp = malloc(sizeof(char));
            j += 1;
            *num_words += 1;
        }
        i += 1;
    }


}