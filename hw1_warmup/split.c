#include "split.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **string_split(const char *input, const char *sep, int *num_words) {
    char **output = NULL;
    char *input_copy = strcopy(input);
    
    char *substring = malloc(sizeof(char) * 5);
    *num_words += 1;

}