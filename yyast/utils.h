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
#ifndef YA_UTILS_H
#define YA_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <yyast/types.h>
#include <yyast/config.h>

typedef union {
    uint128_t   v;
    struct {
        uint64_t v[2];
    } in;
} bswap128_t;

inline uint128_t bswap128(uint128_t x)
{
    bswap128_t  tmp128;
    uint64_t    tmp64;

    tmp128.v = x;

    tmp64          = __builtin_bswap64(tmp128.in.v[0]);
    tmp128.in.v[0] = __builtin_bswap64(tmp128.in.v[1]);
    tmp128.in.v[1] = tmp64;

    return tmp128.v;
}

/** Host to network long long long (128)
 */
inline uint128_t htonlll(uint128_t x)
{
#ifdef WORDS_BIGENDIAN
    return x;
#else
    return bswap128(x);
#endif
}

/** Host to network long long.
 */
inline unsigned long long htonll(unsigned long long x)
{
#ifdef WORDS_BIGENDIAN
    return x;
#else
    return __builtin_bswap64(x);
#endif
}

inline 

/** Network to host long long.
 */
inline unsigned long long ntohll(unsigned long long x)
{
#ifdef WORDS_BIGENDIAN
    return x;
#else
    return __builtin_bswap64(x);
#endif
}

/** Align to a 64 bit boundary.
 * @param x    A size of an object in memory
 * @returns    Either the size if it was aligned, or the next larger size that is aligned.
 */
inline size_t ya_align64(size_t x)
{
    if (sizeof (x) == 4) {
        return (x + 7) & 0xfffffff8;
    } else {
        return (x + 7) & 0xfffffffffffffff8;
    }
}

/** Escape characters in a string.
 * The original string is modified by this function.
 *
 * @param string         The UTF-8 string with escape characters.
 * @param string_size    The number of bytes in the UTF-8 string.
 * @param raw            When 0 standard escape sequence is decoded.
 *                       When 1 only the double quote escape \" is decoded.
 *                       When 2 only the double quote slash \/ is decoded.
 * @returns              The number of characters after decoding.
 */
size_t ya_string_escape(uint8_t *string, size_t string_size, int raw);

/** Strip extension from filename and replace with new extension.
 *
 * @param filename      The original filename.
 * @param new_extension The new extension to append.
 * @returns             Filename with new extension, allocated.
 */
char *ya_new_extension(char *filename, char *new_extension);

#endif
