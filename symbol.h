#ifndef SYMBOL_H
#define SYMBOL_H

#include "data_type.h"

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
    data_type_t data_type;
} symbol_t;

#endif