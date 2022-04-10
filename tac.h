#ifndef TAC_H
#define TAC_H

#include "syntax_tree.h"
#include "symbol_table.h"
#include "symbol.h"
#include "list.h"

typedef list_t tac_list_t;

typedef enum tac_type {
    tac_init,
    tac_vector_init,
    tac_vector_init_value,
    tac_vector_index,
    tac_symbol,
    tac_sum,
    tac_sub,
    tac_mul,
    tac_div,
    tac_eq,
    tac_dif,
    tac_gt,
    tac_ge,
    tac_lt,
    tac_le,
    tac_move,
    tac_vector_move,
    tac_begin_function,
    tac_end_function,
    tac_jump_false,
    tac_jump,
    tac_call,
    tac_argument,
    tac_parameter,
    tac_return,
    tac_print,
    tac_read,
    tac_label
} tac_type_t;

#define NOP NULL

typedef struct tac {
    tac_type_t type; 
    symbol_t* res;
    symbol_t* op1;
    symbol_t* op2;
} tac_t;

tac_t* new_tac(tac_type_t type, symbol_t* res, symbol_t* op1, symbol_t* op2);

void delete_tac(tac_t* tac);

char* tac_type_to_string(tac_type_t type);

#endif