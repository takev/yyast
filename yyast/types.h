#ifndef YA_TYPES_H
#define YA_TYPES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    int32_t         position;
    int32_t         line;
    int32_t         column;
} ya_position_t;

typedef struct {
    uint32_t        size;
    uint32_t        type;
    ya_position_t  start;
    ya_position_t  end;
    uint32_t        data[0];
} ya_node_t;

typedef struct {
    ya_position_t  start;
    ya_position_t  end;
    ya_node_t      *node;
} ya_t;

#endif
