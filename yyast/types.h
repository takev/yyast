#ifndef YA_TYPES_H
#define YA_TYPES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

/** Position in the text file.
 */
typedef struct {
    int32_t         position;   //< The byte number, starting from zero.
    int32_t         line;       //< The line number, starting from zero.
    int32_t         column;     //< The column number, starting from zero.
} ya_position_t;

/** The data as it will be when writing this node and its subnodes to disk.
 * This structure is allocated and free-ed as it passed along. In priciple it shouldn't leak
 * in the end.
 */
typedef struct {
    uint32_t        size;   //< Size including header in big-endian.
    uint32_t        type;   //< Type big-endian fourcc.
    ya_position_t   start;  //< Start position values in big-endian.
    ya_position_t   end;    //< End position values in big-endian.
    uint32_t        data[]; //< Data aligned to 32 bit and sized to 32 bit. Includes padding bytes.
} ya_node_t;

/** Structure to pass around on the stack for the parser.
 * Passing this whole structure around the stack makes it easier for the user of this library.
 */
typedef struct {
    uint32_t        size;   //< Size including header
    uint32_t        type;   //< Type in fourcc
    ya_position_t   start;  //< Start position value
    ya_position_t   end;    //< End position value
    ya_node_t       *node;  //< The complete data, including the header.
} ya_t;

extern ya_t ya_start;

/** Lex and Yacc needs to know what type should be used. To pass tokens and nodes around.
 */
#define YYSTYPE ya_t

#endif
