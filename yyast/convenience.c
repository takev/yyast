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
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/convenience.h>
#include <yyast/literal.h>
#include <yyast/utils.h>
#include <yyast/count.h>
#include <yyast/error.h>

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

    return ya_literal(fourcc("#f64"), &t.u, sizeof (t.u));
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

    return ya_literal(negative ? fourcc("#i64") : fourcc("#u64"), &t.u, sizeof (t.u));
}

static ya_t ya_generic_string(char *buf, size_t buf_size, fourcc_t type, int raw)
{
    ya_t       r;
    uint32_t    *string = malloc(buf_size * sizeof (uint32_t));
    size_t      string_size = ya_utf8_to_ucs4(buf, buf_size, string);

    string_size = ya_string_escape(string, string_size, raw);
    r = ya_literal(type, string, string_size * sizeof (uint32_t));
    free(string);

    return r;
}

ya_t ya_string(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#str"), 0);
}

ya_t ya_raw_string(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#str"), 1);
}

ya_t ya_regex(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#re "), 2);
}

ya_t ya_name(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#id "), 1);
}

ya_t ya_assembly(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#asm"), 1);
}

ya_t ya_comment(char *buf, size_t buf_size)
{
    return ya_generic_string(buf, buf_size, fourcc("#doc"), 1);
}

