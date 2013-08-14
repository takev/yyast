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
#ifndef YA_LITERAL_H
#define YA_LITERAL_H

#include <yyast/types.h>
#include <yyast/utils.h>

#define YA_INTEGER_POSITIVE 0
#define YA_INTEGER_NEGATIVE 1

/** Create an literal node.
 *
 * @param name      name of the node.
 * @param type      type of leaf node.
 * @param buf       The memory which needs to be copied.
 * @param buf_size  The amount of memory to be copied.
 */
ya_t ya_literal(const char * restrict name, ya_type_t type, const void * restrict buf, size_t buf_size);

/** Create literal node from floating point literal string.
 *
 * @param name      name of the node.
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @param base            
 * @param nr_bits   Nr of bits in the floating point format, only 64 bits is supported.
 * @returns         The decoded literal node.
 */
ya_t ya_binary_float(const char * restrict name, const char * restrict buf, size_t buf_size, int base, int nr_bits);

/** Create literal from an integer literal string.
 * The RFC 4648 is used as a guide for encoding integers.
 *
 * - Base  0,                has no alphabet, it always yields zero.
 * - Base  1-31 and 33-36,   These bases are encoded following an extended Base-16
 * - Base 32,                Base-32 is encoding
 * - Base 37-64              These bases are encoded following Base-64
 *
 * @param name      name of the node.
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @param base      The base of the literal. 0-64
 * @returns         The decoded literal.
 */
ya_t ya_integer(const char * restrict name, ya_type_t type, const char * restrict buf, size_t buf_size, int base);

/** Create text literal node for a string literal.
 *
 * @param name      name of the node.
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_text(const char * restrict name, const char * restrict buf, size_t buf_size);

#endif
