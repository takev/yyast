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

/** Maximum number of source filenames being tracked.
 */
#define YA_MAX_NR_FILENAMES 65536

extern ya_position_t ya_previous_position;
extern ya_position_t ya_current_position;

/** Count characters.
 * This functions keeps track of byte position, line and column.
 * The byte, line and columns are zero index.
 * This function works with UTF-8.
 *
 * @param s         The string to analyze
 * @param s_length  The length of the string in bytes.
 * @returns         An initialized ya structure without a node.
 */
ya_t ya_count(char *s, size_t s_length);

/** Get the file number of a filename.
 * Adds the filename to the table when it was not found.
 *
 * @param filename  Filename to search in the source file list.
 * @return          The index of the filename in the source file list. Or UINT32_MAX if the table is full.
 */
uint32_t ya_get_file_nr(char *filename);

/** Reposition.
 * The given string is a reposition command, it contains the line number of the next
 * line and optionally a filename.
 *
 * The regex of s must match:
 *  [0-9]+\ "([^"]|\\")*"
 */
void ya_reposition(char *s, size_t s_length);

/** Count columns and lines for each token found by lex.
 */
#define YY_USER_ACTION        { yylval = ya_count(yytext, yyleng); }

#endif
