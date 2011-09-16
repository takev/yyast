/*  Copyright 2011 Take Vos
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
