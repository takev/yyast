/* Copyright (c) 2011-2013, Take Vos
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <yyast/main.h>
#include <yyast/utils.h>
#include <yyast/node.h>
#include <yyast/count.h>
#include <yyast/leaf.h>

extern FILE *yyin;
int yyparse();

char *ya_output_filename = NULL;
char *ya_input_filename = NULL;

void ya_usage(char *application, int exit_code)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -h\n", application);
    fprintf(stderr, "  %s [-c] [-o output file] input file\n", application);
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
            ya_usage(argv[0], 0);
        case 'c':
            // Compile, which is the only mode it supports.
            break;
        case 0:
            break;
        case ':':
            fprintf(stderr, "Missing option argument.");
            ya_usage(argv[0], 2);
        case '?':
            fprintf(stderr, "Unknown option.");
            ya_usage(argv[0], 2);
        default:
            fprintf(stderr, "Unknown error in getopt_long().");
            ya_usage(argv[0], 2);
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        fprintf(stderr, "Expecting a single filename.\n");
        ya_usage(argv[0], 2);
    }

    ya_input_filename = argv[0];

    if (ya_output_filename == NULL) {
        // output file was not explicityly set, therfor we generate it from the input filename.
        ya_output_filename = ya_new_extension(ya_input_filename, extension);
    }
}

int ya_main(int argc, char *argv[], char *extension)
{
    FILE    *out;
    char    *reposition_s;

    // Initialize singletons.
    ya_null_singleton = ya_null();

    ya_parse_options(argc, argv, extension);

    if (strcmp(ya_input_filename, "-") == 0) {
        yyin = stdin;
    } else {
        if ((yyin = fopen(ya_input_filename, "r")) == NULL) {
            perror("Could not open input file");
            return -1;
        }
    }

    if (asprintf(&reposition_s, "1 \"%s\"", ya_input_filename) >= 0) {
        ya_reposition(reposition_s, strlen(reposition_s));
        free(reposition_s);
    }

    yyparse();
    fclose(yyin);

    if (strcmp(ya_output_filename, "-") == 0) {
        out = stdout;
    } else {
        if ((out = fopen(ya_output_filename, "w")) == NULL) {
            perror("Could not open output file");
            return -1;
        }
    }
    ya_node_save(out, &ya_start);
    fclose(out);

    return 0;
}
