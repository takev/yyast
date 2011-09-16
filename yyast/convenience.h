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
#ifndef YA_CONVENIENCE_H
#define YA_CONVENIENCE_H

#include <yyast/types.h>

/** Define an unsued parameter in a node or list.
 */
#define END &YA_NODE_END

/** This is the sentinal to be used as the last parameter of ya_node() and ya_list().
 * In yacc, you will use NODE() and LIST() which will append END implicitly.
 */
#define PASS &YA_NODE_PASS

/** Define a new node.
 * Used in a yacc action to start a new node.
 *
 * @param first The first literal, used to mark the position of the literal in the text.
 * @param last  The first literal, used to mark the position of the literal in the text.
 * @param type  The type/name of the literal, type 0 is reserved by list.
 * @param ...   A list of literals to be included in this node.
 * @returns     A node
 */
#define NODE(first, last, type, ...)    ya_node(first, last, fourcc(type), __VA_ARGS__, END)

#define EMPTYNODE(first, last, type)    ya_node(first, last, fourcc(type), END)

/** Define a list of literals.
 * Used in yacc to create a list of literals. When adding a list to an other list or node, the contents
 * of the list is appended to the node and list.
 *
 * @param first The first literal, used to mark the position of the literal in the text.
 * @param last  The first literal, used to mark the position of the literal in the text.
 * @param ...   A list of literals to be included in this node.
 * @returns     A list
 */
#define LIST(first, last, ...)          ya_list(first, last, __VA_ARGS__, END)

/** Define an empty list.
 */
#define EMPTYLIST                       ya_list(END, END, END)

/** Create literal for a floating point literal.
 * The literal is of type '#f64', and contains the decoded number as a 64 bit floating point.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_real(char *buf, size_t buf_size);

/** Create literal for an integer literal.
 * The literal is of type '#i64' or '#u64', and contains the decoded number as a 64 bit integer,
 * or 64 bit unsigned integer.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_int(char *buf, size_t buf_size);

/** Create literal for a string literal.
 * The literal is of type '#str', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \n, \r, \t, \x??, \u????, \U????????, \\, \", \/
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_string(char *buf, size_t buf_size);

/** Create literal for a raw string literal.
 * The literal is of type '#str', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \"
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_raw_string(char *buf, size_t buf_size);

/** Create literal for a regex literal.
 * The literal is of type '#re ', and contains a unescaped string encoded as UCS-4.
 * Escape sequences which are recognized are: \/
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_regex(char *buf, size_t buf_size);

/** Create literal for a name.
 * The literal is of type '#id ', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_name(char *buf, size_t buf_size);

/** Create literal for assembly.
 * The literal is of type '#asm', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_assembly(char *buf, size_t buf_size);

/** Create literal for comments.
 * The literal is of type '#doc', and contains a string encoded as UCS-4.
 *
 * @param buf       Literal as string.
 * @param buf_size  String size.
 * @returns         The decoded literal.
 */
ya_t ya_comment(char *buf, size_t buf_size);

#endif
