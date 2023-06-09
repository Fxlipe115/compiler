#include "tac.h"

#include <stdlib.h>

tac_t* new_tac(tac_type_t type, symbol_t* res, symbol_t* op1, symbol_t* op2) {
    tac_t* tac = malloc(sizeof(tac_t));
    tac->type = type;
    tac->res = res;
    tac->op1 = op1;
    tac->op2 = op2;
    return tac;
}


void delete_tac(tac_t* tac) {
    free(tac);
}


char* tac_type_to_string(tac_type_t type) {
    switch(type) {
    case tac_symbol:
        return "tac_symbol";
    case tac_sum:
        return "tac_sum";
    case tac_sub:
        return "tac_sub";
    case tac_mul:
        return "tac_mul";
    case tac_div:
        return "tac_div";
    case tac_eq:
        return "tac_eq";
    case tac_dif:
        return "tac_dif";
    case tac_gt:
        return "tac_gt";
    case tac_ge:
        return "tac_ge";
    case tac_lt:
        return "tac_lt";
    case tac_le:
        return "tac_le";
    case tac_move:
        return "tac_move";
    case tac_vector_move:
        return "tac_vector_move";
    case tac_vector_index:
        return "tac_vector_index";
    case tac_begin_function:
        return "tac_begin_function";
    case tac_end_function:
        return "tac_end_function";
    case tac_jump_false:
        return "tac_jump_false";
    case tac_jump:
        return "tac_jump";
    case tac_call:
        return "tac_call";
    case tac_argument:
        return "tac_argument";
    case tac_parameter:
        return "tac_parameter";
    case tac_return:
        return "tac_return";
    case tac_print:
        return "tac_print";
    case tac_read:
        return "tac_read";
    case tac_label:
        return "tac_label";
    case tac_init:
        return "tac_init";
    case tac_vector_uninit:
        return "tac_vector_uninit";
    case tac_vector_init:
        return "tac_vector_init";
    case tac_vector_init_value:
        return "tac_vector_init_value";
    case tac_temp:
        return "tac_temp";
    case tac_literal:
        return "tac_literal";
    default:
        return "unknown type";
    }
}