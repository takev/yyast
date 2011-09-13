#ifndef YA_NODE_H
#define YA_NODE_H

#include <yyast/types.h>

/** Create an empty node.
 * It denotes an empty node.
 *
 * @returns     A new AST PASS.
 */
ya_t ya_pass(void);

/** Create an end node.
 * This is a sentinal which should be passed as the lya parameter of
 * ya_node() or ya_list()
 *
 * @returns     A new AST PASS.
 */
ya_t ya_end(void);

/** Create an ya node.
 * Create a node from subnodes.
 * This also free memory used by the subnodes.
 *
 * @param type  fourcc code for the ya node
 *              When type is '----' the header is not preserved when used as subnode.
 * @param ...   The subnodes, ending with NULL
 * @returns     A new AST NODE.
 */
ya_t ya_node(ya_t start, ya_t end, uint32_t type, ...);

/** Create an ya node.
 * Create a list of subnodes, when included in a list or node, it expands as if the contents
 * was passed as seperate items.
 * This also free memory used by the subnodes.
 *
 * @param ...   The subnodes, ending with NULL
 * @returns     A new AST NODE.
 */
ya_t ya_list(ya_t start, ya_t end, ...);

/** Count characters.
 * This functions keeps track of byte position, line and column.
 * The byte, line and columns are zero index.
 * This function works with UTF-8.
 *
 * @param s         The string to analyze
 * @param s_length  The length of the string in bytes.
 * @returns         An initialized ya structure without a node.
 */
ya_t ya_count(char *s, size_t s_length);

#endif
