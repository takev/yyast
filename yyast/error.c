
#include <stdlib.h>
#include <stdio.h>

void ya_error(const char *message, ...)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

