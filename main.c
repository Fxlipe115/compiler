#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "parser.h"
#include "scanner.h"
#include "lex_helper_functions.h"
#include "yacc_helper_functions.h"
#include "error_codes.h"

int main(int argc, char** argv){
    FILE* source_file;
    FILE* out_file;

    const int print_parser_steps = 0;
    const int print_scanner_steps = 0;
    const bool print_symbol_table = false;
    const bool print_syntax_table = false;

    if (argc < 3 || argc > 3) {
        fprintf(stderr, "Usage: etapa3 SOURCEFILE OUTFILE");
        exit(FILES_NOT_PROVIDED);
    }

    source_file = fopen(argv[1], "r");
    if (source_file == NULL) {
        perror("Error openning source file");
        exit(FILE_OPEN_ERROR);
    }


    out_file = fopen(argv[2], "w");
    if (out_file == NULL) {
        perror("Error writing to output file");
        exit(FILE_OPEN_ERROR);
    }
    
    symbol_table_t* symbol_table = new_symbol_table();
    ast_t* ast = new_ast();

    lex_init(symbol_table, print_parser_steps, print_scanner_steps);
    yyset_in(source_file);
    yacc_init(ast);
    yyparse();

    if(print_symbol_table) {
        symbol_table_print(stderr, symbol_table);
    }

    if(print_syntax_table) {
        ast_print(stderr, ast);
    }

    decompile(out_file, ast);

    delete_symbol_table(symbol_table);
    delete_ast(ast);

    fclose(source_file);
    fclose(out_file);

    printf("File %s created successfully!\n", argv[2]);
    exit(0);
}

