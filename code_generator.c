#include "code_generator.h"

#include <string.h>
#include <stdio.h>
#include "tac.h"

typedef list_t code_list_t;
typedef list_iterator_t code_iterator_t;

tac_list_t* generate_node_code(ast_node_t* node, symbol_table_t* st);

tac_list_t* generate_variable_declaration(code_iterator_t* declaration_codes);

tac_list_t* generate_vector_declaration(code_iterator_t* declaration_codes);

tac_list_t* generate_vector_initialization(code_iterator_t* initialization_values);

tac_list_t* generate_move_code(code_iterator_t* move_codes);

tac_list_t* generate_move_to_vector_code(code_iterator_t* move_codes);

tac_list_t* generate_if_then_code(symbol_table_t* st, code_iterator_t* condition_and_command_codes);

tac_list_t* generate_if_then_else_code(symbol_table_t* st, code_iterator_t* condition_command_command_codes);

tac_list_t* generate_while_code(symbol_table_t* st, code_iterator_t* condition_and_command_codes);

tac_list_t* generate_binary_operation_code(symbol_table_t* st, code_iterator_t* expressions_codes, ast_node_type_t node_type);

tac_list_t* generate_read_code(symbol_table_t* st);

tac_list_t* generate_vector_indexing_code(symbol_table_t* st, code_iterator_t* vector_codes);

tac_list_t* generate_goto_code(code_iterator_t* goto_codes);

tac_list_t* generate_return_code(code_iterator_t* return_codes);

tac_list_t* generate_label_code(code_iterator_t* label_codes);

tac_list_t* generate_function_argument_code(code_iterator_t* argument_codes);

tac_list_t* generate_function_call_code(symbol_table_t* st, code_iterator_t* declaration_codes);

tac_list_t* generate_print_argument_code(code_iterator_t* printable_codes);

tac_list_t* generate_function_parameter_code(code_iterator_t* parameter_codes);

tac_list_t* generate_function_definition_code(code_iterator_t* definition_codes);

tac_type_t node_type_to_tac_type(ast_node_type_t node_type);

symbol_t* make_temp(symbol_table_t* st);

symbol_t* make_label(symbol_table_t* st);


tac_list_t* generate_code(ast_t* ast, symbol_table_t* st) {
    return generate_node_code(ast_get_root(ast), st);
}


tac_list_t* generate_node_code(ast_node_t* node, symbol_table_t* st) {
    if(node == NULL) {
        return new_list();
    }
    code_list_t* children_codes = new_list();

    ast_list_t* children = ast_node_get_children(node);
    for(list_iterator_t* it = list_begin(children); list_current(it) != NULL; list_next(it)) {
        list_push_back(children_codes, generate_node_code(list_current(it), st));
    }

    code_iterator_t* children_codes_it = list_begin(children_codes);

    tac_list_t* node_code = new_list();

    switch(ast_node_get_type(node)) {
        case ast_symbol:
            list_push_back(node_code, new_tac(tac_symbol, ast_node_get_symbol(node), NULL, NULL));
            break;
        case ast_int_decl:
        case ast_char_decl:
        case ast_float_decl:
            list_merge(node_code, generate_variable_declaration(children_codes_it));
            break;
        case ast_vector_decl:
        case ast_vector_init_decl:
            list_merge(node_code, generate_vector_declaration(children_codes_it));
            break;
        case ast_assign:
            list_merge(node_code, generate_move_code(children_codes_it));
            break;
        case ast_vector_assign:
            list_merge(node_code, generate_move_to_vector_code(children_codes_it));
            break;
        case ast_sum:
        case ast_sub:
        case ast_mul:
        case ast_div:
        case ast_lt:
        case ast_gt:
        case ast_le:
        case ast_ge:
        case ast_eq:
        case ast_dif:
            list_merge(node_code, generate_binary_operation_code(st, children_codes_it, ast_node_get_type(node)));
            break;
        case ast_if:;
            list_merge(node_code, generate_if_then_code(st, children_codes_it));
            break;
        case ast_if_else:
            list_merge(node_code, generate_if_then_else_code(st, children_codes_it));
            break;
        case ast_while:
            list_merge(node_code, generate_while_code(st, children_codes_it));
            break;
        case ast_read:
            list_merge(node_code, generate_read_code(st));
            break;
        case ast_vector_index:
            list_merge(node_code, generate_vector_indexing_code(st, children_codes_it));
            break;
        case ast_goto:
            list_merge(node_code, generate_goto_code(children_codes_it));
            break;
        case ast_return:
            list_merge(node_code, generate_return_code(children_codes_it));
            break;
        case ast_label:
            list_merge(node_code, generate_label_code(children_codes_it));
            break;
        case ast_func_arg:
            list_merge(node_code, generate_function_argument_code(children_codes_it));
            break;
        case ast_func_call:
            list_merge(node_code, generate_function_call_code(st, children_codes_it));
            break;
        case ast_print_arg:
            list_merge(node_code, generate_print_argument_code(children_codes_it));
            break;
        case ast_func_param:
            if(list_size(children) > 0) {
                list_merge(node_code, generate_function_parameter_code(children_codes_it));
            }
            break;
        case ast_func_decl:
            list_merge(node_code, generate_function_definition_code(children_codes_it));
            break;
        default:
            for(list_iterator_t* it = list_begin(children_codes); list_current(it) != NULL; list_next(it)) {
                list_merge(node_code, list_current(it));
            }
            break;
    }
    return node_code;
}


