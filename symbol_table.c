#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "list.h"

#define TABLE_SIZE 1009

struct symbol_table {
    list_t** bucket;
};

size_t hash(const char* key);


symbol_table_t* new_symbol_table() {
    symbol_table_t* st = malloc(sizeof(symbol_table_t));
    st->bucket = calloc(TABLE_SIZE, sizeof(list_t*));

    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        st->bucket[bucket_index] = new_list();
    }

    return st;
}


void delete_symbol_table(symbol_table_t* st) {
    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        delete_list(st->bucket[bucket_index], (void (*)(void*))&delete_symbol);
    } 
    free(st->bucket);
    free(st);
}


symbol_t* symbol_table_add(const symbol_table_t* st, const char* symbol, 
                           symbol_type_t type, int first_defined_at_line) {
    return symbol_table_add_with_scope(st, symbol, type, first_defined_at_line, SYMBOL_SCOPE_GLOBAL);
}


bool symbol_comp(list_element_t* s1, list_element_t* s2) {
    return strcmp(((symbol_t*)s1)->value, ((symbol_t*)s2)->value) == 0 && 
           ((symbol_t*)s1)->scope == ((symbol_t*)s2)->scope;
}


symbol_t* symbol_table_add_symbol(const symbol_table_t* st, symbol_t* symbol) {
    size_t bucket_index = hash(symbol->value);
    list_iterator_t* symbol_in_hash_it = list_find(list_begin(st->bucket[bucket_index]), symbol, &symbol_comp);
    symbol_t* symbol_in_hash = list_current(symbol_in_hash_it);
    delete_list_iterator(symbol_in_hash_it);
    if(symbol_in_hash == NULL) {
        list_push_back(st->bucket[bucket_index], symbol);
        return symbol;
    } else {
        delete_symbol(symbol);
        return symbol_in_hash;
    }
}


symbol_t* symbol_table_add_with_scope(const symbol_table_t* st, const char* value, 
                           symbol_type_t type, int first_defined_at_line, symbol_t* scope){
    symbol_t* symbol = new_symbol(value, type, data_type_undefined, scope, first_defined_at_line, NULL);

    return symbol_table_add_symbol(st, symbol);
}


symbol_t* symbol_table_get(symbol_table_t* st, char* key, symbol_t* scope) {
    size_t bucket_index = hash(key);
    list_iterator_t* it = list_begin(st->bucket[bucket_index]);
    symbol_t* symbol = new_symbol(key, 0, 0, scope, 0, NULL);
    list_find(it, symbol, &symbol_comp);
    delete_symbol(symbol);
    symbol = list_current(it);
    delete_list_iterator(it);
    return symbol;
}


bool symbol_table_contains(symbol_table_t* st, char* key, symbol_t* scope) {
    return symbol_table_get(st, key, scope) != NULL;
}

void symbol_bucket_printer(list_element_t* element) {
    symbol_t* symbol = element;
    fprintf(stderr, "{%s, %d, %d, %s} ",
            symbol->value, 
            symbol->type, 
            symbol->data_type, 
            symbol->scope == SYMBOL_SCOPE_GLOBAL ?
                "GLOBAL" : symbol->scope->value);
}

void symbol_table_print(FILE* stream, const symbol_table_t* st) {
    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        if (list_size(st->bucket[bucket_index]) > 0) {
            fprintf(stream, "[ %d ] : ", bucket_index);
            list_print(st->bucket[bucket_index], &symbol_bucket_printer);
            fprintf(stream, "\n");
        }    
    }
}


size_t hash(const char* key) {
    int calculated_hash = 1;
    size_t key_length = strlen(key);
    for(int i = 0; i < key_length; i++) {
        calculated_hash = ((calculated_hash * key[i]) % TABLE_SIZE) + 1;
    }
    return calculated_hash - 1;
}

