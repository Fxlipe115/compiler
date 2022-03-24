#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct symbol_table symbol_table_t;

typedef enum symbol_type {
    symbol_int_literal,
    symbol_char_literal,
    symbol_string_literal,
    symbol_identifier,
    symbol_variable,
    symbol_vector,
    symbol_function
} symbol_type_t;

typedef struct symbol {
    char* value;
    symbol_type_t type;
} symbol_t;

symbol_table_t* new_symbol_table();

void delete_symbol_table(symbol_table_t* st);

symbol_t* symbol_table_add(const symbol_table_t* st, const char* symbol, 
                           symbol_type_t type);

bool symbol_table_contains(const symbol_table_t* st, const char* key);

void symbol_table_print(FILE* stream, const symbol_table_t* st);

#endif
