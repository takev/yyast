#ifndef YA_TOKEN_H
#define YA_TOKEN_H

#include <yyast/types.h>


/** Create an token.
 *
 * @param type      fourcc code for the ya leaf
 * @param buf       The memory which needs to be copied.
 * @param buf_size  The amount of memory to be copied.
 */
ya_t ya_token(uint32_t type, void *buf, size_t buf_size);

#endif
