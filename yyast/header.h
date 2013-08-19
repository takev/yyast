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
#ifndef YA_HEADER_H
#define YA_HEADER_H

#include <stdio.h>
#include <yyast/types.h>

/** Create an header node.
 *
 * @param document_node     The node for the full document.
 * @returns                 The top node, which includes the header and finally the document.
 */
ya_t ya_header(ya_t *document_node);

#define YA_HEADER(document_node)    ya_header(document_node)

#endif
