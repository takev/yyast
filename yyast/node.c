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

ya_t YA_NODE_END = {
    .start = {.position = 0, .line = 0, .column = 0},
    .end   = {.position = 0, .line = 0, .column = 0},
    .type  = FCC_END,
    .node  = NULL
};

ya_node_t YA_NODE_NODE_PASS = {
    .start  = {.position = 0, .line = 0, .column = 0},
    .end    = {.position = 0, .line = 0, .column = 0},
#ifdef WORDS_BIGENDIAN
    .type   = FCC_PASS,
    .length = 0x00000020
#else
    .type   = FCC_SSAP,
    .length = 0x20000000
#endif
};

ya_t YA_NODE_PASS = {
    .start = {.position = 0, .line = 0, .column = 0},
    .end   = {.position = 0, .line = 0, .column = 0},
    .type  = FCC_PASS,
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

    if (type == FCC_LIST) {
        fprintf(stderr, "list");
    } else {
        fprintf(stderr, "node");
    }

    fprintf(stderr, " %i:%i-%i:%i, '%c%c%c%c'\n",
        first->start.line, first->start.column,
        last->end.line, last->end.column,
        (char)((type >> 24) & 0xff), (char)((type >> 16) & 0xff), (char)((type >> 8) & 0xff), (char)(type & 0xff)
    );

    // Calculate the size of the content.
    for (item = va_arg(ap, ya_t *); item->type != FCC_END; item = va_arg(ap, ya_t *)) {
        fprintf(stderr, "- '%c%c%c%c'\n", (char)((item->type >> 24) & 0xff), (char)((item->type >> 16) & 0xff), (char)((item->type >> 8) & 0xff), (char)(item->type & 0xff));
        switch (item->type) {
        case FCC_COUNT:
            fprintf(stderr, "Found '@cnt' token, which is only allowed for line counting\n");
            abort();
        case FCC_LIST:
            // Only the content is copied of a list, not its header.
            self_size+= item->size - sizeof (ya_node_t);
            break;
        default:
            // This is a normal node, the whole thing must be copied.
            self_size+= item->size;
        }
    }

    // Create a new node.
    self.start = first->start;
    self.end = last->end;
    self.type = type;
    self.size = self_size + sizeof (ya_node_t);
    // With the content header.
    self.node = calloc(1, self.size);
    self.node->type           = htonl(self.type);
    self.node->length         = htonl(self.size);
    self.node->start.position = htonl(self.start.position);
    self.node->start.line     = htonl(self.start.line);
    self.node->start.column   = htonl(self.start.column);
    self.node->end.position   = htonl(self.end.position);
    self.node->end.line       = htonl(self.end.line);
    self.node->end.column     = htonl(self.end.column);

    // Add the content of the items to the new list.
    for (item = va_arg(ap2, ya_t *); item->type != FCC_END; item = va_arg(ap2, ya_t *)) {
        switch (item->type) {
        case FCC_COUNT:
            fprintf(stderr, "Found '@cnt' token, which is only allowed for line counting\n");
            abort();
        case FCC_LIST:
            // Only copy the contents of a list, without the header.
            item_size = item->size - sizeof (ya_node_t);
            memcpy(&(self.node->data[self_offset]), item->node->data, item_size);
            self_offset+= item_size;
            break;
        default:
            // Copy all of the node including the header.
            item_size = item->size;
            memcpy(&(self.node->data[self_offset]), item->node, item_size);
            self_offset+= item_size;
        }

        if (item->type != FCC_PASS) {
            // Only free the node if it was allocated by malloc, @pas are singletons.
            free(item->node);
        }
    }

    va_end(ap2);

    fprintf(stderr, "+ size %lu\n", (long unsigned int)self.size);
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
    r = ya_generic_node(first, last, FCC_LIST, ap);
    va_end(ap);

    return r;
}

void ya_node_save(FILE *output_file, ya_t *node)
{
    fwrite(node->node, node->size, 1, output_file);
}

