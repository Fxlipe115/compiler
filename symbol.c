#include "symbol.h"

#include <stdlib.h>
#include <string.h>

void copy_symbol_value(const char* value, symbol_t* symbol) {
    size_t value_length = strlen(value);
    symbol->value = calloc(value_length+1, sizeof(char));
    strcpy(symbol->value, value);
    symbol->value[value_length] = '\0';
}

symbol_t* new_symbol(const char* value, symbol_type_t type, data_type_t data_type, 
                    symbol_t* scope, int first_defined_at_line, 
                    list_t* parameters) {
    symbol_t* symbol = malloc(sizeof(symbol_t));
    copy_symbol_value(value, symbol);
    symbol->type = type;
    symbol->data_type = data_type;
    symbol->scope = scope;
    symbol->first_define_at_line = first_defined_at_line;
    symbol->parameters = parameters;
    return symbol;
}


symbol_t* new_empty_symbol() {
    symbol_t* symbol = malloc(sizeof(symbol_t));
    return symbol;
}


void delete_symbol(symbol_t* symbol) {
    free(symbol->value);
    free(symbol);
}
