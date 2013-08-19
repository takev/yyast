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

/** Define a new node.
 * Used in a yacc action to start a new node.
 * If a list is added to a branch, then the nodes of the list are added to the created
 * node, instead of the list itself.
 *
 * @param name  Name of the node
 * @param ...   A set of nodes, leaves or lists to be included in this node.
 * @returns     A node
 */
#define YA_BRANCH(name, ...)  ya_branch(name, __VA_ARGS__, NULL)

/** A branch that is empty.
 * Used if a node normally has children, except in this case.
 *
 * @param name  Name of the node.
 * @return      A empty node.
 */
#define YA_EMPTYBRANCH(name)  ya_branch(name, NULL)

/** Define a list of nodes.
 * Used in yacc to create a list of nodes. When adding a list to an other list or node, the contents
 * of the list is appended to the node and list.
 *
 * @param ...   A list of literals to be included in this node.
 * @returns     A list
 */
#define YA_LIST(...)        ya_list("@list", __VA_ARGS__, NULL)

/** An empty list contains no nodes, but can be used to start a sequence of nodes in the grammar.
 */
#define YA_EMPTYLIST        ya_list("@list", NULL)

/** Create an ya node.
 * Create a node from subnodes.
 * This will free memory used by the subnodes.
 * If a list is added to a branch, then the nodes of the list are added to the created
 * node, instead of the list itself.
 *
 * @param name  Name of the node.
 * @param ...   The subnodes, ending with NULL
 * @returns     A new AST NODE.
 */
ya_t ya_branch(const char * restrict name, ...);

/** Create an ya node.
 * Create a list of subnodes, when included in a list or node, it expands as if the contents
 * was passed as seperate items.
 * This also free memory used by the subnodes.
 *
 * @param name  Name of the node. This should be '\@list' because this function is called from the YA_LIST macro.
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
