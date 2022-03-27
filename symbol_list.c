#include "symbol_list.h"

#include <string.h>
#include <stdlib.h>
#include "data_type.h"

typedef struct symbol_list_node symbol_list_node_t;

struct symbol_list {
    symbol_list_node_t* first;
    size_t size;
};

struct symbol_list_node {
    symbol_t* symbol;
    struct symbol_list_node* next;
};

struct symbol_list_iterator {
    symbol_list_t* list;
    symbol_list_node_t* current;
};


symbol_list_t* new_symbol_list() {
    symbol_list_t* sl = malloc(sizeof(symbol_list_t));
    sl->first = SYMBOL_LIST_END;
    sl->size = 0;
    return sl;
}


void delete_symbol_list(symbol_list_t* sl) {
    symbol_list_node_t* node = sl->first;
    while (node != SYMBOL_LIST_END) {
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
    new_node->symbol->data_type = data_type_undefined;
    new_node->symbol->scope = SYMBOL_SCOPE_GLOBAL;
    new_node->next = sl->first;

    sl->size++;
    sl->first = new_node;

    return new_node->symbol;
}


bool symbol_list_contains(symbol_list_t* sl, const char* key, symbol_t* scope) {
    symbol_list_node_t* node = sl->first;
    while (node != SYMBOL_LIST_END) {
        if (strcmp(node->symbol->value, key) == 0 && node->symbol->scope == scope) {
            return true;
        }
        node = node->next;
    }
    return false;
}


symbol_t* symbol_list_search(symbol_list_t* sl, const char* key, symbol_t* scope) {
    symbol_list_node_t* node = sl->first;
    while (node != SYMBOL_LIST_END) {
        if (strcmp(node->symbol->value, key) == 0 && node->symbol->scope == scope) {
            return node->symbol;
        }
        node = node->next;
    }
    return NULL;
}


void symbol_list_print(FILE* stream, symbol_list_t* sl) {
    symbol_list_node_t* node = sl->first;
    while (node != SYMBOL_LIST_END) {
        fprintf(stream, "{%s, %d} ", node->symbol->value, node->symbol->type);

        node = node->next;
    }
}


size_t symbol_list_get_size(symbol_list_t* st) {
    return st->size;
}


symbol_list_iterator_t* new_symbol_list_iterator(symbol_list_t* sl) {
    symbol_list_iterator_t* it = malloc(sizeof(symbol_list_iterator_t));
    it->list = sl;
    it->current = sl->first;
    return it;
}


void delete_symbol_list_iterator(symbol_list_iterator_t* it) {
    free(it);
}


symbol_t* symbol_list_iterator_get(symbol_list_iterator_t* it) {
    return it->current->symbol;
}


void symbol_list_iterator_next(symbol_list_iterator_t* it) {
    if(it->current != SYMBOL_LIST_END) {
        it->current = it->current->next;
    }
}