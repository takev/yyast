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
#include <getopt.h>
#include <yyast/main.h>
#include <yyast/utils.h>
#include <yyast/node.h>

extern FILE *yyin;
int yyparse();

char *ya_output_filename = NULL;
char *ya_input_filename = NULL;

void ya_usage(int exit_code)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  application -h\n");
    fprintf(stderr, "  application [-c] [-o output file] input file\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h   Show help message\n");
    fprintf(stderr, "  -c   Compile, this option is ignored\n");
    fprintf(stderr, "  -o   Set the output file, the default is the same as the input file\n");
    fprintf(stderr, "\n");
    exit(exit_code);
}

void ya_parse_options(int argc, char *argv[], char *extension)
{
    int             ch;
    struct option   longopts[] = {
        {"output",  required_argument, NULL, 'o'},
        {"compile", no_argument,       NULL, 'c'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL,      0,                 NULL, 0}
    };

    while ((ch = getopt_long(argc, argv, "hco:", longopts, NULL)) != -1) {
        switch (ch) {
        case 'o':
            // Set the output filename.
            ya_output_filename = optarg;
            break;
        case 'h':
            // Help, just shows usage without error.
            ya_usage(0);
        case 'c':
            // Compile, which is the only mode it supports.
            break;
        case 0:
            break;
        case ':':
            fprintf(stderr, "Missing option argument.");
            ya_usage(2);
        case '?':
            fprintf(stderr, "Unknown option.");
            ya_usage(2);
        default:
            fprintf(stderr, "Unknown error in getopt_long().");
            ya_usage(2);
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        fprintf(stderr, "Expecting a single filename.\n");
        ya_usage(2);
    }

    ya_input_filename = argv[0];

    if (ya_output_filename == NULL) {
        // output file was not explicityly set, therfor we generate it from the input filename.
        ya_output_filename = ya_new_extension(ya_input_filename, extension);
    }
}

int ya_main(int argc, char *argv[], char *extension)
{
    FILE *out;

    ya_parse_options(argc, argv, extension);
    if ((yyin = fopen(ya_input_filename, "r")) == NULL) {
        perror("Could not open input file");
        return -1;
    }
    yyparse();
    fclose(yyin);

    if ((out = fopen(ya_output_filename, "w")) == NULL) {
        perror("Could not open output file");
        return -1;
    }
    ya_node_save(out, &ya_start);
    fclose(out);

    return 0;
}
