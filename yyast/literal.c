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
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/literal.h>
#include <yyast/count.h>
#include <yyast/utils.h>
#include <yyast/error.h>

ya_t ya_literal(fourcc_t type, void *buf, size_t buf_size)
{
    ya_t r = {
        .type = type,
        .size = ya_align64(buf_size + sizeof (ya_node_t)),  // The outer size is the allocated size.
        .start = ya_previous_position,
        .end = ya_current_position
    };

    fprintf(stderr, "literal %i:%i-%i:%i, '%c%c%c%c'\n",
        r.start.line, r.start.column,
        r.end.line, r.end.column,
        (char)((r.type >> 24) & 0xff), (char)((r.type >> 16) & 0xff), (char)((r.type >> 8) & 0xff), (char)(r.type & 0xff)
    );

    // Allocate memory aligned to 32 bit. We need to use calloc so that we don't accidently
    // leak information into the output file.
    r.node = calloc(1, r.size);
    r.node->length         = htonl(buf_size + sizeof (ya_node_t));  // The inner length is the length of header+data.
    r.node->type           = htonl(r.type);
    r.node->start.position = htonl(r.start.position);
    r.node->start.line     = htonl(r.start.line);
    r.node->start.column   = htonl(r.start.column);
    r.node->end.position   = htonl(r.end.position);
    r.node->end.line       = htonl(r.end.line);
    r.node->end.column     = htonl(r.end.column);
    memcpy(r.node->data, buf, buf_size);
    // Set padding bytes to zero, so as not to leak data. For security purposes.
    memset(&r.node->data[buf_size], 0, ya_align64(buf_size) - buf_size);
    return r;
}


typedef union {
    double      d;
    uint64_t    u;
    int64_t     i;
} translate_t;

ya_t ya_real(char *buf, size_t buf_size)
{
    translate_t t;
    char        *endptr;

    t.d = strtold(buf, &endptr);
    if (buf == endptr) {
        ya_error("Could not convert real value '%s'", buf);
        exit(1);
    }

    if ((t.d == HUGE_VALL || t.d == -HUGE_VALL) && errno == ERANGE) {
        ya_error("Could not convert real value '%s', overflow", buf);
        exit(1);
    }

    if ((t.d == 0.0 || t.d == -0.0) && errno == ERANGE) {
        ya_error("Could not convert real value '%s', underflow", buf);
        exit(1);
    }

    t.u = htonll(t.u);
    return ya_literal(FCC_F64, &t.u, sizeof (t.u));
}

ya_t ya_int(char *buf, size_t buf_size)
{
    int         i = 0;
    int         negative = 0;
    int         base = 10;
    char        *endptr;
    translate_t t;

    switch (buf[i]) {
    case '-': i++; negative = 1; break;
    case '+': i++; negative = 0; break;
    }

    if (buf[i] == '0') {
        i++;
        base = 8;
        switch (buf[i]) {
        case 'x': case 'X': i++; base = 16; break;
        case 'd': case 'D': i++; base = 10; break;
        case 'o': case 'O': i++; base =  8; break;
        case 'b': case 'B': i++; base =  2; break;
        }
    }

    t.u = strtoull(&buf[i], &endptr, base);
    if (&buf[i] == endptr) {
        ya_error("Could not convert int value '%s'", buf);
        exit(1);
    }
    if (t.u == ULLONG_MAX && errno == ERANGE) {
        ya_error("Could not convert int value '%s', overflow", buf);
        exit(1);
    }

    if (negative) {
        if (t.u > INT64_MAX) {
            ya_error("Could not convert int value '%s', overflow", buf);
            exit(1);
        }
        t.i = -t.u;
    }

    t.u = htonll(t.u);
    return ya_literal(negative ? FCC_I64 : FCC_U64, &t.u, sizeof (t.u));
}

static ya_t ya_generic_string(char *buf, size_t buf_size, fourcc_t type, int raw)
{
    ya_t            r;
    uint8_t         *string = (uint8_t *)strndup(buf, buf_size);
    size_t          string_size = strlen((char *)string);
    unsigned int    i;

    string_size = ya_string_escape(string, string_size, raw);
    r = ya_literal(type, string, string_size);

    free(string);
    return r;
}

ya_t ya_string(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_STR, 0);
}

ya_t ya_raw_string(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_STR, 1);
}

ya_t ya_regex(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_RE, 2);
}

ya_t ya_name(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_ID, 1);
}

ya_t ya_assembly(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_ASM, 1);
}

ya_t ya_comment(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, FCC_DOC, 1);
}

