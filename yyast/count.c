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

#include <sys/types.h>
#include <yyast/node.h>
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
        fprintf(stderr, "ya_reposition unexpected characters.");
        abort();
    }

    ya_current_position.line = line - 2; // 1 because of zero index, 2 because of line feed after reposition command.

    if (*s_filename == ' ') {
        // This will contain a filename. Skip of the space and the quote. Strip the trailing quote.
        filename = strndup(&s_filename[2], s_length - (s_filename - s) - 3);

        ya_current_position.file = ya_get_file_nr(filename);
    }
}

