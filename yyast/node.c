
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/node.h>

ya_t ya_generic_node(ya_t first, ya_t lya, uint32_t type, va_list ap)
{
    va_list ap2;
    ya_t   item;
    size_t  item_size = 0;
    ya_t   self;
    size_t  self_size = 0;
    off_t   self_offset = 0;

    // Calculate the size of the content.
    va_copy(ap, ap2);
    item = va_arg(ap2, ya_t);
    while (item.start.position != -1) {
        if (item.node->type == 0) {
            // Only the content is copied of a list, not its header.
            self_size+= align_size(item.node->size) - sizeof (ya_node_t);
        } else {
            self_size+= align_size(item.node->size);
        }

        item = va_arg(ap2, ya_t);
    }
    va_end(ap2);

    // Create a new list.
    self.node = malloc(align_size(self_size + sizeof (ya_node_t)));
    self.node->type = type;
    self.node->size = self_size + sizeof (ya_node_t);;
    self.start = self.node->start = first.start;
    self.end = self.end->start = lya.end;

    // Add the content of the items to the new list.
    item = va_arg(ap, ya_t);
    while (item.start.position != -1) {
        if (item.node->type == 0) {
            item_size = align_size(item.node->size) - sizeof (ya_node_t);
            memcpy(&(self.node->data[self_offset]), item.node->data, item_size);
            self_offset+= item_size;
        } else {
            item_size = align_size(item.node->size);
            memcpy(&(self.node->data[self_offset]), item, item_size);
            self_offset+= item_size;
        }
        free(item.node);
        item = va_arg(ap, ya_t);
    }

    return self;
}

ya_t ya_node(ya_t first, ya_t lya, uint32_t type, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap);
    r = ya_generic_node(first, lya, type, ap);
    va_end(ap);

    return r;
}

ya_t ya_list(ya_t first, ya_t lya, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap);
    r = ya_generic_node(first, lya, 0, ap);
    va_end(ap);

    return r;
}

ya_t ya_pass(void)
{
    ya_t r = {
        .start = {.position = -2, .line = 0, .column = 0},
        .end   = {.position =  0, .line = 0, .column = 0}
    };

    r.node = malloc(sizeof (ya_node_t));
    r.node->type = 'pass';
    r.node->size = sizeof (ya_node_t);
    r.node->start = r.start;
    r.node->end = r.end;
    return r;
}

ya_t ya_end(void)
{
    ya_t r = {
        .start = {.position = -1, .line = 0, .column = 0},
        .end   = {.position =  0, .line = 0, .column = 0}
    };

    r.node = NULL;
    return r;
}
