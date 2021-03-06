/* Copyright (c) 2011-2013, Take Vos
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <yyast/yyast.h>

typedef union {
    int64_t     i;
    uint64_t    u;
    double      d;
    char        c[8];
} type64_t;

void indent(unsigned int level)
{
    unsigned int    i;

    for (i = 0; i < level; i++) {
        fprintf(stdout, "  ");
    }
}

void print_name(ya_name_t name)
{
    fprintf(stdout, "'%c%c%c%c%c%c%c%c'",
        (char)(name >> 56) & 0xff,
        (char)(name >> 48) & 0xff,
        (char)(name >> 40) & 0xff,
        (char)(name >> 32) & 0xff,
        (char)(name >> 24) & 0xff,
        (char)(name >> 16) & 0xff,
        (char)(name >>  8) & 0xff,
        (char)(name      ) & 0xff
    );
}

off_t node_decode(char *buf, size_t buf_size, unsigned int level)
{
    if (buf_size < sizeof (ya_node_t)) {
        fprintf(stdout, "!error size to small to decode header.\n");
        exit(1);
    }

    ya_node_t           *node = (ya_node_t *)buf;
    ya_name_t           name = ntohll(node->name);
    ya_type_t           type = node->type;
    ya_position_t       position = {.file = ntohl(node->position.file), .line = ntohl(node->position.line), .column = ntohl(node->position.column)};
    size_t              inner_size = ntohll(node->size) - sizeof (ya_node_t);
    off_t               inner_offset = 0;
    type64_t            t64;
    char                *s;

    if (inner_size > buf_size) {
        // We use stdout, so that the error is synchronized to the output data.
        fprintf(stdout, "!error inner_size (%llu) larger than buffer_size (%llu)\n",
            (unsigned long long)inner_size,
            (unsigned long long)buf_size
        );
        exit(1);
    }

    if (position.file != UINT32_MAX) {
        fprintf(stdout, "%2lu:%4lu:%3lu", (long)position.file, (long)position.line, (long)position.column);
    } else {
        fprintf(stdout, "           ");
    }

    fprintf(stdout, " (%8llu)", (unsigned long long)inner_size);

    indent(level);
    fprintf(stdout, " ");
    print_name(name);

    switch (type) {
    case YA_NODE_TYPE_POSITIVE_INTEGER:
        if (inner_size == sizeof (t64)) {
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " +%lli\n", (long long int)t64.i);
        } else {
            fprintf(stdout, " +i%i\n", (int)inner_size);
        }
        break;
    case YA_NODE_TYPE_NEGATIVE_INTEGER:
        if (inner_size == sizeof (t64)) {
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " -%llu\n", (long long unsigned)t64.u);
        } else {
            fprintf(stdout, " -i%i\n", (int)inner_size);
        }
        break;
    case YA_NODE_TYPE_BINARY_FLOAT:
        if (inner_size == sizeof (t64)) {
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " %lf\n", t64.d);
        } else {
            fprintf(stdout, " bf%i\n", (int)inner_size);
        }
        break;
    case YA_NODE_TYPE_TEXT:
        s = strndup(node->data, inner_size);
        fprintf(stdout, " \"%s\"\n", s);
        free(s);
        break;
    case YA_NODE_TYPE_NULL:
        fprintf(stdout, " pass\n");
        break;
    case YA_NODE_TYPE_BRANCH:
        // we continue until a header will not fit in the data anymore.
        fprintf(stdout, "\n");
        while (inner_offset < inner_size) {
            // decode the inner nodes.
            inner_offset+= node_decode(&node->data[inner_offset], inner_size - inner_offset, level + 1);
        }
        break;
    default:
        fprintf(stderr, " *unknown*\n");
        break;
    }

    // Return the actual size occupied.
    return ya_align64(ntohll(node->size));
}

int main(int argc, char *argv[])
{
    int         fd;
    struct stat fd_st;
    char        *buf;
    size_t      buf_size;

    if (argc != 2) {
        fprintf(stderr, "Expect 1 filename as argument.\n");
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file.");
        exit(1);
    }

    if (fstat(fd, &fd_st) == -1) {
        perror("Could not stat the file.");
        exit(1);
    }
    buf_size = fd_st.st_size;

    if ((buf = mmap(0, buf_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("Could not map the file in memory.");
        exit(1);
    }

    // Start decoding the file.
    (void)node_decode(buf, buf_size, 0);

    if (munmap(buf, buf_size) == -1) {
        perror("Could not unmap the file from memory.");
        exit(1);
    }

    if (close(fd) == -1) {
        perror("Close the file.");
        exit(1);
    }
}

