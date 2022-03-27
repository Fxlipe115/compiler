#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>

#include "data_type.h"
#include "symbol.h"

typedef struct symbol_table symbol_table_t;


symbol_table_t* new_symbol_table();

void delete_symbol_table(symbol_table_t* st);

symbol_t* symbol_table_add(const symbol_table_t* st, const char* symbol, 
                           symbol_type_t type);

symbol_t* symbol_table_add_function_parameter();

symbol_t* symbol_table_get(symbol_table_t* st, const char* key, symbol_t* scope);

bool symbol_table_contains(const symbol_table_t* st, const char* key, 
                           symbol_t* scope);

void symbol_table_print(FILE* stream, const symbol_table_t* st);

#endif
