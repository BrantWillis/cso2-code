#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **string_split(const char *input, const char *sep, int *num_words) {
    //initialize output array and set counter to 0
    char **output = NULL;
    *num_words = 0;

    //copy input for iteration
    char *input_copy = malloc(strlen(input) + 1);
    strcpy(input_copy, input);

    //flag to track if a found separator character is the first in a row
    int first_comp_flag = 0;

    //input iterator
    int i = 0;

    //temporary string to build for each 
    char *temp = malloc(1);
    temp[0] = '\0';

    //set all separator characters to the first one for easy checking
    //  note that only input_copy is changed, not input
    while (input_copy[i] != '\0') {
        for (int e = 0; e < strlen(sep); e++) {
            if (input_copy[i] == sep[e]) {
                input_copy[i] = sep[0];
            }
        }
        i += 1;
    }

    //iterate through input
    i = 0;
    while (input_copy[i] != '\0') {
        //if the current character is not a separator, add it to temp
        if (input_copy[i] != sep[0]) {
            first_comp_flag = 0;
            temp = realloc(temp, strlen(temp) + 2);
            strncat(temp, input_copy + i, 1);
        }
        //if the current character is the first separator after
        //a non-separator, add temp to the output, reset temp, and
        //increment num_words
        else if (first_comp_flag == 0) {
            first_comp_flag = 1;
            output = realloc(output, sizeof(char*) * (*num_words + 1));
            output[*num_words] = strdup(temp);

            free(temp);
            temp = malloc(1);
            temp[0] = '\0';
            *num_words += 1;
        }
        i += 1;
    }

    //move current temp to output
    output = realloc(output, sizeof(char*) * (*num_words + 1));
    output[*num_words] = strdup(temp);
    *num_words += 1;

    //free memory that shouldn't exit the function
    free(temp);
    free(input_copy);

    return output;
}