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
#include <yyast/count.h>

ya_t YA_NODE_DEFAULT = {
    .type  = YA_NODE_TYPE_END,
    .position = {.line = UINT32_MAX, .column = UINT32_MAX, .file = UINT32_MAX},
    .size  = sizeof (ya_node_t),
    .node  = NULL
};

ya_t YA_NODE_END = {
    .type  = YA_NODE_TYPE_END,
    .position = {.line = UINT32_MAX, .column = UINT32_MAX, .file = UINT32_MAX},
    .size  = 0,
    .node  = NULL
};

ya_t ya_generic_nodev(const char * restrict name, ya_type_t type, va_list ap)
{
    va_list ap2;
    ya_t    *item;
    size_t  item_size   = 0;
    ya_t    self        = YA_NODE_DEFAULT;
    off_t   self_offset = 0;

    va_copy(ap2, ap);

    // Calculate the size and position of the content.
    self.type = type;
    for (item = va_arg(ap, ya_t *); item->type != YA_NODE_TYPE_END; item = va_arg(ap, ya_t *)) {
        if (self.position.file == UINT32_MAX && item->position.file != UINT32_MAX) {
            // The first node which has a file (not UINT32_MAX) is used for where self is located.
            self.position.file   = item->position.file;
            self.position.line   = item->position.line;
            self.position.column = item->position.column;

        } else if (self.position.file == item->position.file) {
            // The lowest line, column number combination is selected from nodes which are in the same file.
            if (item->position.line < self.position.line) {
                self.position.line   = item->position.line;
                self.position.column = item->position.column;

            } else if ((item->position.line == self.position.line) && (item->position.column < self.position.column)) {
                self.position.column = item->position.column;
            }
        }

        switch (item->type) {
        case YA_NODE_TYPE_COUNT:
            fprintf(stderr, "Found YA_NODE_TYPE_COUNT token, which is only allowed for line counting\n");
            abort();

        case YA_NODE_TYPE_LIST:
            // A list node inserts the child nodes into self, therefor the header of the list node is not included.
            self.size+= item->size - sizeof (ya_node_t);
            break;

        default:
            // This is a normal node, it will be copied as a normal child into self.
            self.size+= item->size;
        }
    }

    // With the content header.
    self.node = calloc(1, self.size);
    self.node->name                 = htonll(ya_create_name(name));
    self.node->type                 = self.type;
    self.node->size                 = htonll(self.size);
    self.node->position.file     = htonl(self.position.file);
    self.node->position.line     = htonl(self.position.line);
    self.node->position.column   = htonl(self.position.column);

    // Add the content of the items to the new list.
    for (item = va_arg(ap2, ya_t *); item->type != YA_NODE_TYPE_END; item = va_arg(ap2, ya_t *)) {
        switch (item->type) {
        case YA_NODE_TYPE_COUNT:
            fprintf(stderr, "Found YA_NODE_TYPE_COUNT token, which is only allowed for line counting\n");
            abort();

        case YA_NODE_TYPE_LIST:
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

        // Now that the child node is copied in self, we should free() it.
        free(item->node);
    }

    va_end(ap2);

    // Make sure the this node has a position, if not take it from the current position of the parser.
    if (self.position.file == UINT32_MAX) {
        self.position = ya_previous_position;
    }

    return self;
}

ya_t ya_generic_node(const char * restrict name, ya_type_t type, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, type);
    r = ya_generic_nodev(name, type, ap);
    va_end(ap);

    return r;
}

ya_t ya_pass()
{
    return ya_generic_node("pass", YA_NODE_TYPE_PASS, END);
}

ya_t ya_node(const char * restrict name, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, name);
    r = ya_generic_nodev(name, YA_NODE_TYPE_BRANCH, ap);
    va_end(ap);

    return r;
}

ya_t ya_list(const char * restrict name, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, name);
    r = ya_generic_nodev(name, YA_NODE_TYPE_LIST, ap);
    va_end(ap);

    return r;
}

void ya_node_save(FILE *output_file, ya_t *node)
{
    fwrite(node->node, node->size, 1, output_file);
}

