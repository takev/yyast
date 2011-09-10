
#include <stdlib.h>
#include <stdio.h>

void ya_error(char *message, ...)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

