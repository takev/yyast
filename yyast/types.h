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
#ifndef YA_TYPES_H
#define YA_TYPES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>

typedef uint32_t fourcc_t;



/** Convert 4 characters to a fourcc integer.
 */
#define fourcc(x, y, z, w) ((((((x << 8) | y) << 8) | z) << 8) | w)

/** Convert a 4 character string to a fourcc integer.
 */
#define fourcc_str(s) fourcc(s[0], s[1], s[2], s[3])

/** Position in the text file.
 */
struct ya_position_s {
    uint32_t        position;   //< The byte number, starting from zero.
    uint32_t        line;       //< The line number, starting from zero.
    uint32_t        column;     //< The column number, starting from zero.
} __attribute__((packed));
typedef struct ya_position_s ya_position_t;

/** Compressed position in the text file.
 * This value can be encoded in two different ways:
 *
 * bit 63      ( 1 bit )    0 - Both begin and end position with limited range.
 * bit 40-62   (23 bits)    Byte offset of start position.
 * bit 24-39   (16 bits)    Line of the begin position.
 * bit 16-23   ( 8 bits)    Column of the begin position.
 * bit  8-15   ( 8 bits)    Number of lines between begin and end position.
 * bit  0- 7   ( 8 bits)    Column of the end position.
 *
 * bit 63      ( 1 bit )    1 - Only begin position with extended range.
 * bit 32-62   (31 bits)    Byte offset of start position.
 * bit  8-31   (24 bits)    Line of the begin position.
 * bit  0- 7   ( 8 bits)    Column of the begin position.
 */
typedef uint64_t ya_short_position_t;

/** The data as it will be when writing this node and its subnodes to disk.
 * This structure is allocated and free-ed as it passed along. In priciple it shouldn't leak
 * in the end.
 */
struct ya_node_s {
    fourcc_t            type;       //< Type big-endian fourcc.
    uint32_t            length;     //< Length inclding header, but excluding padding in big-endian.
    ya_short_position_t position;   //< Position of the token.
    char                data[];     //< Data aligned to 64 bit and sized to 64 bit. Includes padding zero bytes at the end.
} __attribute__((packed));
typedef struct ya_node_s ya_node_t;

/** Structure to pass around on the stack for the parser.
 * Passing this whole structure around the stack makes it easier for the user of this library.
 */
typedef struct {
    fourcc_t        type;   //< Type in fourcc.
    uint32_t        size;   //< Size including header and padding.
    ya_position_t   start;  //< Start position value.
    ya_position_t   end;    //< End position value.
    ya_node_t       *node;  //< The complete data, including the header.
} ya_t;

extern ya_t ya_start;

/** Convert positions into a compressed position.
 * @param s   Start position
 * @param e   End position
 * @returns   Begin (and End) position compressed into 64 bits.
 */
inline ya_short_position_t short_position(ya_position_t s, ya_position_t e)
{
    ya_short_position_t p = 0;

    if (s.position <= 0x7fffff && s.line <= 0xffff) {
        p = (uint64_t)s.position << 40;
        p|= (uint64_t)s.line << 24;
        p|= MIN((uint64_t)s.column, 0xffULL) << 16;
        p|= MIN((uint64_t)e.line - s.line, 0xffULL) << 8;
        p|= MIN((uint64_t)e.column, 0xffULL);
    } else {
        p = 0x8000000000000000;
        p|= MIN((uint64_t)s.position, 0x7fffffffULL) << 32;
        p|= MIN((uint64_t)s.line, 0xffffffULL) << 8;
        p|= MIN((uint64_t)s.column, 0xffULL);
    }
    return p;
}


/** Lex and Yacc needs to know what type should be used. To pass tokens and nodes around.
 */
#define YYSTYPE ya_t

#endif
