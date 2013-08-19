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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <yyast/node.h>
#include <yyast/header.h>
#include <yyast/count.h>

ya_t ya_header(ya_t *document_node)
{
    // This is executed after the document was parsed, because the document is passed as an argument.
    ya_t    header;
    ya_t    filenames = ya_get_filenames();

    header = YA_BRANCH("yyast", &filenames, document_node);
    ya_clear_position(&header);
    return header;
}

