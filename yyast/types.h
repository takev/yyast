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
#include <strings.h>
#include <sys/param.h>

typedef __uint128_t uint128_t;

typedef uint64_t ya_name_t;
typedef uint8_t  ya_type_t;

#define YA_NODE_TYPE_PASS              0
#define YA_NODE_TYPE_EMPTY             1
#define YA_NODE_TYPE_BRANCH            2
#define YA_NODE_TYPE_TEXT              3
#define YA_NODE_TYPE_POSITIVE_INTEGER  4
#define YA_NODE_TYPE_NEGATIVE_INTEGER  5
#define YA_NODE_TYPE_BINARY_FLOAT      6
//#define YA_NODE_TYPE_DECIMAL_FLOAT     7

#define YA_NODE_TYPE_LIST              253
#define YA_NODE_TYPE_COUNT             254
#define YA_NODE_TYPE_END               255

/** Position in the text file.
 */
struct ya_position_s {
    uint32_t        line;       //< The line number, starting from one.
    uint32_t        column;     //< The column number, starting from one.
    uint32_t        file;       //< The file number, starting from zero (the main file).
} __attribute__((packed, aligned(4)));
typedef struct ya_position_s ya_position_t;

/** The data as it will be when writing this node and its subnodes to disk.
 * This structure is allocated and free-ed as it passed along. In priciple it shouldn't leak
 * in the end. 
 */
struct ya_node_s {
    ya_name_t           name;
    uint64_t            size;
    ya_position_t       position;
    uint16_t            reserved_1;
    uint8_t             reserved_2;
    ya_type_t           type;
    char                data[];     //< Data aligned to 64 bit and sized to 64 bit. Includes padding zero bytes at the end.
} __attribute__((packed, aligned(8)));
typedef struct ya_node_s ya_node_t;

/** Structure to pass around on the stack for the parser.
 * Passing this whole structure around the stack makes it easier for the user of this library.
 */
typedef struct {
    uint64_t        size;       //< Size including header and padding.
    ya_position_t   position;   //< Position where the start of the node is located in the source file.
    ya_type_t       type;       //< The type of node.
    ya_node_t       *node;      //< The complete data, including the header.
} ya_t;

extern ya_t ya_start;

/** Lex and Yacc needs to know what type should be used. To pass tokens and nodes around.
 */
#define YYSTYPE ya_t

/** Convert a string into a eightcc integer.
 * @param s The string to convert.
 * @return  An eightcc integer.
 */
static inline ya_name_t ya_create_name(const char * restrict s)
{
    unsigned int    i;
    unsigned int    s_len = strlen(s);
    ya_name_t       name = 0;

    for (i = 0; i < sizeof (name); i++) {
        name <<= 8;
        if (i < s_len) {
            // String is left justified.
            name |= s[i];
        } else {
            // With trailing spaces.
            name |= ' ';
        }
    }
    return name;
}


#endif
