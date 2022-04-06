#include "argparse.h"
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <libgen.h>

void print_usage(char* program_name);

void print_help(char* program_name);

argparse_error_t parse_arguments(int argc, char** argv, arguments_t* arguments) {
    arguments->print_parser_steps = 0;
    arguments->print_scanner_steps = 0;
    arguments->print_symbol_table = false;
    arguments->print_syntax_table = false;

    int c = 0;
    while (c != -1) {
      static struct option long_options[] =
        {
          {"print-parser-steps", no_argument, NULL, 'p'},
          {"print-scanner_steps", no_argument, NULL, 's'},
          {"print-symbol_table", no_argument, NULL, 't'},
          {"print-syntax_tree", no_argument, NULL, 'a'},
          {"help", no_argument, NULL, 'h'},
          {"usage", no_argument, NULL, 'u'},
          {0, 0, 0, 0}
        };
      
        int option_index = 0;

        c = getopt_long (argc, argv, "pstah",
                         long_options, &option_index);

        switch (c) {
            case 'p':
                arguments->print_parser_steps = 1;
                break;
            case 's':
                arguments->print_scanner_steps = 1;
                break;
            case 't':
                arguments->print_symbol_table = true;
                break;
            case 'a':
                arguments->print_syntax_table = true;
                break;
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'u':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case '?':
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        
        }
    }

    if(argc - optind != 2) {
        print_usage(argv[0]);
        return argparse_missing_positional;
    } else {
        arguments->source_file = argv[optind];
        arguments->output_file = argv[optind+1];
    }

    return argparse_success;
}

const char* usage_string() {
    return "Usage: %s [OPTION...] source_file output_file";
}

void print_usage(char* program_name) {
    fprintf(stderr, "Usage: %s [OPTION...] source_file output_file\n"
            "Try `%s --help' for more information.\n", 
            basename(program_name), program_name);
}

void print_help(char* program_name) {
    fprintf(stderr, "Usage: %s [OPTION...] source_file output_file\n"
            "    UFRGS Compilers discipline assignment.\n"
            "\n"
            "    -a, --print-syntax-tree    Print the Abstract Syntax Tree generated\n"
            "                               from the source file code\n"
            "    -p, --print-parser-steps   Print YACC parsing steps\n"
            "    -s, --print-scanner-steps  Print LEX parsing steps\n"
            "    -t, --print-symbol-table   Print all the contents of the parser's\n"
            "                               symbol table\n"
            "    -h, --help                 Give this help list\n"
            "        --usage                Give a short usage message\n"
            "\n"
            "Author: Felipe de Almeida Graeff.\n"
            "\n"
            "Report bugs to <felipe.graeff@inf.ufrgs.br>.\n",
            basename(program_name));
}

