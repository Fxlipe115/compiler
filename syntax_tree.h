#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include "symbol_table.h"
#include "list.h"

typedef struct ast ast_t;

typedef struct ast_node ast_node_t;

typedef list_t ast_list_t;

typedef enum ast_node_type {
    ast_symbol,
    ast_sum,
    ast_sub,
    ast_mul,
    ast_div,
    ast_lt,
    ast_gt,
    ast_le,
    ast_ge,
    ast_eq,
    ast_dif,
    ast_vector_index,
    ast_vector_init_value,
    ast_func_call,
    ast_func_param,
    ast_func_arg,
    ast_read,
    ast_program,
    ast_decl,
    ast_int_decl,
    ast_char_decl,
    ast_float_decl,
    ast_vector_decl,
    ast_vector_init_decl,
    ast_func_decl,
    ast_int_id_def,
    ast_char_id_def,
    ast_float_id_def,
    ast_cmd_block,
    ast_assign,
    ast_print_type,
    ast_return,
    ast_cmd,
    ast_label,
    ast_vector_assign,
    ast_print_arg,
    ast_if,
    ast_if_else,
    ast_while,
    ast_goto
} ast_node_type_t;

ast_t* new_ast();

void ast_set_root(ast_t* ast, ast_node_t* root);

ast_node_t* ast_get_root(ast_t* ast);

void delete_ast(ast_t* ast);

ast_node_t* new_ast_node(ast_node_type_t type, size_t children_quantity,...);

ast_node_t* new_ast_symbol_node(symbol_t* symbol);

symbol_t* ast_node_get_symbol(ast_node_t* node);

void ast_node_set_symbol(ast_node_t* node, symbol_t* symbol);

symbol_t* ast_node_get_scope(ast_node_t* node);

ast_node_type_t ast_node_get_type(ast_node_t* node);

ast_list_t* ast_node_get_children(ast_node_t* node);

data_type_t ast_node_get_evaluated_data_type(ast_node_t* node);

void ast_node_set_evaluated_data_type(ast_node_t* node, data_type_t evaluated_data_type);

void delete_ast_node(ast_node_t* node);

void ast_print(FILE* stream, ast_t* ast);

void decompile(FILE* stream, ast_t* ast);

#endif
