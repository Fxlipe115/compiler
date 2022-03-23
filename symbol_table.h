#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct symbol_table symbol_table_t;

// workaround for y.tab.h not containing symbol_t and ast_node_t definitions
typedef struct symbol symbol_t;
typedef struct ast_node ast_node_t;
#include "y.tab.h"
struct symbol {
    char* value;
    enum yytokentype type;
};

symbol_table_t* new_symbol_table();

void delete_symbol_table(symbol_table_t* st);

symbol_t* symbol_table_add(const symbol_table_t* st, const char* symbol, 
                           enum yytokentype type);

bool symbol_table_contains(const symbol_table_t* st, const char* key);

void symbol_table_print(FILE* stream, const symbol_table_t* st);

#endif
