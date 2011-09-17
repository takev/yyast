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

#include <yyast/config.h>
#include <yyast/utils.h>
#include <yyast/error.h>

unsigned long long htonll(unsigned long long x)
{
#ifdef WORDS_BIGENDIAN
    return x;
#else
    return __builtin_bswap64(x);
#endif
}

fourcc_t fourcc(char *s)
{
    fourcc_t r;
    int      i;

    memcpy(r.c, s, sizeof (fourcc_t));
    return r;
}

size_t ya_align32(size_t x)
{
    if (sizeof (x) == 4) {
        return (x + 3) & 0xfffffffc;
    } else {
        return (x + 3) & 0xfffffffffffffffc;
    }
}

size_t ya_utf8_to_ucs4(const char *in, size_t in_size, uint32_t *out)
{
    union {
        char    i;
        uint8_t u;
    } c_in;
    uint32_t      i, j;
    uint32_t      c_out = 0;

    for (i = 0, j = 0; i < in_size; i++) {
        c_in.i = in[i];

        if ((c_in.u & 0xc0) != 0x80) {
            // The next byte is an UTF-8 continuation. So write out the character.
            if (c_out) {
                out[j++] = c_out;
                c_out = 0;
             }
        }

        if (c_in.u < 128) {
            c_out = c_in.u;
        } else if ((c_in.u & 0xe0) == 0xc0) {
            c_out = c_in.u & 0x1f;
        } else if ((c_in.u & 0xf0) == 0xe0) {
            c_out = c_in.u & 0x0f;
        } else if ((c_in.u & 0xf8) == 0xf0) {
            c_out = c_in.u & 0x07;
        } else if ((c_in.u & 0xfc) == 0xf8) {
            c_out = c_in.u & 0x03;
        } else if ((c_in.u & 0xfe) == 0xfc) {
            c_out = c_in.u & 0x01;
        } else if ((c_in.u & 0xc0) == 0x80) {
            c_out = (c_out << 6) | (c_in.u & 0x3f);
        }
    }

    // Write the lya character out.
    if (c_out) {
        out[j++] = c_out;
        c_out = 0;
    }

    return j;
}

size_t ya_string_escape(uint32_t *string, size_t string_size, int raw)
{
    uint32_t c;
    uint32_t i, j;
    int      escape = 0;
    int      base = 0;
    int      count = 0;
    int      hex_code = 0;

    for (i = 0, j = 0; i < string_size; i++) {
        c = string[i];
        if (count) {
            // We have a hex digit to decode.
            if (c >= '0' && c <= '9') {
                hex_code = (hex_code << 4) | (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                hex_code = (hex_code << 4) | (c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                hex_code = (hex_code << 4) | (c - 'A' + 10);
            } else {
                ya_error("Could not decode hex escape character.");
            }

            // If this is the lya digit to expect, then add it as a character.
            if (--count == 0) {
                string[j++] = hex_code;
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
                case 'x': base = 16; count = 2; hex_code = 0; break;
                case 'u': base = 16; count = 4; hex_code = 0; break;
                case 'U': base = 16; count = 8; hex_code = 0; break;
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

