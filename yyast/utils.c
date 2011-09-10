
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "yyast/utils.h"

size_t ya_align_size(size_t x)
{
    if (sizeof (x) == 4) {
        return (x + 3) & 0xfffffffc;
    } else {
        return (x + 3) & 0xfffffffffffffffc;
    }
}

size_t ya_utf8_to_ucs4(const uint8_t *in, size_t in_size, uint32_t *out)
{
    off_t    i, j;
    uint8_t  c_in;
    uint32_t c_out = 0;

    for (i = 0, j = 0; i < in_size; i++) {
        c_in = in[i];

        if ((c_in & 0xc0) != 0x80) {
            // The next byte is an UTF-8 continuation. So write out the character.
            if (c_out) {
                out[j++] = c_out;
                c_out = 0;
             }
        }

        if (c_in < 128) {
            c_out = c_in;
        } else if ((c_in & 0xe0) == 0xc0) {
            c_out = c_in & 0x1f;
        } else if ((c_in & 0xf0) == 0xe0) {
            c_out = c_in & 0x0f;
        } else if ((c_in & 0xf8) == 0xf0) {
            c_out = c_in & 0x07;
        } else if ((c_in & 0xfc) == 0xf8) {
            c_out = c_in & 0x03;
        } else if ((c_in & 0xfe) == 0xfc) {
            c_out = c_in & 0x01;
        } else if ((c_in & 0xc0) == 0x80) {
            c_out = (c_out << 6) | (c_in & 0x3f);
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
    off_t    i, j;
    int      escape = 0;
    int      base = 0;
    int      count = 0;
    int      hex_code = 0;

    for (i = 0, j = 0; i < string_size; i++) {
        c = string[i];
        if (count) {
            // We have a hex digit to decode.
            if (c >= '0' && c <= '9') {
                hex_code = (hex_code << 4) | c - '0';
            } else if (c >= 'a' && c <= 'f') {
                hex_code = (hex_code << 4) | c - 'a' + 10;
            } else if (c >= 'A' && c <= 'F') {
                hex_code = (hex_code << 4) | c - 'A' + 10;
            } else {
                ya_error("Could not decode hex escape character.");
                abort();
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

