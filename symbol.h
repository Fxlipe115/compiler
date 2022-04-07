#ifndef SYMBOL_H
#define SYMBOL_H

#include "data_type.h"
#include "list.h"

#define SYMBOL_SCOPE_GLOBAL NULL

typedef struct symbol symbol_t;
typedef struct symbol_list symbol_list_t;

typedef enum symbol_type {
    symbol_int_literal,
    symbol_char_literal,
    symbol_string_literal,
    symbol_identifier,
    symbol_variable,
    symbol_vector,
    symbol_function,
    symbol_parameter,
    symbol_label
} symbol_type_t;


struct symbol {
    char* value;
    symbol_type_t type;
    data_type_t data_type;
    symbol_t* scope;
    int first_define_at_line;
    list_t* parameters;
};

symbol_t* new_symbol(const char* value, symbol_type_t type, data_type_t data_type, 
                    symbol_t* scope, int first_defined_at_line, 
                    list_t* parameters);

symbol_t* new_empty_symbol();

void delete_symbol(symbol_t* symbol);

#endif