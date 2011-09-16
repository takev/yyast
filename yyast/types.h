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

typedef union {
    uint32_t    i;
    char        c[4];
} fourcc_t;

extern const fourcc_t YA_END;
extern const fourcc_t YA_PASS;
extern const fourcc_t YA_SSAP;
extern const fourcc_t YA_LIST;
extern const fourcc_t YA_COUNT;

/** Position in the text file.
 */
struct ya_position_s {
    uint32_t        position;   //< The byte number, starting from zero.
    uint32_t        line;       //< The line number, starting from zero.
    uint32_t        column;     //< The column number, starting from zero.
} __attribute__((packed));
typedef struct ya_position_s ya_position_t;

/** The data as it will be when writing this node and its subnodes to disk.
 * This structure is allocated and free-ed as it passed along. In priciple it shouldn't leak
 * in the end.
 */
struct ya_node_s {
    ya_position_t   start;  //< Start position values in big-endian.
    ya_position_t   end;    //< End position values in big-endian.
    fourcc_t        type;   //< Type big-endian fourcc.
    uint32_t        size;   //< Size including header in big-endian.
    uint32_t        data[]; //< Data aligned to 32 bit and sized to 32 bit. Includes padding bytes.
} __attribute__((packed));
typedef struct ya_node_s ya_node_t;

/** Structure to pass around on the stack for the parser.
 * Passing this whole structure around the stack makes it easier for the user of this library.
 */
typedef struct {
    fourcc_t        type;   //< Type in fourcc
    uint32_t        size;   //< Size including header
    ya_position_t   start;  //< Start position value
    ya_position_t   end;    //< End position value
    ya_node_t       *node;  //< The complete data, including the header.
} ya_t;

extern ya_t ya_start;

/** Lex and Yacc needs to know what type should be used. To pass tokens and nodes around.
 */
#define YYSTYPE ya_t

#endif
