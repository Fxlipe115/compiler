#include "list.h"

#include <stdlib.h>


typedef struct list_node list_node_t;

struct list
{
    size_t size;
    list_node_t* first;
    list_node_t* last;
};

struct list_node
{
    list_element_t* element;
    list_node_t* previous;
    list_node_t* next;
};


struct list_iterator {
    list_t* list;
    list_node_t* current;
};


list_t* new_list() {
    list_t* new_list = malloc(sizeof(list_t));
    new_list->first = NULL;
    new_list->last = NULL;
    new_list->size = 0;
    return new_list;
}


void delete_list(list_t* list, void (*element_destructor)(list_element_t*)) {
    if(list == NULL) {
        return;
    }
    while(!list_is_empty(list)) {
        list_element_t* element = list_front(list);
        if(element_destructor != NULL) {
            element_destructor(element);
        }
        list_pop_front(list);
    }
    free(list);
}


void list_push_front(list_t* list, list_element_t* element) {
    list_node_t* new_node = malloc(sizeof(list_node_t));
    new_node->element = element;
    new_node->next = list->first;
    if(list->first != NULL) {
        list->first->previous = new_node;
    }
    new_node->previous = NULL;
    list->first = new_node;
    if(list->last == NULL) {
        list->last = new_node;
    }
    list->size++;
}


void list_push_back(list_t* list, list_element_t* element) {
    list_node_t* new_node = malloc(sizeof(list_node_t));
    new_node->element = element;
    new_node->next = NULL;
    if(list->last != NULL) {
        list->last->next = new_node;
    }
    new_node->previous = list->last;
    list->last = new_node;
    if(list->first == NULL) {
        list->first = new_node;
    }
    list->size++;
}


void list_pop_front(list_t* list) {
    list_node_t* node_to_pop = list->first;
    if(list_size(list) <= 1) {
        list->first=NULL;
    } else {
        list->first = list->first->next;
        list->first->previous = NULL;
    }
    list->size -= list->size < 1 ? 0 : 1;
    free(node_to_pop);
}


void list_pop_back(list_t* list) {
    list_node_t* node_to_pop = list->last;
    if(list_size(list) <= 1) {
        list->last=NULL;
    } else {
        list->last = list->last->previous;
        list->last->next = NULL;
    }
    list->size -= list->size < 1 ? 0 : 1;
    free(node_to_pop);
}


list_element_t* list_front(list_t* list) {
    return list->first->element;
}

list_element_t* list_back(list_t* list) {
    return list->last->element;
}


bool list_is_empty(list_t* list) {
    return list->size == 0;
}


size_t list_size(list_t* list) {
    return list->size;
}


list_iterator_t* list_find(list_iterator_t* it, list_element_t* element, 
                           bool (*comp)(list_element_t*, list_element_t*)) {
    while(list_current(it) != NULL && !comp(list_current(it), element)) {
        list_next(it);
    }
    return it;
}


list_iterator_t* list_begin(list_t* list) {
    list_iterator_t* it = malloc(sizeof(list_iterator_t));
    it->list = list;
    it->current = list->first;
    return it;
}


list_iterator_t* list_end(list_t* list) {
    list_iterator_t* it = malloc(sizeof(list_iterator_t));
    it->list = list;
    it->current = list->last;
    return it;
}


void delete_list_iterator(list_iterator_t* it) {
    free(it);
}


list_iterator_t* list_next(list_iterator_t* it) {
    if(it->current != NULL) {
        it->current = it->current->next;
    }
    return it;
}


list_iterator_t* list_previous(list_iterator_t* it) {
    if(it->current != NULL) {
        it->current = it->current->previous;
    }
    return it;
}


list_element_t* list_current(list_iterator_t* it) {
    return it->current != NULL ? it->current->element : NULL;
}


void list_print(list_t* list, void (*printer)(list_element_t*)) {
    if(printer != NULL) {
        for(list_iterator_t* it = list_begin(list);  list_current(it) != NULL; list_next(it)) {
            printer(list_current(it));
        }
    }
}