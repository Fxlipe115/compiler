#ifndef SYMBOL_LIST_H
#define SYMBOL_LIST_H

#include <stdio.h>
#include <stdbool.h>
#include "symbol.h"

#define SYMBOL_LIST_END NULL

typedef struct symbol symbol_t;
typedef enum symbol_type symbol_type_t;

typedef struct symbol_list symbol_list_t;

symbol_list_t* new_symbol_list();

void delete_symbol_list(symbol_list_t* sl);

symbol_t* symbol_list_add(symbol_list_t* sl, const char* value, 
                          symbol_type_t type, int first_defined_at_line, symbol_t* scope);

symbol_t* symbol_list_add_symbol(symbol_list_t* sl, symbol_t* symbol);

bool symbol_list_contains(symbol_list_t* sl, const char* key, symbol_t* scope);

symbol_t* symbol_list_search(symbol_list_t* sl, const char* key, symbol_t* scope);

void symbol_list_print(FILE* stream, symbol_list_t* sl);

size_t symbol_list_get_size(symbol_list_t* st);


typedef struct symbol_list_iterator symbol_list_iterator_t;

symbol_list_iterator_t* new_symbol_list_iterator(symbol_list_t* sl);

void delete_symbol_list_iterator(symbol_list_iterator_t* it);

symbol_t* symbol_list_iterator_get(symbol_list_iterator_t* it);

void symbol_list_iterator_next(symbol_list_iterator_t* it);

#endif