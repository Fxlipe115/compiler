#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "parser.h"
#include "scanner.h"
#include "lex_helper_functions.h"
#include "yacc_helper_functions.h"
#include "error_codes.h"
#include "argparse.h"
#include "semantic.h"
#include "code_generator.h"
#include "assembly_generator.h"
#include "tac.h"

int main(int argc, char** argv){
    FILE* source_file;
    FILE* out_file;
    arguments_t args;

    if(parse_arguments(argc, argv, &args) != argparse_success) {
        exit(ARGUMENTS_ERROR);
    }

    source_file = fopen(args.source_file, "r");
    if (source_file == NULL) {
        perror("Error openning source file");
        exit(FILE_OPEN_ERROR);
    }


    out_file = fopen(args.output_file, "w");
    if (out_file == NULL) {
        perror("Error writing to output file");
        exit(FILE_OPEN_ERROR);
    }
    
    symbol_table_t* symbol_table = new_symbol_table();
    ast_t* ast = new_ast();

    lex_init(symbol_table, args.print_parser_steps, args.print_scanner_steps);
    yyset_in(source_file);
    yacc_init(ast);
    yyparse();
    if(syntax_error_occured()) {
        exit(SYNTAX_ERROR);
    }

    int semantic_errors = check_semantic_errors(ast, symbol_table);
    if(args.print_symbol_table) {
        symbol_table_print(stderr, symbol_table);
    }

    if(args.print_syntax_table) {
        ast_print(stderr, ast);
    }

    if(semantic_errors > 0) {
        fprintf(stderr, "Compilation failed.\n");
        exit(SEMANTIC_ERROR);
    } else {

        list_t* code = generate_code(ast, symbol_table);

        if(args.print_symbol_table) {
            symbol_table_print(stderr, symbol_table);
        }

        if(args.print_syntax_table) {
            ast_print(stderr, ast);
        }
        
        if(args.print_tacs_list) {
            print_code(code);
        }

        generate_assembly(out_file, code);
        fprintf(stderr, "File %s created successfully!\n", args.output_file);
        delete_list(code, (void (*)(list_element_t *))&delete_tac);
    }

    delete_symbol_table(symbol_table);
    delete_ast(ast);

    fclose(source_file);
    fclose(out_file);

    exit(EXIT_SUCCESS);
}

