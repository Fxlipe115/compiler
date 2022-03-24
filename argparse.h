#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>

typedef struct arguments {
    char* source_file;
    char* output_file;
    int print_parser_steps;
    int print_scanner_steps;
    bool print_symbol_table;
    bool print_syntax_table;
} arguments_t;

typedef enum argparse_error {
    argparse_success = 0,
    argparse_invalid_option,
    argparse_missing_positional
} argparse_error_t;

argparse_error_t parse_arguments(int argc, char** argv, arguments_t* arguments);

#endif