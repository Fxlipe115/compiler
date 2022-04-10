#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct list list_t;

typedef struct list_iterator list_iterator_t;

typedef void list_element_t;

list_t* new_list();

void delete_list(list_t* list, void (*element_destructor)(list_element_t*));

void list_push_front(list_t* list, list_element_t* element);

void list_push_back(list_t* list, list_element_t* element);

void list_pop_front(list_t* list);

void list_pop_back(list_t* list);

void list_merge(list_t* dest, list_t* other);

list_element_t* list_front(list_t* list);

list_element_t* list_back(list_t* list);

bool list_is_empty(list_t* list);

size_t list_size(list_t* list);

list_iterator_t* list_find(list_iterator_t* it, list_element_t* element, 
                           bool (*comp)(list_element_t*, list_element_t*));

list_iterator_t* list_begin(list_t* list);

list_iterator_t* list_end(list_t* list);

void delete_list_iterator(list_iterator_t* it);

list_iterator_t* list_next(list_iterator_t* it);

list_iterator_t* list_previous(list_iterator_t* it);

list_element_t* list_current(list_iterator_t* it);

void list_print(list_t* list, void (*printer)(list_element_t*));

#endif