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

#define FCC_F64 fourcc('#','f','6','4')
#define FCC_I64 fourcc('#','i','6','4')
#define FCC_U64 fourcc('#','u','6','4')
#define FCC_STR fourcc('#','s','t','r')
#define FCC_RE fourcc('#','r','e',' ')
#define FCC_ID fourcc('#','i','d',' ')
#define FCC_ASM fourcc('#','a','s','m')
#define FCC_DOC fourcc('#','d','o','c')

/** Create an literal.
 *
 * @param type      fourcc code for the ya leaf
 * @param buf       The memory which needs to be copied.
 * @param buf_size  The amount of memory to be copied.
 */
ya_t ya_literal(fourcc_t type, void *buf, size_t buf_size);

/** Create literal for a floating point literal.
 * The literal is of type '#f64', and contains the decoded number as a 64 bit floating point.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_real(char *buf, size_t buf_size);

/** Create literal for an integer literal.
 * The literal is of type '#i64' or '#u64', and contains the decoded number as a 64 bit integer,
 * or 64 bit unsigned integer.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_int(char *buf, size_t buf_size);

/** Create literal for a string literal.
 * The literal is of type '#str', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \n, \r, \t, \x??, \u????, \U????????, \\, \", \/
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_string(char *buf, size_t buf_size);

/** Create literal for a raw string literal.
 * The literal is of type '#str', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \"
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_raw_string(char *buf, size_t buf_size);

/** Create literal for a regex literal.
 * The literal is of type '#re ', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \/
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_regex(char *buf, size_t buf_size);

/** Create literal for a name.
 * The literal is of type '#id ', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_name(char *buf, size_t buf_size);

/** Create literal for assembly.
 * The literal is of type '#asm', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_assembly(char *buf, size_t buf_size);

/** Create literal for comments.
 * The literal is of type '#doc', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_comment(char *buf, size_t buf_size);

#endif
