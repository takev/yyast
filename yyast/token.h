#ifndef YA_TOKEN_H
#define YA_TOKEN_H

#include <yyast/types.h>


/** Create an integer ya leaf.
 *
 * @param type      fourcc code for the ya leaf
 * @param buf       The memory which needs to be copied.
 * @param buf_size  The amount of memory to be copied.
 */
ya_node_t *ya_leaf(uint32_t type, void *buf, size_t buf_size);

ya_t ya_real(char *buf, size_t buf_size);
ya_t ya_int(char *buf, size_t buf_size);
ya_t ya_string(char *buf, size_t buf_size);
ya_t ya_raw_string(char *buf, size_t buf_size);
ya_t ya_regex(char *buf, size_t buf_size);
ya_t ya_name(char *buf, size_t buf_size);
ya_t ya_assembly(char *buf, size_t buf_size);

#endif
