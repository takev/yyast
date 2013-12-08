/* Copyright (c) 2011-2013, Take Vos
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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

/** Reverse count characters.
 * This is a replacement function for yy_more() which throws of our line counting.
 */
void ya_reverse_count(char *s, size_t s_length);

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

/** Get a list of filenames, as a node that is placed in the header.
 * @return A node with a list of filenames.
 */
ya_t ya_get_filenames(void);

/** Clear the position of a node.
 * @param node  The node to clear the position of.
 */
void ya_clear_position(ya_t *node);

/** Count columns and lines for each token found by lex.
 */
#define YY_USER_ACTION        { yylval = ya_count(yytext, yyleng); }

#define YA_MORE               { ya_reverse_count(yytext, yyleng); yymore(); }

#endif
