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
#include <stdarg.h>
#include <yyast/error.h>
#include <yyast/count.h>

void ya_error(const char *message, ...)
{
    char *msg;
    va_list ap;

    va_start(ap, message);
    vasprintf(&msg, message, ap);
    va_end(ap);

    fprintf(stderr, "line %i:%i, %s\n", ya_current_position.line, ya_current_position.column, msg);

    free(msg);
    exit(1);
}

