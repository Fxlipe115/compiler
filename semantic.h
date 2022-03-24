#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "syntax_tree.h"
#include "symbol_table.h"

int check_semantic_errors(ast_t* ast, symbol_table_t* st);

#endif