symbol_t* make_temp(symbol_table_t* st) {
    static int serial = 0;
    char name[256] = "";

    sprintf(name, "%d_temp_%d", serial, serial);
    serial++;

    return symbol_table_add(st, name, symbol_variable, 0);
}


symbol_t* make_label(symbol_table_t* st) {
    static int serial = 0;
    char name[256] = "";

    sprintf(name, "%d_label_%d", serial, serial);
    serial++;

    return symbol_table_add(st, name, symbol_label, 0);
}


tac_list_t* generate_if_then_code(symbol_table_t* st, code_iterator_t* condition_and_command_codes) {
    tac_list_t* condition_code = list_current(condition_and_command_codes);
    tac_list_t* command_code = list_current(list_next(condition_and_command_codes));

    symbol_t* label_symbol = make_label(st);
    tac_t* condition_result = list_back(condition_code);
    tac_t* jump = new_tac(tac_jump_false, label_symbol, condition_result->res, NOP);
    tac_t* label = new_tac(tac_label, label_symbol, NOP, NOP);

    list_t* if_then_code = new_list();
    list_merge(if_then_code, condition_code);
    list_push_back(if_then_code, jump);
    list_merge(if_then_code, command_code);
    list_push_back(if_then_code, label);
    
    return if_then_code;
}


tac_list_t* generate_if_then_else_code(symbol_table_t* st, code_iterator_t* condition_command_command_codes) {
    tac_list_t* condition_code = list_current(condition_command_command_codes);
    tac_list_t* then_code = list_current(list_next(condition_command_command_codes));
    tac_list_t* else_code = list_current(list_next(condition_command_command_codes));

    symbol_t* else_symbol = make_label(st);
    symbol_t* continue_symbol = make_label(st);
    tac_t* condition_result = list_back(condition_code);
    tac_t* jump_to_else = new_tac(tac_jump_false, else_symbol, condition_result->res, NOP);
    tac_t* skip_else = new_tac(tac_jump, continue_symbol, NOP, NOP);
    tac_t* else_label = new_tac(tac_label, else_symbol, NOP, NOP);
    tac_t* continue_label = new_tac(tac_label, continue_symbol, NOP, NOP);

    tac_list_t* if_then_else_code = new_list();
    list_merge(if_then_else_code, condition_code);
    list_push_back(if_then_else_code, jump_to_else);
    list_merge(if_then_else_code, then_code);
    list_push_back(if_then_else_code, skip_else);
    list_push_back(if_then_else_code, else_label);
    list_merge(if_then_else_code, else_code);
    list_push_back(if_then_else_code, continue_label);

    return if_then_else_code;
}


