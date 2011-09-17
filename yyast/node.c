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
#include <yyast/node.h>
#include <yyast/utils.h>
#include <yyast/config.h>

#define YA_END_DEF {.c = {'@', 'e', 'n', 'd'}}
#define YA_PASS_DEF {.c = {'@', 'p', 'a', 's'}}
#define YA_SSAP_DEF {.c = {'s', 'a', 'p', '@'}}
#define YA_LIST_DEF {.c = {'@', 'l', 's', 't'}}
#define YA_COUNT_DEF {.c = {'@', 'c', 'n', 't'}}

const fourcc_t YA_END = YA_END_DEF;
const fourcc_t YA_PASS = YA_PASS_DEF;
const fourcc_t YA_SSAP = YA_SSAP_DEF;
const fourcc_t YA_LIST = YA_LIST_DEF;
const fourcc_t YA_COUNT = YA_COUNT_DEF;

ya_t YA_NODE_END = {
    .start = {.position = 0, .line = 0, .column = 0},
    .end   = {.position = 0, .line = 0, .column = 0},
    .type  = YA_END_DEF, // "@end"
    .node  = NULL
};

ya_node_t YA_NODE_NODE_PASS = {
    .start = {.position = 0, .line = 0, .column = 0},
    .end   = {.position = 0, .line = 0, .column = 0},
#ifdef WORDS_BIGENDIAN
    .type  = YA_PASS_DEF, // "@pas"
    .size  = 0x00000020
#else
    .type  = YA_SSAP_DEF, // "sap@"
    .size  = 0x20000000
#endif
};

ya_t YA_NODE_PASS = {
    .start = {.position = 0, .line = 0, .column = 0},
    .end   = {.position = 0, .line = 0, .column = 0},
    .type  = YA_PASS_DEF, // "@pas"
    .node  = &YA_NODE_NODE_PASS
};

ya_t ya_generic_node(ya_t *first, ya_t *last, fourcc_t type, va_list ap)
{
    va_list ap2;
    ya_t    *item;
    size_t  item_size = 0;
    ya_t    self;
    size_t  self_size = 0;
    off_t   self_offset = 0;

    va_copy(ap2, ap);

    if (type.i == fourcc("@lst").i) {
        fprintf(stderr, "list");
    } else {
        fprintf(stderr, "node");
    }

    fprintf(stderr, " %i:%i-%i:%i, '%c%c%c%c'\n",
        first->start.line, first->start.column,
        last->end.line, last->end.column,
        type.c[0], type.c[1], type.c[2], type.c[3]
    );

    // Calculate the size of the content.
    for (item = va_arg(ap, ya_t *); item->type.i != YA_END.i; item = va_arg(ap, ya_t *)) {
        fprintf(stderr, "- '%c%c%c%c'\n", item->type.c[0], item->type.c[1], item->type.c[2], item->type.c[3]);
        if (item->type.i == YA_COUNT.i) {
            fprintf(stderr, "Found '@cnt' token, which is only allowed for line counting\n");
            abort();
        } else if (item->type.i == YA_LIST.i) {
            // Only the content is copied of a list, not its header.
            self_size+= ya_align32(item->size) - sizeof (ya_node_t);
        } else {
            // This is a normal node, the whole thing must be copied.
            self_size+= ya_align32(item->size);
        }
    }

    // Create a new list.
    self.start = first->start;
    self.end = last->end;
    self.type = type;
    self.size = self_size + sizeof (ya_node_t);
    // With the content header.
    self.node = calloc(1, ya_align32(self.size));
    self.node->type.i         = htonl(self.type.i);
    self.node->size           = htonl(self.size);
    self.node->start.position = htonl(self.start.position);
    self.node->start.line     = htonl(self.start.line);
    self.node->start.column   = htonl(self.start.column);
    self.node->end.position   = htonl(self.end.position);
    self.node->end.line       = htonl(self.end.line);
    self.node->end.column     = htonl(self.end.column);

    // Add the content of the items to the new list.
    for (item = va_arg(ap2, ya_t *); item->type.i != YA_END.i; item = va_arg(ap2, ya_t *)) {
        if (item->type.i == YA_COUNT.i) {
            fprintf(stderr, "Found '@cnt' token, which is only allowed for line counting\n");
            abort();
        } else if (item->type.i == YA_LIST.i) {
            // Only copy the contents of a list, without the header.
            item_size = ya_align32(item->size) - sizeof (ya_node_t);
            memcpy(&(self.node->data[self_offset]), &(item->node->data[sizeof (ya_node_t)]), item_size);
            self_offset+= item_size;
        } else {
            // Copy all of the node including the header.
            item_size = ya_align32(item->size);
            memcpy(&(self.node->data[self_offset]), item->node->data, item_size);
            self_offset+= item_size;
        }

        if (item->type.i != YA_PASS.i) {
            // Only free the node if it was allocated by malloc, @pas are singletons.
            free(item->node);
        }
    }

    va_end(ap2);
    return self;
}

ya_t ya_node(ya_t *first, ya_t *last, fourcc_t type, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, type);
    r = ya_generic_node(first, last, type, ap);
    va_end(ap);

    return r;
}

ya_t ya_list(ya_t *first, ya_t *last, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, last);
    r = ya_generic_node(first, last, YA_LIST, ap);
    va_end(ap);

    return r;
}

