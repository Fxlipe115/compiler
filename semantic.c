#include "semantic.h"

#include <stdbool.h>

int check_and_set_node_types(ast_node_t* node, symbol_table_t* st);

int check_variable_declaration(ast_node_t* declaration_node, data_type_t data_type);

int check_vector_declaration(ast_node_t* declaration_node);

int check_function_declaration(ast_node_t* declaration_node);

int check_expression_type(ast_node_t* expression_node, data_type_t expected_data_type);

int check_identifier(ast_node_t* identifier_node, symbol_type_t type, data_type_t data_type);

int check_identifier_definition(ast_node_t* definition_node, ast_node_type_t type);

data_type_t evaluate_expression_data_type(ast_node_t* expression_node);

data_type_t evaluate_vector_data_type(ast_node_t* vector_node);

data_type_t evaluate_function_data_type(ast_node_t* function_node);

data_type_t evaluate_arithmetic_expression(ast_node_t* expression_node);

data_type_t evaluate_boolean_expression(ast_node_t* expression_node);

data_type_t evaluate_identifier_definition_data_type(ast_node_t* node);

data_type_t evaluate_symbol_data_type(symbol_t* symbol);

data_type_t resulting_data_type(data_type_t data_type1, data_type_t data_type2);

bool identifier_already_declared(symbol_t* identifier);

bool are_compatible_data_types(data_type_t data_type1, data_type_t data_type2);

bool data_type_is_numeric(data_type_t data_type);


int check_semantic_errors(ast_t* ast, symbol_table_t* st) {
    return check_and_set_node_types(ast_get_root(ast), st);
}


int check_and_set_node_types(ast_node_t* node, symbol_table_t* st) {
    int semantic_errors = 0;

    if(node != NULL) {
        switch(ast_node_get_type(node)) {
        case ast_symbol:
                        
            break;
        case ast_char_decl: ;
            semantic_errors += check_variable_declaration(node, data_type_char);
            break;
        case ast_int_decl:
            semantic_errors += check_variable_declaration(node, data_type_int);
            break;
        case ast_float_decl:
            semantic_errors += check_variable_declaration(node, data_type_float);
            break;
        case ast_vector_decl:
            semantic_errors += check_vector_declaration(node);
            break;
        case ast_func_decl:
            semantic_errors += check_function_declaration(node);
            break;
        case ast_assign:
            break;
        case ast_if:
            break;
        case ast_if_else:
            break;
        default:
            break;
        }
        
        ast_node_list_t* children = ast_node_get_children(node);
        ast_node_t* child = ast_node_list_begin(children);
        while (child != NULL) {
            semantic_errors += check_and_set_node_types(child, st);
            child = ast_node_list_next(children);
        }
    }

    return semantic_errors;
}


int check_variable_declaration(ast_node_t* declaration_node, data_type_t data_type) {
    int semantic_errors = 0;
    ast_node_list_t* children = ast_node_get_children(declaration_node);
    ast_node_t* identifier_node = ast_node_list_begin(children);
    ast_node_t* initial_value_node = ast_node_list_next(children);
    
    semantic_errors += check_identifier(identifier_node, symbol_variable, data_type);
    semantic_errors += check_expression_type(initial_value_node, data_type);

    return semantic_errors;
}


int check_identifier(ast_node_t* identifier_node, symbol_type_t type, data_type_t data_type) {
    int semantic_errors = 0;
    symbol_t* identifier = ast_node_get_symbol(identifier_node);
    
    if(!identifier_already_declared(identifier)) {
        identifier->type = type;
        identifier->data_type = data_type;
    } else {
        fprintf(stderr, "Redeclared identifier '%s'\n", identifier->value);
        semantic_errors++;
    }
    return semantic_errors;
}

int check_vector_declaration(ast_node_t* declaration_node) {
    int semantic_errors = 0;
    ast_node_list_t* children = ast_node_get_children(declaration_node);
    ast_node_t* identifier_definition_node = ast_node_list_begin(children);
    
    semantic_errors += check_identifier_definition(identifier_definition_node, symbol_vector);

    return semantic_errors;
}


int check_identifier_definition(ast_node_t* definition_node, ast_node_type_t type) {
    int semantic_errors = 0;
    ast_node_list_t* children = ast_node_get_children(definition_node);
    ast_node_t* identifier_node = ast_node_list_begin(children);

    data_type_t data_type = evaluate_identifier_definition_data_type(definition_node);
    semantic_errors += check_identifier(identifier_node, symbol_vector, data_type);
    return semantic_errors;
}

int check_function_declaration(ast_node_t* declaration_node) {
    int semantic_errors = 0;
    return semantic_errors;
}


bool identifier_already_declared(symbol_t* identifier) {
    return identifier->type != symbol_identifier;
}


int check_expression_type(ast_node_t* expression_node, data_type_t expected_data_type) {
    int semantic_errors = 0;
    symbol_t* expression = ast_node_get_symbol(expression_node);
    data_type_t evaluated_data_type = evaluate_expression_data_type(expression_node);
    if(!are_compatible_data_types(evaluated_data_type, expected_data_type)) {
        fprintf(stderr, "Expression %s incompatible with type '%d'.\n", expression->value, expected_data_type);
        semantic_errors++;
    }
    return semantic_errors;
}


