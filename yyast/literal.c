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
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/literal.h>
#include <yyast/count.h>

ya_t ya_literal(fourcc_t type, void *buf, size_t buf_size)
{
    ya_t r = {
        .type = type,
        .size = buf_size + sizeof (ya_node_t),
        .start = ya_previous_position,
        .end = ya_current_position
    };

    fprintf(stderr, "literal %i:%i-%i:%i, '%c%c%c%c'\n",
        r.start.line, r.start.column,
        r.end.line, r.end.column,
        type.c[0], type.c[1], type.c[2], type.c[3]
    );

    // Allocate memory aligned to 32 bit. We need to use calloc so that we don't accidently
    // leak information into the output file.
    r.node = calloc(1, ya_align32(r.size));
    r.node->size           = htonl(r.size);
    r.node->type.i         = htonl(r.type.i);
    r.node->start.position = htonl(r.start.position);
    r.node->start.line     = htonl(r.start.line);
    r.node->start.column   = htonl(r.start.column);
    r.node->end.position   = htonl(r.end.position);
    r.node->end.line       = htonl(r.end.line);
    r.node->end.column     = htonl(r.end.column);
    memcpy(r.node->data, buf, buf_size);
    return r;
}

