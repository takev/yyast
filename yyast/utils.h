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
#ifndef YA_UTILS_H
#define YA_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <yyast/types.h>

/** Host to network long long.
 */
unsigned long long htonll(unsigned long long x);

/** Align to a 64 bit boundary.
 * @param x    A size of an object in memory
 * @returns    Either the size if it was aligned, or the next larger size that is aligned.
 */
size_t ya_align64(size_t x);

/** Convert an UTF-8 string into an UCS-4 string.
 * @param in       The UTF-8 string to convert.
 * @param in_size  The size in bytes of the UTF-8 string to convert.
 * @param out      The pre allocated memory the UCS-4 string should be entered, worst case is in_size * 4.
 * @returns        The number of UCS-4 characters emited.
 */
size_t ya_utf8_to_ucs4(const char *_in, size_t in_size, uint32_t *out);

/** Escape characters in a string.
 * The original string is modified by this function.
 *
 * @param string         The UCS-4 string with escape characters.
 * @param string_length  The number of characters in the string.
 * @param raw            When 0 standard escape sequence is decoded.
 *                       When 1 only the double quote escape \" is decoded.
 *                       When 2 only the double quote slash \/ is decoded.
 * @returns              The number of characters after decoding.
 */
size_t ya_string_escape(uint32_t *string, size_t string_length, int raw);

/** Strip extension from filename and replace with new extension.
 *
 * @param filename      The original filename.
 * @param new_extension The new extension to append.
 * @returns             Filename with new extension, allocated.
 */
char *ya_new_extension(char *filename, char *new_extension);

#endif
