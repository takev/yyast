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

#include <sys/types.h>
#include <yyast/node.h>
#include <yyast/leaf.h>
#include <yyast/count.h>
#include <yyast/utils.h>

ya_position_t ya_previous_position = {0, 0, 0};
ya_position_t ya_current_position = {0, 0, 0};

char *ya_filenames[YA_MAX_NR_FILENAMES];
int ya_nr_filenames = 0;

ya_t ya_count(char *s, size_t s_length)
{
    uint32_t i;
    char     c;
    int      ascii;
    int      utf8_start;
    int      printable_ascii;
    ya_t     r;

    ya_previous_position = ya_current_position;

    // For the actual column and line we have to read the characters.
    for (i = 0; i < s_length; i++) {
        switch (c = s[i]) {
        case '\n': // Line feed goes to the next line and to the left.
            ya_current_position.line++;
            ya_current_position.column = 0;
            break;
        case '\r': // Carriage return only goes to the left.
            ya_current_position.column = 0;
            break;
        case '\t': // Tab moves cursor to the right on the next 8 column boundary.
            ya_current_position.column+= 8 - (ya_current_position.column % 8);
        default:
            printable_ascii = (c >= ' ') && (c <= '~');
            utf8_start = (c & 0xc0) == 0x80;

            if (printable_ascii || utf8_start) {
                // Only increment column on ASCII and UTF-8 start marker.
                ya_current_position.column++;
            }
        }
    }
    r.size = 0;
    r.type = YA_NODE_TYPE_COUNT;
    r.position = ya_previous_position;
    r.node = NULL;
    return r;
}

uint32_t ya_get_file_nr(char *filename)
{
    int i;

    for (i = 0; i < YA_MAX_NR_FILENAMES; i++) {
        if (i < ya_nr_filenames) {
            if (strcmp(ya_filenames[i], filename) == 0) {
                // filename is found in the table, return the index in the table.
                return i;
            }
            // filename is not found in this entry, try the next entry.

        } else {
            // Append the filename to the table.
            ya_filenames[i] = strdup(filename);
            ya_nr_filenames++;
            return i;
        }
    }

    // Filename could not be found and could not be added to the table.
    return UINT32_MAX;
}

void ya_reposition(char *s, size_t s_length)
{
    char    *s_filename;
    char    *filename;
    long    line       = strtol(s, &s_filename, 10);

    ya_previous_position = ya_current_position;

    if (s == s_filename) {
        fprintf(stderr, "ya_reposition unexpected characters.\n");
        abort();
    }

    ya_current_position.column = 0;
    ya_current_position.line = line - 1; // 1 because of zero index, 2 because of line feed after reposition command.

    if (*s_filename == ' ') {
        // This will contain a filename. Skip of the space and the quote. Strip the trailing quote.
        filename = strndup(&s_filename[2], s_length - (s_filename - s) - 3);

        ya_current_position.file = ya_get_file_nr(filename);
    }
}

ya_t ya_get_filenames(void)
{
    ya_t    filename;
    ya_t    filename_list = YA_EMPTYLIST;
    ya_t    filenames;
    int     i;

    for (i = 0; i < ya_nr_filenames; i++) {
        filename = ya_text("#file", ya_filenames[i], strlen(ya_filenames[i]));
        ya_clear_position(&filename);
        filename_list = YA_LIST(&filename_list, &filename);
    }

    filenames = YA_BRANCH("#files", &filename_list);
    ya_clear_position(&filenames);
    return filenames;
}

void ya_clear_position(ya_t *node)
{
    node->position.column = UINT32_MAX;
    node->position.line = UINT32_MAX;
    node->position.file = UINT32_MAX;
    node->node->position.column = UINT32_MAX;
    node->node->position.line = UINT32_MAX;
    node->node->position.file = UINT32_MAX;
}

