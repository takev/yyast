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

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
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

void print_fourcc(fourcc_t type)
{
    fprintf(stdout, "%c%c%c%c",
        (type >> 24) & 0xff,
        (type >> 16) & 0xff,
        (type >>  8) & 0xff,
        (type      ) & 0xff
    );
}

off_t node_decode(char *buf, size_t buf_size, unsigned int level)
{
    ya_node_t           *node = (ya_node_t *)buf;
    fourcc_t            type = ntohl(node->type);
    ya_short_position_t position = ntohll(node->position);
    size_t              inner_size = ntohl(node->length) - sizeof (ya_node_t);
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

    if (position < 0x8000000000000000) {
        fprintf(stdout, "%4llu:%3llu -%4llu:%3llu",
            ((position >> 24) & 0xffffL) + 1,
            ((position >> 16) & 0xffULL) + 1,
            ((position >> 24) & 0xffffL) + ((position >> 8) & 0xffUL) + 1,
            ((position      ) & 0xffULL) + 1
        );

    } else {
        fprintf(stdout, "%4llu:%3llu          ",
            ((position >> 8) & 0xffffffULL) + 1,
            ((position     ) & 0xffULL) + 1
        );
    }

    fprintf(stdout, " (%4llu)", (unsigned long long)inner_size);

    indent(level);
    fprintf(stdout, " ");
    print_fourcc(type);

    if ((type >> 24) == '#') {
        // This is a literal, decode the ones we know, show bytes of the ones we don't.
        switch (type) {
        case fourcc('#','i','6','4'):
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " %lli\n", t64.i);
            break;
        case fourcc('#','u','6','4'):
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " %llu\n", t64.u);
            break;
        case fourcc('#','f','6','4'):
            memcpy(t64.c, node->data, sizeof (t64));
            t64.u = ntohll(t64.u);
            fprintf(stdout, " %lf\n", t64.d);
            break;
        case fourcc('#','s','t','r'):
        case fourcc('#','r','e',' '):
        case fourcc('#','i','d',' '):
        case fourcc('#','a','s','m'):
        case fourcc('#','d','o','c'):
            s = strndup(node->data, inner_size);
            fprintf(stdout, " '%s'\n", s);
            free(s);
            break;
        default:
            // print all the bytes.
            for (; inner_offset < inner_size; inner_offset++) {
                if ((inner_offset % 32) == 0) {
                    fprintf(stdout, "\n");
                    indent(level + 1);
                    fprintf(stdout, "                         ");
                }
                fprintf(stdout, " 0x%02hhx", node->data[inner_offset]);
            }
            fprintf(stdout, "\n");
        }
    } else {
        // we continue until a header will not fit in the data anymore.
        fprintf(stdout, "\n");
        while (inner_offset + sizeof (ya_node_t) < inner_size) {
            // decode the inner nodes.
            inner_offset+= node_decode(&node->data[inner_offset], inner_size - inner_offset, level + 1);
        }
    }

    // Return the actual size occupied.
    return ya_align64(ntohl(node->length));
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

