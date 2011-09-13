
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/token.h>
#include <yyast/count.h>
#include <yyast/utils.h>

ya_t ya_token(uint32_t type, void *buf, size_t buf_size)
{
    ya_t r = {
        .type = type,
        .size = buf_size + sizeof (ya_node_t),
        .start = ya_previous_position,
        .end = ya_current_position
    };

    // Allocate memory aligned to 32 bit. We need to use calloc so that we don't accidently
    // leak information into the output file.
    r.node = calloc(1, ya_align32(r.size));
    r.node->size           = htonl(r.size);
    r.node->type           = htonl(r.type);
    r.node->start.position = htonl(r.start.position);
    r.node->start.line     = htonl(r.start.line);
    r.node->start.column   = htonl(r.start.column);
    r.node->end.position   = htonl(r.end.position);
    r.node->end.line       = htonl(r.end.line);
    r.node->end.column     = htonl(r.end.column);
    memcpy(r.node->data, buf, buf_size);
    return r;
}

