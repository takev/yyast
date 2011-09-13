
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
#include <yyast/utils.h>

ya_t ya_generic_node(ya_t first, ya_t last, uint32_t type, va_list ap)
{
    va_list ap2;
    ya_t    item;
    size_t  item_size = 0;
    ya_t    self;
    size_t  self_size = 0;
    off_t   self_offset = 0;

    // Calculate the size of the content.
    va_copy(ap, ap2);
    item = va_arg(ap2, ya_t);
    while (item.start.position != -1) {
        if (item.type == 0) {
            // Only the content is copied of a list, not its header.
            self_size+= ya_align32(item.size) - sizeof (ya_node_t);
        } else {
            self_size+= ya_align32(item.size);
        }

        item = va_arg(ap2, ya_t);
    }
    va_end(ap2);

    // Create a new list.
    self.start = first.start;
    self.end = last.end;
    self.type = type;
    self.size = self_size + sizeof (ya_node_t);
    // With the content header.
    self.node = calloc(1, ya_align32(self.size));
    self.node->type           = htonl(self.type);
    self.node->size           = htonl(self.size);
    self.node->start.position = htonl(self.start.position);
    self.node->start.line     = htonl(self.start.line);
    self.node->start.column   = htonl(self.start.column);
    self.node->end.position   = htonl(self.end.position);
    self.node->end.line       = htonl(self.end.line);
    self.node->end.column     = htonl(self.end.column);

    // Add the content of the items to the new list.
    item = va_arg(ap, ya_t);
    while (item.start.position != -1) {
        if (item.type == 0) {
            item_size = ya_align32(item.size) - sizeof (ya_node_t);
            memcpy(&(self.node->data[self_offset]), &(item.node->data[sizeof (ya_node_t)]), item_size);
            self_offset+= item_size;
        } else {
            item_size = ya_align32(item.node->size);
            memcpy(&(self.node->data[self_offset]), item.node->data, item_size);
            self_offset+= item_size;
        }
        free(item.node);
        item = va_arg(ap, ya_t);
    }

    return self;
}

ya_t ya_node(ya_t first, ya_t last, uint32_t type, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, type);
    r = ya_generic_node(first, last, type, ap);
    va_end(ap);

    return r;
}

ya_t ya_list(ya_t first, ya_t last, ...)
{
    ya_t   r;
    va_list ap;

    va_start(ap, last);
    r = ya_generic_node(first, last, 0, ap);
    va_end(ap);

    return r;
}

ya_t ya_pass(void)
{
    ya_t r = {
        .start = {.position = -2, .line = 0, .column = 0},
        .end   = {.position =  0, .line = 0, .column = 0}
    };

    r.node = calloc(1, sizeof (ya_node_t));
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
