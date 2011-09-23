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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#include <yyast/utils.h>
#include <yyast/error.h>


size_t ya_string_escape(uint8_t *string, size_t string_size, int raw)
{
    uint8_t         c;
    unsigned int    i, j;
    int             escape = 0;
    int             base = 0;
    int             count = 0;
    int32_t         code_point = 0;

    for (i = 0, j = 0; i < string_size; i++) {
        c = string[i];
        if (count) {
            // We have a hex digit to decode.
            if (c >= '0' && c <= '9') {
                code_point = (code_point << 4) | (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                code_point = (code_point << 4) | (c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                code_point = (code_point << 4) | (c - 'A' + 10);
            } else {
                ya_error("Could not decode hex escape character.");
            }

            // If this is the last digit to expect, then add it as a character.
            if (--count == 0) {
                if        (code_point <= 0x7f) {
                    string[j++] = code_point;
                } else if (code_point <= 0x7ff) {
                    string[j++] = 0xc0 | ((code_point >> 6) & 0x1f);
                    string[j++] = 0x80 | ((code_point     ) & 0x3f);
                } else if (code_point <= 0xffff) {
                    string[j++] = 0xe0 | ((code_point >> 12) & 0x0f);
                    string[j++] = 0x80 | ((code_point >>  6) & 0x3f);
                    string[j++] = 0x80 | ((code_point      ) & 0x3f);
                } else if (code_point <= 0x1fffff) {
                    string[j++] = 0xf0 | ((code_point >> 18) & 0x07);
                    string[j++] = 0x80 | ((code_point >> 12) & 0x3f);
                    string[j++] = 0x80 | ((code_point >>  6) & 0x3f);
                    string[j++] = 0x80 | ((code_point      ) & 0x3f);
                } else if (code_point <= 0x3ffffff) {
                    string[j++] = 0xf8 | ((code_point >> 24) & 0x03);
                    string[j++] = 0x80 | ((code_point >> 18) & 0x3f);
                    string[j++] = 0x80 | ((code_point >> 12) & 0x3f);
                    string[j++] = 0x80 | ((code_point >>  6) & 0x3f);
                    string[j++] = 0x80 | ((code_point      ) & 0x3f);
                } else if (code_point <= 0x7fffffff) {
                    string[j++] = 0xfc | ((code_point >> 30) & 0x01);
                    string[j++] = 0x80 | ((code_point >> 24) & 0x3f);
                    string[j++] = 0x80 | ((code_point >> 18) & 0x3f);
                    string[j++] = 0x80 | ((code_point >> 12) & 0x3f);
                    string[j++] = 0x80 | ((code_point >>  6) & 0x3f);
                    string[j++] = 0x80 | ((code_point      ) & 0x3f);
                } else {
                    ya_error("Could not encode code point %lli.", (long long)code_point);
                }
                string[j++] = code_point;
            }

        } else if (escape) {
            escape = 0;
            // There was an escape character previously. We have different kind of escaping,
            switch (raw) {
            case 2: // Raw regex
                switch (c) {
                case '/': string[j++] = '/';  break;
                default:  string[j++] = '\\'; string[j++] = c;
                }
                break;
            case 1: // Raw string
                switch (c) {
                case '"': string[j++] = '"';  break;
                default:  string[j++] = '\\'; string[j++] = c;
                }
                break;
            default: // Cooked string
                switch (c) {
                case 'n': string[j++] = '\n'; break;
                case 'r': string[j++] = '\r'; break;
                case 't': string[j++] = '\t'; break;
                case '"': string[j++] = '"';  break;
                case '/': string[j++] = '/';  break;
                case '\\': string[j++] = '\\';  break;
                case 'x': base = 16; count = 2; code_point = 0; break;
                case 'u': base = 16; count = 4; code_point = 0; break;
                case 'U': base = 16; count = 8; code_point = 0; break;
                default:  string[j++] = '\\'; string[j++] = c;
                }
            }
        } else if (c == '\\') {
            // We found the escape character, see if we have to do anything.
            escape = 1;

        } else {
            string[j++] = c;
        }
    }

    // Lonely escape character at the end of the string should be emitted.
    if (escape) {
        string[j++] = '\\';
    }

    return j;
}

char *ya_new_extension(char *filename, char *new_extension)
{
    char   *filename_result;
    // Copy the string to strip the extension.
    char   *filename_copy = strdup(filename);
    // Find the last dot in the filename.
    char   *last_dot = strrchr(filename_copy, '.');

    if (last_dot) {
        // Set the last dot in the filename_copy to a nul terminating character.
        *last_dot = 0;
    }

    // Append the new extension to the filename_copy without the last extension.
    if (asprintf(&filename_result, "%s.%s", filename_copy, new_extension) == -1) {
        perror("Could not allocate filename with extension\n");
        abort();
    }

    free(filename_copy);

    return filename_result;
}

