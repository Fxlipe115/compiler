#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>
#include "symbol_list.h"

#define TABLE_SIZE 1009

struct symbol_table {
    symbol_list_t** bucket;
};

size_t hash(const char* key);


symbol_table_t* new_symbol_table() {
    symbol_table_t* st = malloc(sizeof(symbol_table_t));
    st->bucket = calloc(TABLE_SIZE, sizeof(symbol_list_t*));

    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        st->bucket[bucket_index] = new_symbol_list();
    }

    return st;
}


void delete_symbol_table(symbol_table_t* st) {
    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        delete_symbol_list(st->bucket[bucket_index]);
    } 
    free(st->bucket);
    free(st);
}


symbol_t* symbol_table_add(const symbol_table_t* st, const char* symbol, 
                           symbol_type_t type) {
    size_t bucket_index = hash(symbol);
    if(!symbol_table_contains(st, symbol)) {
        return symbol_list_add(st->bucket[bucket_index], symbol, type);
    } else {
        return symbol_list_search(st->bucket[bucket_index], symbol);
    }
}


symbol_t* symbol_table_get(symbol_table_t* st, const char* key) {
    size_t bucket_index = hash(key);
    return symbol_list_search(st->bucket[bucket_index], key);
}


bool symbol_table_contains(const symbol_table_t* st, const char* key) {
    size_t bucket_index = hash(key);
    return symbol_list_contains(st->bucket[bucket_index], key);
}


void symbol_table_print(FILE* stream, const symbol_table_t* st) {
    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        if (symbol_list_get_size(st->bucket[bucket_index]) > 0) {
            fprintf(stream, "[ %d ] : ", bucket_index);
            symbol_list_print(stream, st->bucket[bucket_index]);
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

