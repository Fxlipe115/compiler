#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "list.h"
#include "syntax_tree.h"
#include "symbol_table.h"

list_t* generate_code(ast_t* ast, symbol_table_t* st);

void print_code(list_t* code);

#endif