#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define TABLE_SIZE 1009

typedef struct symbol_list_node symbol_list_node_t;

typedef struct symbol_list symbol_list_t;

struct symbol_list_node {
    symbol_t* symbol;
    struct symbol_list_node* next;
};

struct symbol_list {
    symbol_list_node_t* first;
    size_t size;
};

struct symbol_table {
    symbol_list_t** bucket;
};

symbol_list_t* new_symbol_list();

void delete_symbol_list(symbol_list_t* sl);

symbol_t* symbol_list_add(symbol_list_t* sl, const char* value, 
                          symbol_type_t type);

bool symbol_list_contains(symbol_list_t* sl, const char* key);

symbol_t* symbol_list_get(symbol_list_t* sl, const char* key);

void symbol_list_print(FILE* stream, symbol_list_t* sl);

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
        return symbol_list_get(st->bucket[bucket_index], symbol);
    }
}


symbol_t* symbol_table_get(symbol_table_t* st, const char* key) {
    size_t bucket_index = hash(key);
    return symbol_list_get(st->bucket[bucket_index], key);
}


bool symbol_table_contains(const symbol_table_t* st, const char* key) {
    size_t bucket_index = hash(key);
    return symbol_list_contains(st->bucket[bucket_index], key);
}

void symbol_table_print(FILE* stream, const symbol_table_t* st) {
    for (int bucket_index = 0; bucket_index < TABLE_SIZE; bucket_index++) {
        if (st->bucket[bucket_index]->size > 0) {
            fprintf(stream, "[ %d ] : ", bucket_index);
            symbol_list_print(stream, st->bucket[bucket_index]);
            fprintf(stream, "\n");
        }    
    }
}

symbol_list_t* new_symbol_list() {
    symbol_list_t* sl = malloc(sizeof(symbol_list_t));
    sl->first = NULL;
    sl->size = 0;
    return sl;
}

void delete_symbol_list(symbol_list_t* sl) {
    symbol_list_node_t* node = sl->first;
    while (node != NULL) {
        symbol_list_node_t* node_to_delete = node;
        node = node->next;
        free(node_to_delete->symbol->value);
        free(node_to_delete->symbol);
        free(node_to_delete);
    }
    free(sl);
}

symbol_t* symbol_list_add(symbol_list_t* sl, const char* value, 
                          symbol_type_t type) {
    symbol_list_node_t* new_node = malloc(sizeof(symbol_list_node_t));
    new_node->symbol = malloc(sizeof(symbol_t));
    
    size_t value_length = strlen(value);
    new_node->symbol->value = calloc(value_length+1, sizeof(char));
    strcpy(new_node->symbol->value, value);
    new_node->symbol->value[value_length] = '\0';
    new_node->symbol->type = type;
    new_node->next = sl->first;

    sl->size++;
    sl->first = new_node;

    return new_node->symbol;
}

bool symbol_list_contains(symbol_list_t* sl, const char* key) {
    symbol_list_node_t* node = sl->first;
    while (node != NULL) {
        if (strcmp(node->symbol->value, key) == 0) {
            return true;
        }
        node = node->next;
    }
    return false;
}

symbol_t* symbol_list_get(symbol_list_t* sl, const char* key) {
    symbol_list_node_t* node = sl->first;
    while (node != NULL) {
        if (strcmp(node->symbol->value, key) == 0) {
            return node->symbol;
        }
        node = node->next;
    }
    return NULL;
}

void symbol_list_print(FILE* stream, symbol_list_t* sl) {
    symbol_list_node_t* node = sl->first;
    while (node != NULL) {
        fprintf(stream, "{%s, %d} ", node->symbol->value, node->symbol->type);

        node = node->next;
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