tac_list_t* generate_while_code(symbol_table_t* st, code_iterator_t* condition_and_command_codes) {
    tac_list_t* condition_code = list_current(condition_and_command_codes);
    tac_list_t* command_code = list_current(list_next(condition_and_command_codes));

    symbol_t* while_begin_label = make_label(st);
    symbol_t* while_end_label = make_label(st);

    tac_t* while_begin = new_tac(tac_label, while_begin_label, NOP, NOP);
    tac_t* condition_result = list_back(condition_code);
    tac_t* conditional_jump_to_end = new_tac(tac_jump_false, while_end_label, condition_result->res, NOP);
    tac_t* jump_to_begin = new_tac(tac_jump, while_begin_label, NULL, NULL);
    tac_t* while_end = new_tac(tac_label, while_end_label, NOP, NOP);

    list_t* while_code = new_list();
    list_push_back(while_code, while_begin);
    list_merge(while_code, condition_code);
    list_push_back(while_code, conditional_jump_to_end);
    list_merge(while_code, command_code);
    list_push_back(while_code, jump_to_begin);
    list_push_back(while_code, while_end);
    
    return while_code;
}


tac_list_t* generate_binary_operation_code(symbol_table_t* st, code_iterator_t* expressions_codes, ast_node_type_t node_type) {
    tac_list_t* left_expression_code = list_current(expressions_codes);
    tac_list_t* right_expression_code = list_current(list_next(expressions_codes));

    tac_t* left_expression_result = list_back(left_expression_code);
    tac_t* right_expression_result = list_back(right_expression_code);
    tac_t* operation = new_tac(node_type_to_tac_type(node_type), make_temp(st), 
                               left_expression_result->res, right_expression_result->res);


    list_t* binary_operation_code = new_list();
    if(left_expression_result->type != tac_symbol) {
        list_merge(binary_operation_code, left_expression_code);
    }
    if(right_expression_result->type != tac_symbol) {
        list_merge(binary_operation_code, right_expression_code);
    }
    list_push_back(binary_operation_code, operation);

    return binary_operation_code;
}


void tac_printer(list_element_t* element) {
    tac_t* tac = element;
    fprintf(stderr, "TAC(%s, %s, %s, %s)\n",
            tac_type_to_string(tac->type),
            tac->res->value,
            tac->op1 != NULL ? tac->op1->value : "NOP",
            tac->op2 != NULL ? tac->op2->value : "NOP");
}


void print_code(list_t* code) {
    list_print(code, &tac_printer); 
}


tac_type_t node_type_to_tac_type(ast_node_type_t node_type) {
    switch(node_type) {
        case ast_sum:
            return tac_sum;
        case ast_sub:
            return tac_sub;
        case ast_mul:
            return tac_mul;
        case ast_div:
            return tac_div;
        case ast_lt:
            return tac_lt;
        case ast_gt:
            return tac_gt;
        case ast_le:
            return tac_le;
        case ast_ge:
            return tac_ge;
        case ast_eq:
            return tac_eq;
        case ast_dif:
            return tac_dif;
        default:
            return -1;
    }
}


tac_list_t* generate_variable_declaration(code_iterator_t* declaration_codes) {
    tac_list_t* identifier_code = list_current(declaration_codes);
    tac_list_t* value_code = list_current(list_next(declaration_codes));
    tac_list_t* divisor_code = list_current(list_next(declaration_codes));

    tac_t* identifier = list_front(identifier_code);
    tac_t* value = list_front(value_code);
    // will always be NULL unless it's a float declaration
    tac_t* divisor = list_front(divisor_code);

    tac_list_t* declaration_code = new_list();
    tac_t* declaration = new_tac(tac_init, identifier->res, value->res, 
                                 divisor == NULL ? NOP : divisor->res);
    list_push_back(declaration_code, declaration);

    return declaration_code;
}


