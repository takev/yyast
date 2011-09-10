#ifndef YA_CONVENIENCE_H
#define YA_CONVENIENCE_H

#include <yyast/types.h>

/** Define an unsued parameter in a node or list.
 */
#define PASS                           ya_pass()

/** This is the sentinal to be used as the last parameter of ya_node() and ya_list().
 * In yacc, you will use NODE() and LIST() which will append END implicitly.
 */
#define END                            ya_end()

/** Define a new node.
 * Used in a yacc action to start a new node.
 *
 * @param first The first token, used to mark the position of the token in the text.
 * @param last  The first token, used to mark the position of the token in the text.
 * @param type  The type/name of the token, type 0 is reserved by list.
 * @param ...   A list of tokens to be included in this node.
 * @returns     A node
 */
#define NODE(first, last, type, ...)    ya_node(first, last, type, ##__VA_ARGS__, END)

/** Define a list of tokens.
 * Used in yacc to create a list of tokens. When adding a list to an other list or node, the contents
 * of the list is appended to the node and list.
 *
 * @param first The first token, used to mark the position of the token in the text.
 * @param last  The first token, used to mark the position of the token in the text.
 * @param ...   A list of tokens to be included in this node.
 * @returns     A list
 */
#define LIST(first, last, ...)          ya_list(first, last, ##__VA_ARGS__, END)

/** Create taken for a floating point literal.
 * The token is of type 'real', and contains the decoded number as a 64 bit floating point.
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_real(char *buf, size_t buf_size);

/** Create taken for an integer literal.
 * The token is of type 'int ' or 'uint', and contains the decoded number as a 64 bit integer,
 * or 64 bit unsigned integer.
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_int(char *buf, size_t buf_size);

/** Create taken for a string literal.
 * The token is of type 'str ', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \n, \r, \t, \x??, \u????, \U????????, \\, \", \/
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_string(char *buf, size_t buf_size);

/** Create taken for a raw string literal.
 * The token is of type 'str ', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \"
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_raw_string(char *buf, size_t buf_size);

/** Create taken for a regex literal.
 * The token is of type 'regx', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \/
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_regex(char *buf, size_t buf_size);

/** Create taken for a name.
 * The token is of type 'regx', and contains a string encoded as UCS-4.
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_name(char *buf, size_t buf_size);

/** Create taken for assembly.
 * The token is of type 'asm ', and contains a string encoded as UCS-4.
 *
 * @param buf       Token as string.
 * @param buf_size  String size.
 * @returns         The decoded token.
 */
ya_t ya_assembly(char *buf, size_t buf_size);

#endif