bool data_type_is_numeric(data_type_t data_type) {
    switch(data_type) {
        case data_type_int:
        case data_type_char:
        case data_type_float:
            return true;
            break;
        default:
            return false;
            break;
    }
}


data_type_t evaluate_expression_data_type(ast_node_t* expression_node){
    switch(ast_node_get_type(expression_node)) {
        case ast_symbol:
            return evaluate_symbol_data_type(ast_node_get_symbol(expression_node));
            break;

        case ast_vector_index:
            return evaluate_vector_data_type(expression_node);
            break;

        case ast_func_call:
            return evaluate_function_data_type(expression_node);
            break;

        case ast_read:
            return data_type_char;
            break;

        case ast_sum:
        case ast_sub:
        case ast_mul:
        case ast_div:
            return evaluate_arithmetic_expression(expression_node);
            break;

        case ast_eq:
        case ast_dif:
        case ast_lt:
        case ast_gt:
        case ast_le:
        case ast_ge:
            return evaluate_boolean_expression(expression_node);
            break;

        default:
            return data_type_undefined;
            break;
    }
}


data_type_t evaluate_symbol_data_type(symbol_t* symbol) {
    switch(symbol->type) {
        case symbol_variable:
        case symbol_vector:
        case symbol_function:
            return symbol->data_type;
            break;
        case symbol_char_literal:
            symbol->data_type = data_type_char;
            return symbol->data_type;
            break;
        case symbol_int_literal:
            symbol->data_type = data_type_int;
            return symbol->data_type;
            break;
        case symbol_string_literal:
            symbol->data_type = data_type_string;
            return symbol->data_type;
            break;
        default:
            return data_type_undefined;
            break;
    }
}


data_type_t resulting_data_type(data_type_t data_type1, data_type_t data_type2) {
    if(data_type1 == data_type_undefined || data_type2 == data_type_undefined) {
        return data_type_undefined;
    } else if(data_type1 == data_type_float || data_type2 == data_type_float) {
        return data_type_float;
    } else if(data_type1 == data_type_char && data_type2 == data_type_char) {
        return data_type_char;
    } else {
        return data_type_int;
    }
}


bool are_compatible_data_types(data_type_t data_type1, data_type_t data_type2) {
    if(data_type1 == data_type_undefined || data_type2 == data_type_undefined) {
        return false;
    } else if(data_type1 == data_type_string || data_type2 == data_type_string) {
        return false;
    } else if((data_type1 == data_type_bool || data_type2 == data_type_bool) && data_type1 != data_type2) {
        return false;
    } else {
        return true;
    }
}


data_type_t evaluate_arithmetic_expression(ast_node_t* expression_node) {
    ast_node_list_t* operands = ast_node_get_children(expression_node);
    ast_node_t* op1 = ast_node_list_begin(operands);
    ast_node_t* op2 = ast_node_list_next(operands);
    
    data_type_t op1_data_type = evaluate_expression_data_type(op1);
    data_type_t op2_data_type = evaluate_expression_data_type(op2);

    if(data_type_is_numeric(op1_data_type) && data_type_is_numeric(op2_data_type)) {
        return resulting_data_type(op1_data_type, op2_data_type);
    } else {
        return data_type_undefined;
    }
}


data_type_t evaluate_boolean_expression(ast_node_t* expression_node) {
    ast_node_list_t* operands = ast_node_get_children(expression_node);
    ast_node_t* op1 = ast_node_list_begin(operands);
    ast_node_t* op2 = ast_node_list_next(operands);
    
    data_type_t op1_data_type = evaluate_expression_data_type(op1);
    data_type_t op2_data_type = evaluate_expression_data_type(op2);

    if(data_type_is_numeric(op1_data_type) && data_type_is_numeric(op2_data_type)) {
        return data_type_bool;
    } else {
        return data_type_undefined;
    }
}


data_type_t evaluate_vector_data_type(ast_node_t* vector_node) {
    ast_node_list_t* children = ast_node_get_children(vector_node);
    ast_node_t* identifier_node = ast_node_list_begin(children);

    return evaluate_symbol_data_type(ast_node_get_symbol(identifier_node));
}


data_type_t evaluate_function_data_type(ast_node_t* function_node) {
    ast_node_list_t* children = ast_node_get_children(function_node);
    ast_node_t* identifier_node = ast_node_list_begin(children);

    return evaluate_symbol_data_type(ast_node_get_symbol(identifier_node));
}

data_type_t evaluate_identifier_definition_data_type(ast_node_t* node) {
    switch (ast_node_get_type(node)) {
        case ast_int_id_def:
            return data_type_int;
            break;
        case ast_char_id_def:
            return data_type_char;
            break;
        case ast_float_id_def:
            return data_type_float;
            break;
        default:
            return data_type_undefined;
            break;
    }
}