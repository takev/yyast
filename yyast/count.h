#ifndef YA_COUNT_H
#define YA_COUNT_H

#include <yyast/types.h>

extern ya_position_t ya_previous_position;
extern ya_position_t ya_current_position;

ya_t ya_count(char *s, size_t s_length);

/** Count columns and lines for each token found by lex.
 */
#define YY_USER_ACTION        { yylval = ya_count(yytext, yyleng); }

#endif
