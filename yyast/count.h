#ifndef YA_COUNT_H
#define YA_COUNT_H

#include <yyast/types.h>

extern ya_position_t previous_position;
extern ya_position_t current_position;

ya_t ya_count(char *s, size_t s_length);

#endif