tac_list_t* generate_vector_declaration(code_iterator_t* declaration_codes) {
    tac_list_t* identifier_code = list_current(declaration_codes);
    tac_list_t* size_code = list_current(list_next(declaration_codes));
    tac_list_t* initialization_code = list_current(list_next(declaration_codes));

    tac_t* identifier = list_front(identifier_code);
    tac_t* size = list_front(size_code);
    tac_t* vector = new_tac(tac_vector_init, identifier->res, size->res, NULL);

    tac_list_t* declaration = new_list();
    list_push_back(declaration, vector);
    if(initialization_code != NULL) {
        for(list_iterator_t* it = list_begin(initialization_code); list_current(it) != NULL; list_next(it)) {
            tac_t* value = list_current(it);
            list_push_back(declaration, new_tac(tac_vector_init_value, vector->res, value->res, NULL));
        }
    }

    return declaration;
}


tac_list_t* generate_move_code(code_iterator_t* move_codes) {
    tac_list_t* result_code = list_current(move_codes);
    tac_list_t* value_code = list_current(list_next(move_codes));

    tac_t* result = list_back(result_code);
    tac_t* value = list_back(value_code);
    tac_t* move = new_tac(tac_move, result->res, value->res, NULL);

    tac_list_t* move_code = new_list();

    if(value->type != tac_symbol) {
        list_merge(move_code, value_code);
    }
    list_push_back(move_code, move);

    return move_code;
}


tac_list_t* generate_move_to_vector_code(code_iterator_t* move_codes) {
    tac_list_t* result_code = list_current(move_codes);
    tac_list_t* index_code = list_current(list_next(move_codes));
    tac_list_t* value_code = list_current(list_next(move_codes));

    tac_t* result = list_back(result_code);
    tac_t* index = list_back(index_code);
    tac_t* value = list_back(value_code);
    tac_t* move = new_tac(tac_vector_move, result->res, index->res, value->res);

    tac_list_t* move_code = new_list();

    if(value->type != tac_symbol) {
        list_merge(move_code, value_code);
    }
    if(index->type != tac_symbol) {
        list_merge(move_code, index_code);
    }
    list_push_back(move_code, move);

    return move_code;
}


tac_list_t* generate_read_code(symbol_table_t* st) {
    symbol_t* read_result = make_temp(st);
    tac_t* read = new_tac(tac_read, read_result, NULL, NULL);
    
    tac_list_t* read_code = new_list();
    list_push_back(read_code, read);

    return read_code;
}


tac_list_t* generate_vector_indexing_code(symbol_table_t* st, code_iterator_t* vector_codes) {
    tac_list_t* identifier_code = list_current(vector_codes);
    tac_list_t* index_code = list_current(list_next(vector_codes));

    tac_t* identifier = list_back(identifier_code);
    tac_t* index = list_back(index_code);

    symbol_t* temp = make_temp(st);

    tac_t* indexed_vector = new_tac(tac_vector_index, temp, identifier->res, index->res);

    tac_list_t* indexing_code = new_list();

    if(index->type != tac_symbol) {
        list_merge(indexing_code, index_code);
    }
    list_push_back(indexing_code, indexed_vector);

    return indexing_code;
}


tac_list_t* generate_goto_code(code_iterator_t* goto_codes) {
    tac_t* label = list_back(list_current(goto_codes));

    tac_list_t* goto_code = new_list();
    list_push_back(goto_code, new_tac(tac_jump, label->res, NULL, NULL));

    return goto_code;
}


tac_list_t* generate_return_code(code_iterator_t* return_codes) {
    tac_list_t* return_value_code = list_current(return_codes);
    
    tac_t* return_value = list_back(return_value_code);
    tac_t* return_tac = new_tac(tac_return, return_value->res, NULL, NULL);

    tac_list_t* return_code = new_list();
    if(return_value->type != tac_symbol) {
        list_merge(return_code, return_value_code);
    }
    list_push_back(return_code, return_tac);

    return return_code;
}


