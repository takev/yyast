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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/leaf.h>
#include <yyast/count.h>
#include <yyast/utils.h>
#include <yyast/error.h>

ya_t ya_literal(const char * restrict name, ya_type_t type, const void * restrict buf, size_t buf_size)
{
    size_t aligned_buf_size = ya_align64(buf_size);

    ya_t r = {
        .type = type,
        .size = sizeof (ya_node_t) + aligned_buf_size,  // The outer size is the allocated size.
        .position = ya_previous_position,
    };

    // Allocate memory aligned to 32 bit. We need to use calloc so that we don't accidently
    // leak information into the output file.
    r.node = calloc(1, r.size);
    r.node->name             = htonll(ya_create_name(name));
    r.node->size             = htonll(sizeof (ya_node_t) + aligned_buf_size);  // The inner length is the length of header+data.
    r.node->type             = r.type;
    r.node->position.file    = htonl(r.position.file);
    r.node->position.line    = htonl(r.position.line);
    r.node->position.column  = htonl(r.position.column);

    memcpy(r.node->data, buf, buf_size);
    // Set padding bytes to zero, so as not to leak data. For security purposes.
    memset(&r.node->data[buf_size], 0, aligned_buf_size - buf_size);
    return r;
}


typedef union {
    double      d;
    uint64_t    u;
    int64_t     i;
} translate_t;

ya_t ya_binary_float(const char * restrict name, const char * restrict buf, size_t buf_size __attribute__((unused)), int base __attribute__((unused)), int nr_bits __attribute__((unused)))
{
    translate_t t;
    char        *endptr;

    t.d = strtold(buf, &endptr);
    if (buf == endptr) {
        ya_error("Could not convert real value '%s'", buf);
    }

    if ((t.d == HUGE_VALL || t.d == -HUGE_VALL) && errno == ERANGE) {
        ya_error("Could not convert real value '%s', overflow", buf);
    }

    if ((t.d == 0.0 || t.d == -0.0) && errno == ERANGE) {
        ya_error("Could not convert real value '%s', underflow", buf);
    }

    t.u = htonll(t.u);
    return ya_literal(name, YA_NODE_TYPE_BINARY_FLOAT, &t.u, sizeof (t.u));
}

ya_t ya_integer(const char * restrict name, ya_type_t type, const char * restrict buf, size_t buf_size, int base)
{
    int                 i = 0;
    char                c;
    uint128_t           value;
    uint128_t           value128;
    uint64_t            value64;
    int                 digit;

    value = 0;
    for (i = 0; i < buf_size; i++) {
        c = buf[i];

        if (base == 32) {
            // RFC 4648, Base-32
            if      (c >= 'a' && c <= 'z')  { digit = c - 'a';      }
            else if (c >= 'A' && c <= 'Z')  { digit = c - 'A';      }
            else if (c >= '2' && c <= '7')  { digit = c - '2' + 26; }
            else                            { digit = base;         }

        } else if (base >= 37 && base <= 64) {
            // RFC 4648, Base-64
            if      (c >= 'A' && c <= 'Z')  { digit = c - 'A';      }
            else if (c >= 'a' && c <= 'z')  { digit = c - 'a' + 26; }
            else if (c >= '0' && c <= '9')  { digit = c - '0' + 52; }
            else if (c == '+'            )  { digit = 62;           }
            else if (c == '/'            )  { digit = 63;           }
            else                            { digit = base;         }

        } else if (base >= 0 && base <= 36) {
            // RFC 4648, Base-16
            if      (c >= '0' && c <= '9')  { digit = c - '0';      }
            else if (c >= 'A' && c <= 'Z')  { digit = c - 'A' + 10; }
            else if (c >= 'a' && c <= 'z')  { digit = c - 'a' + 10; }
            else                            { digit = base;         }

        } else {
            fprintf(stderr, "ya_integer does not implement a base above 64.\n");
            abort();
        }

        if (digit < base) {
            // Only characters inside the alphabet are processed, all other characters are ignored.
            value *= base;
            if (value + digit < value) {
                ya_error("Overflow of integer literal");
            }
            value += digit;
        }
    }

    if (value <= UINT64_MAX) {
        value64 = htonll(value);
        return ya_literal(name, type, &value64, sizeof(value64));
    } else {
        value128 = htonlll(value);
        return ya_literal(name, type, &value128, sizeof(value128));
    }
}

ya_t ya_text(const char * restrict name, const char * restrict buf, size_t buf_size)
{
    return ya_literal(name, YA_NODE_TYPE_TEXT, buf, buf_size);
}

ya_t ya_leaf(const char * restrict name)
{
    return ya_literal(name, YA_NODE_TYPE_LEAF, NULL, 0);
}

ya_t ya_null(void)
{
    return ya_literal("#null", YA_NODE_TYPE_NULL, NULL, 0);
}

ya_t ya_positive_integer(const char * restrict name, const char * restrict buf, size_t buf_size, int base)
{
    return ya_integer(name, YA_NODE_TYPE_POSITIVE_INTEGER, buf, buf_size, base);
}

ya_t ya_negative_integer(const char * restrict name, const char * restrict buf, size_t buf_size, int base)
{
    return ya_integer(name, YA_NODE_TYPE_NEGATIVE_INTEGER, buf, buf_size, base);
}

