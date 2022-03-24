#include "semantic.h"

int check_and_set_node_types(ast_node_t* node, symbol_table_t* st);

int check_variable_declaration(ast_node_t* declaration_node, symbol_data_type_t data_type);

int check_expression_type(ast_node_t* expression_node, symbol_data_type_t expected_data_type);

symbol_data_type_t evaluate_expression_type();

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
            check_variable_declaration(node, symbol_type_char);
            break;
        case ast_int_decl:
            check_variable_declaration(node, symbol_type_int);
            break;
        case ast_float_decl:
            check_variable_declaration(node, symbol_type_float);
            break;
        case ast_vector_decl:
            // Assign data type based on children
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

int check_variable_declaration(ast_node_t* declaration_node, symbol_data_type_t data_type) {
    int semantic_errors = 0;
    ast_node_list_t* children = ast_node_get_children(declaration_node);

    ast_node_t* identifier_node = ast_node_list_begin(children);
    symbol_t* identifier = ast_node_get_symbol(identifier_node);
    
    if(identifier->type == symbol_identifier) {
        identifier->type = symbol_variable;
        identifier->data_type = data_type;
    } else {
        fprintf(stderr, "Redeclared identifier '%s'\n", identifier->value);
        semantic_errors++;
    }

    ast_node_t* initial_value_node = ast_node_list_next(children);
    semantic_errors += check_expression_type(initial_value_node, data_type);

    return semantic_errors;
}

int check_expression_type(ast_node_t* expression_node, symbol_data_type_t expected_data_type) {
    int semantic_errors = 0;
    symbol_t* expression = ast_node_get_symbol(expression_node);
    symbol_data_type_t evaluated_data_type = evaluate_expression_type(expression_node);
    if(evaluated_data_type != expected_data_type) {
        fprintf(stderr, "Expression %s incompatible with type '%d'.\n", expression->value, expected_data_type);
        semantic_errors++;
    }
    return semantic_errors;
}

symbol_data_type_t evaluate_expression_type(ast_node_t* expression_node){
    //TODO
    return 0;
}