#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "list.h"

#define TABLE_SIZE 1009
#define BUCKET_LIST_END -1

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
    list_iterator_t symbol_in_hash_it = list_find(list_begin(st->bucket[bucket_index]), symbol, &symbol_comp);
    symbol_t* symbol_in_hash = list_current(symbol_in_hash_it);
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
    list_iterator_t it = list_begin(st->bucket[bucket_index]);
    symbol_t* symbol = new_symbol(key, 0, 0, scope, 0, NULL);
    it = list_find(it, symbol, &symbol_comp);
    delete_symbol(symbol);
    symbol = list_current(it);
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

size_t next_non_empty_bucket(symbol_table_t* st, size_t current_index) {
    size_t index = current_index + 1;
    while(index < TABLE_SIZE && list_size(st->bucket[index]) == 0) {
        index++;
    }
    if(index == TABLE_SIZE) {
        return BUCKET_LIST_END;
    }
    return index;
}

symbol_table_iterator_t symbol_table_begin(symbol_table_t* st) {
    symbol_table_iterator_t it;
    it.table = st;
    it.current_bucket_index = next_non_empty_bucket(st, -1);
    if (it.current_bucket_index != BUCKET_LIST_END) {
        it.current_bucket = list_begin(st->bucket[it.current_bucket_index]);
    }
    return it;
}

symbol_table_iterator_t symbol_table_next(symbol_table_iterator_t* it) {
    list_iterator_t next = list_next(&(it->current_bucket));
    symbol_t* current = list_current(next);
    if(current == NULL) {
        size_t next_index = next_non_empty_bucket(it->table, it->current_bucket_index);
        if(next_index != BUCKET_LIST_END) {
            it->current_bucket_index = next_index;
            it->current_bucket = list_begin(it->table->bucket[next_index]);
        }
    }
    return *it;
}

symbol_t* symbol_table_current(symbol_table_iterator_t it) {
    return (it.current_bucket_index != BUCKET_LIST_END) ? list_current(it.current_bucket) : NULL;
}