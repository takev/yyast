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
#ifndef YA_NODE_H
#define YA_NODE_H

#include <stdio.h>
#include <yyast/types.h>


/** This is the sentinal to be used as the last parameter of ya_node() and ya_list().
 * In yacc, you will use NODE() and LIST() which will append END implicitly.
 */
#define END             &YA_NODE_END

/** Define a pass node.
 * A pass node is used when one of the ordered childs of a node needs to be empty.
 */
#define PASS            ya_pass()

/** Define a new node.
 * Used in a yacc action to start a new node.
 *
 * @param type  The type/name of the literal, type 0 is reserved by list.
 * @param ...   A list of literals to be included in this node.
 * @returns     A node
 */
#define NODE(name, ...)  ya_node(name, __VA_ARGS__, END)
#define EMPTYNODE(name)  ya_node(name, END)

/** Define a list of literals.
 * Used in yacc to create a list of literals. When adding a list to an other list or node, the contents
 * of the list is appended to the node and list.
 *
 * @param first The first literal, used to mark the position of the literal in the text.
 * @param last  The first literal, used to mark the position of the literal in the text.
 * @param ...   A list of literals to be included in this node.
 * @returns     A list
 */
#define LIST(...)        ya_list("", __VA_ARGS__, END)
#define EMPTYLIST        ya_list("", END)

/** Create an end node.
 * This is a sentinal which should be passed as the last parameter of
 * ya_node() or ya_list()
 */
extern ya_t YA_NODE_END;

/** Create a pass node.
 * A pass node is used for empty entries in an ordered node child list.
 */
ya_t ya_pass();

/** Create an ya node.
 * Create a node from subnodes.
 * This also free memory used by the subnodes.
 *
 * @param type  fourcc code for the ya node
 *              When type is 0 the header is not preserved when used as subnode.
 * @param ...   The subnodes, ending with NULL
 * @returns     A new AST NODE.
 */
ya_t ya_node(const char * restrict name, ...);

/** Create an ya node.
 * Create a list of subnodes, when included in a list or node, it expands as if the contents
 * was passed as seperate items.
 * This also free memory used by the subnodes.
 *
 * @param type  fourcc code for the ya node
 * @param ...   The subnodes, ending with NULL
 * @returns     A new AST NODE.
 */
ya_t ya_list(const char * restrict name, ...);

/** Save the node to a file.
 *
 * @param output_file   A file pointer of an file open for writing.
 * @param node          The node to be saved to file.
 */
void ya_node_save(FILE *output_file, ya_t *node);

#endif