tac_list_t* generate_label_code(code_iterator_t* label_codes) {
    tac_list_t* identifier_code = list_current(label_codes);
    tac_list_t* following_code = list_current(list_next(label_codes));
    
    tac_t* label = list_back(identifier_code);
    tac_t* return_tac = new_tac(tac_label, label->res, NULL, NULL);

    tac_list_t* label_code = new_list();
    list_push_back(label_code, return_tac);
    list_merge(label_code, following_code);
    

    return label_code;
}


tac_list_t* generate_function_argument_code(code_iterator_t* argument_codes) {
    tac_list_t* argument_code = list_current(argument_codes);
    tac_list_t* next_arguments_code = list_current(list_next(argument_codes));

    tac_t* argument_value = list_back(argument_code);
    tac_t* argument = new_tac(tac_argument, argument_value->res, NOP, NOP);

    tac_list_t* arguments = new_list();
    if(argument_value->type != tac_symbol) {
        list_merge(arguments, argument_code);
    }
    if(next_arguments_code != NULL) {
        list_merge(arguments, next_arguments_code);
    }
    list_push_back(arguments, argument);
    
    return arguments;
}


tac_list_t* generate_function_call_code(symbol_table_t* st, code_iterator_t* declaration_codes) {
    tac_list_t* identifier_code = list_current(declaration_codes);
    tac_list_t* arguments_code = list_current(list_next(declaration_codes));

    tac_t* identifier = list_back(identifier_code);
    tac_t* call = new_tac(tac_call, make_temp(st), identifier->res, NULL);

    tac_list_t* function_call = new_list();
    if(arguments_code != NULL) {
        list_merge(function_call, arguments_code);
    }
    list_push_back(function_call, call);

    return function_call;
}


tac_list_t* generate_print_argument_code(code_iterator_t* printable_codes) {
    tac_list_t* printable_code = list_current(printable_codes);
    tac_list_t* rest = list_current(list_next(printable_codes));

    tac_t* print_value = list_back(printable_code);
    tac_t* print_tac = new_tac(tac_print, print_value->res, NOP, NOP);
    
    tac_list_t* print_code = new_list();
    if(print_value->type != tac_symbol) {
        list_merge(print_code, printable_code);
    }
    list_push_back(print_code, print_tac);
    if(rest != NULL) {
        list_merge(print_code, rest);
    }

    return print_code;
}


tac_list_t* generate_function_parameter_code(code_iterator_t* parameter_codes) {
    tac_list_t* identifier_code = list_current(parameter_codes);
    tac_list_t* next_parameters_code = list_current(list_next(parameter_codes));

    tac_t* identifier_value = list_back(identifier_code);
    tac_t* parameter = new_tac(tac_parameter, identifier_value->res, NOP, NOP);

    tac_list_t* parameters = new_list();
    list_push_back(parameters, parameter);
    if(next_parameters_code != NULL) {
        list_merge(parameters, next_parameters_code);
    }
    
    return parameters;
}


tac_list_t* generate_function_definition_code(code_iterator_t* definition_codes) {
    tac_list_t* identifier_code = list_current(definition_codes);
    tac_list_t* parameters_code = list_current(list_next(definition_codes));
    tac_list_t* function_code = list_current(list_next(definition_codes));

    tac_t* identifier_value = list_back(identifier_code);
    tac_t* function_begin = new_tac(tac_begin_function, identifier_value->res, NOP, NOP);
    tac_t* function_end = new_tac(tac_end_function, identifier_value->res, NOP, NOP);

    tac_list_t* function_definition = new_list();
    list_push_back(function_definition, function_begin);
    list_merge(function_definition, parameters_code);
    list_merge(function_definition, function_code);
    list_push_back(function_definition, function_end);
    
    return function_definition;
}