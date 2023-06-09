#include "semantic.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "list.h"


void set_literal_type(ast_node_t* node);

int check_definitions(ast_node_t* node, symbol_table_t* st);

int check_implementations(ast_node_t* node, symbol_table_t* st);

int check_variable_declaration(ast_node_t* declaration_node, symbol_table_t* st, data_type_t data_type);

int check_vector_declaration(ast_node_t* declaration_node, symbol_table_t* st);

int check_initialization_list_size(ast_node_t* list_node, ast_node_t* size_node);

int check_function_declaration(ast_node_t* declaration_node, symbol_table_t* st);

int check_function_implementation(ast_node_t* declaration_node, symbol_table_t* st);

int check_function_call(ast_node_t* function_node, symbol_table_t* st, symbol_t* scope);

int check_parameter_list(ast_node_t* parameter_list_node, symbol_table_t* st, symbol_t* function);

int check_expression_type(ast_node_t* expression_node, data_type_t expected_data_type, symbol_table_t* st, symbol_t* scope);

int check_identifiers_in_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope);

int check_identifier(ast_node_t* identifier_node, symbol_table_t* st, symbol_type_t type, data_type_t data_type, symbol_t* scope);

int check_identifier_definition(ast_node_t* definition_node, symbol_table_t* st, symbol_type_t type, symbol_t* scope);

int check_command(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope);

int check_assignment(ast_node_t* assignment_node, symbol_table_t* st, symbol_t* scope);

int check_vector_assignment(ast_node_t* assignment_node, symbol_table_t* st, symbol_t* scope);

int check_vector_index_type(ast_node_t* index_node, symbol_table_t* st, symbol_t* scope);

int check_command_block(ast_node_t* command_block_node, symbol_table_t* st, symbol_t* scope);

int check_command_list(ast_node_t* command_list_node, symbol_table_t* st, symbol_t* scope);

int check_if_else(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope);

int check_print(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope);

int check_return(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope);

int check_goto(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_expression_data_type(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_vector_data_type(ast_node_t* vector_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_function_data_type(ast_node_t* function_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_arithmetic_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_boolean_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope);

data_type_t evaluate_identifier_definition_data_type(ast_node_t* node);

data_type_t evaluate_symbol_data_type(symbol_t* symbol, symbol_table_t* st, symbol_t* scope);

data_type_t resulting_data_type(data_type_t data_type1, data_type_t data_type2);

symbol_t* get_identifier_definition_symbol(ast_node_t* identifier_definition_node);

bool identifier_already_declared(symbol_table_t* st, symbol_t* identifier, symbol_t* scope);

bool identifier_declared_in_any_other_scope(symbol_table_t* st, symbol_t* identifier, symbol_t* except_scope);

bool are_compatible_data_types(data_type_t data_type1, data_type_t data_type2);

bool data_type_is_numeric(data_type_t data_type);

bool is_valid_expression_node_type(ast_node_t* expression_node);

bool is_identifier_valid_in_scope(symbol_table_t* st, symbol_t* identifier, symbol_t* scope);

const char* symbol_type_to_string(symbol_type_t type);

const char* data_type_to_string(data_type_t type);


int check_semantic_errors(ast_t* ast, symbol_table_t* st) {
    int semantic_errors = 0;
    semantic_errors = check_definitions(ast_get_root(ast), st);
    semantic_errors = check_implementations(ast_get_root(ast), st);
    return semantic_errors;
}


int check_implementations(ast_node_t* node, symbol_table_t* st) {
    int semantic_errors = 0;

    if(node != NULL) {
        switch(ast_node_get_type(node)) {
        case ast_func_decl:
            semantic_errors += check_function_implementation(node, st);
            break;
        default:
            break;
        }
        
        list_iterator_t it = list_begin(ast_node_get_children(node));
        for(; list_current(it) != NULL; list_next(&it)){
            semantic_errors += check_implementations(list_current(it), st);
        }        
    }

    return semantic_errors;
}


int check_definitions(ast_node_t* node, symbol_table_t* st) {
    int semantic_errors = 0;

    if(node != NULL) {
        switch(ast_node_get_type(node)) {
        case ast_symbol:
            set_literal_type(node);
            break;
        case ast_char_decl: ;
            semantic_errors += check_variable_declaration(node, st, data_type_char);
            break;
        case ast_int_decl:
            semantic_errors += check_variable_declaration(node, st, data_type_int);
            break;
        case ast_float_decl:
            semantic_errors += check_variable_declaration(node, st, data_type_float);
            break;
        case ast_vector_decl:
        case ast_vector_init_decl:
            semantic_errors += check_vector_declaration(node, st);
            break;
        case ast_func_decl:
            semantic_errors += check_function_declaration(node, st);
            break;
        default:
            break;
        }
        
        list_iterator_t it = list_begin(ast_node_get_children(node));
        for(; list_current(it) != NULL; list_next(&it)){
            semantic_errors += check_definitions(list_current(it), st);
        }        
    }

    return semantic_errors;
}


void set_literal_type(ast_node_t* node) {
    symbol_t* symbol = ast_node_get_symbol(node);
    switch(symbol->type) {
        case symbol_int_literal:
            symbol->data_type = data_type_int;
            break;
        case symbol_char_literal:
            symbol->data_type = data_type_char;
            break;
        case symbol_string_literal:
            symbol->data_type = data_type_string;
            break;
        default:
            break;
    }
}


int check_variable_declaration(ast_node_t* declaration_node, symbol_table_t* st, data_type_t data_type) {
    assert(declaration_node != NULL);
    assert(ast_node_get_type(declaration_node) == ast_int_decl ||
           ast_node_get_type(declaration_node) == ast_char_decl ||
           ast_node_get_type(declaration_node) == ast_float_decl);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(declaration_node);
    ast_node_t* identifier_node = list_current(list_begin(children));
    
    semantic_errors += check_identifier(identifier_node, st, symbol_variable, data_type, SYMBOL_SCOPE_GLOBAL);

    return semantic_errors;
}


int check_identifier(ast_node_t* identifier_node, symbol_table_t* st, symbol_type_t type, data_type_t data_type, symbol_t* scope) {
    assert(identifier_node != NULL);
    assert(ast_node_get_type(identifier_node) == ast_symbol);

    int semantic_errors = 0;
    symbol_t* identifier = ast_node_get_symbol(identifier_node);
    


    if(!identifier_already_declared(st, identifier, scope)) {
        // Case already in symbol table
        /*if(scope == SYMBOL_SCOPE_GLOBAL) {
            identifier->type = type;
            identifier->data_type = data_type;
            identifier->scope = scope;
        // Case in scope of function, but already declared in any other scope
        } else*/ if(identifier_declared_in_any_other_scope(st, identifier, scope)){
            symbol_t* new_identifier = symbol_table_add_with_scope(st, identifier->value, type, identifier->first_define_at_line, scope);
            new_identifier->data_type = data_type;
            new_identifier->scope = scope;
            if(scope != SYMBOL_SCOPE_GLOBAL){
                new_identifier->first_define_at_line = scope->first_define_at_line;
                list_push_back(scope->parameters, new_identifier);
            }
            ast_node_set_symbol(identifier_node, new_identifier);
        // Case in scope of function and not declared as global or in any other function
        } else {
            identifier->type = type;
            identifier->data_type = data_type;
            identifier->scope = scope;
            if(scope != SYMBOL_SCOPE_GLOBAL){
                identifier->first_define_at_line = scope->first_define_at_line;
                list_push_back(scope->parameters, identifier);
            }
        }
    } else {
        fprintf(stderr, "%s: redeclared identifier '%s'. First declared on line %d as %s of type %s.\n",
            scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : 
                scope->value,
            identifier->value,
            identifier->first_define_at_line,
            symbol_type_to_string(identifier->type),
            data_type_to_string(identifier->data_type));
        semantic_errors++;
    }
    return semantic_errors;
}

int check_vector_declaration(ast_node_t* declaration_node, symbol_table_t* st) {
    assert(declaration_node != NULL);
    assert(ast_node_get_type(declaration_node) == ast_vector_decl ||
           ast_node_get_type(declaration_node) == ast_vector_init_decl);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(declaration_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_definition_node = list_current(it);
    ast_node_t* vector_size_node = list_current(list_next(&it));
    ast_node_t* initialization_list_node = list_current(list_next(&it));
    
    semantic_errors += check_identifier_definition(identifier_definition_node, st, symbol_vector, SYMBOL_SCOPE_GLOBAL);

    if(initialization_list_node != NULL) {
        semantic_errors += check_initialization_list_size(initialization_list_node, vector_size_node);
    }

    return semantic_errors;
}


int check_initialization_list_size(ast_node_t* list_node, ast_node_t* size_node) {
    int semantic_errors = 0;

    int declared_size = atoi(ast_node_get_symbol(size_node)->value);
    int actual_list_size = 0;

    list_iterator_t it = list_begin(ast_node_get_children(list_node));
    ast_node_t* next = list_current(list_next(&it));
    actual_list_size++;
    while(next != NULL) {
        list_iterator_t children_it = list_begin(ast_node_get_children(next));
        next = list_current(list_next(&children_it));
        actual_list_size++;
    }
    
    if(declared_size != actual_list_size) {
        semantic_errors++;
        fprintf(stderr, "GLOBAL: number of elements in initialization list does not match "
            "declared vector length of %d\n", declared_size);
    }

    return semantic_errors;
}


int check_identifier_definition(ast_node_t* definition_node, symbol_table_t* st, symbol_type_t type, symbol_t* scope) {
    assert(definition_node != NULL);
    assert(ast_node_get_type(definition_node) == ast_int_id_def ||
           ast_node_get_type(definition_node) == ast_char_id_def ||
           ast_node_get_type(definition_node) == ast_float_id_def);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(definition_node);
    ast_node_t* identifier_node = list_current(list_begin(children));

    data_type_t data_type = evaluate_identifier_definition_data_type(definition_node);
    ast_node_set_evaluated_data_type(definition_node, data_type);
    semantic_errors += check_identifier(identifier_node, st, type, data_type, scope);
    return semantic_errors;
}


int check_function_declaration(ast_node_t* declaration_node, symbol_table_t* st) {
    assert(declaration_node != NULL);
    assert(ast_node_get_type(declaration_node) == ast_func_decl);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(declaration_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_definition_node = list_current(it);
    ast_node_t* parameter_list_node = list_current(list_next(&it));
    symbol_t* scope = get_identifier_definition_symbol(identifier_definition_node);

    semantic_errors += check_identifier_definition(identifier_definition_node, st, symbol_function, SYMBOL_SCOPE_GLOBAL);

    scope->parameters = new_list();
    semantic_errors += check_parameter_list(parameter_list_node, st, scope);

    return semantic_errors;
}


int check_function_implementation(ast_node_t* declaration_node, symbol_table_t* st) {
    assert(declaration_node != NULL);
    assert(ast_node_get_type(declaration_node) == ast_func_decl);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(declaration_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_definition_node = list_current(it);
    list_next(&it); // ignore parameters
    ast_node_t* command_node = list_current(list_next(&it));
    symbol_t* scope = get_identifier_definition_symbol(identifier_definition_node);

    if(command_node != NULL) {
        semantic_errors += check_command(command_node, st, scope);
    }

    return semantic_errors;
}


int check_parameter_list(ast_node_t* parameter_list_node, symbol_table_t* st, symbol_t* function) {
    assert(parameter_list_node != NULL);
    assert(ast_node_get_type(parameter_list_node) == ast_func_param);

    int semantic_errors = 0;
    list_t* parameters = ast_node_get_children(parameter_list_node);
    list_iterator_t it = list_begin(parameters);
    ast_node_t* identifier_definition_node = list_current(it);
    ast_node_t* parameter_list_rest = list_current(list_next(&it));

    if(identifier_definition_node != NULL) {
        semantic_errors += check_identifier_definition(identifier_definition_node, st, symbol_parameter, function);
    }

    if(parameter_list_rest != NULL) {
        semantic_errors += check_parameter_list(parameter_list_rest, st, function);
    }
    

    return semantic_errors;
}


int check_command(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope) {
    assert(command_node != NULL);

    int semantic_errors = 0;
    switch(ast_node_get_type(command_node)) {
        case ast_cmd_block:
            semantic_errors += check_command_block(command_node, st, scope);
            break;
        case ast_assign:
            semantic_errors += check_assignment(command_node, st, scope);
            break;
        case ast_vector_assign:
            semantic_errors += check_vector_assignment(command_node, st, scope);
            break;
        case ast_if:
        case ast_if_else:
        case ast_while:
            semantic_errors += check_if_else(command_node, st, scope);
            break;
        case ast_print_type:
            semantic_errors += check_print(command_node, st, scope);
            break;
        case ast_return:
            semantic_errors += check_return(command_node, st, scope);
            break;
        case ast_goto:
            semantic_errors += check_goto(command_node, st, scope);
            break;
        default:
            break;
    }
    return semantic_errors;
}


int check_if_else(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    list_t* children = ast_node_get_children(command_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* if_condition = list_current(it);
    ast_node_t* then_expression = list_current(list_next(&it));
    ast_node_t* else_expression = list_current(list_next(&it));

    semantic_errors += check_expression_type(if_condition, data_type_bool, st, scope);

    semantic_errors += check_command(then_expression, st, scope);

    if(else_expression != NULL) {
        semantic_errors += check_command(else_expression, st, scope);
    }

    return semantic_errors;
}


int check_print(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    
    ast_node_t* print_list = list_current(list_begin(ast_node_get_children(command_node)));


    while(print_list != NULL) {
        ast_node_t* printable = list_current(list_begin(ast_node_get_children(print_list)));
        if(!(ast_node_get_type(printable) == ast_symbol && 
             ast_node_get_symbol(printable)->type == symbol_string_literal)) {
            semantic_errors += check_identifiers_in_expression(printable, st, scope);
        }
        evaluate_expression_data_type(printable, st, scope);
        list_iterator_t it = list_begin(ast_node_get_children(print_list));
        print_list = list_current(list_next(&it));
    }
    
    return semantic_errors;
}


int check_return(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    ast_node_t* return_value = list_current(list_begin(ast_node_get_children(command_node)));
    semantic_errors += check_expression_type(return_value, scope->data_type, st, scope);
    return semantic_errors;
}


int check_goto(ast_node_t* command_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    return semantic_errors;
}


int check_command_block(ast_node_t* command_block_node, symbol_table_t* st, symbol_t* scope) {
    assert(command_block_node != NULL);
    assert(ast_node_get_type(command_block_node) == ast_cmd_block);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(command_block_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* command_list = list_current(it);

    if(command_list != NULL) {
        semantic_errors += check_command_list(command_list, st, scope);
    }

    return semantic_errors;
}

int check_command_list(ast_node_t* command_list_node, symbol_table_t* st, symbol_t* scope) {
    assert(command_list_node != NULL);
    assert(ast_node_get_type(command_list_node) == ast_cmd ||
           ast_node_get_type(command_list_node) == ast_label);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(command_list_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* command_or_label = list_current(it);
    ast_node_t* command_list_rest = list_current(list_next(&it));

    if(command_or_label != NULL) {
        if(ast_node_get_type(command_list_node) == ast_cmd) {
            semantic_errors += check_command(command_or_label, st, scope);
        } else {
            semantic_errors += check_identifier(command_or_label, st, symbol_label, data_type_undefined, scope);
        }
    }

    if(command_list_rest != NULL) {
        semantic_errors += check_command_list(command_list_rest, st, scope);
    }
    return semantic_errors;
}

int check_assignment(ast_node_t* assignment_node, symbol_table_t* st, symbol_t* scope) {
    assert(assignment_node != NULL);
    assert(ast_node_get_type(assignment_node) == ast_assign);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(assignment_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);
    ast_node_t* expression_node = list_current(list_next(&it));
    symbol_t* identifier = ast_node_get_symbol(identifier_node);

    if(!is_identifier_valid_in_scope(st, identifier, scope)) {
        semantic_errors++;
        fprintf(stderr, "%s: %s not declared in this scope\n", 
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : 
                    scope->value, 
                identifier->value);
    } else {
        symbol_t* identifier_in_global_scope = symbol_table_get(st, identifier->value, SYMBOL_SCOPE_GLOBAL);
        symbol_t* identifier_in_function_scope = symbol_table_get(st, identifier->value, scope);
        ast_node_set_symbol(identifier_node, (identifier_in_function_scope != NULL) ? 
                                              identifier_in_function_scope:
                                              identifier_in_global_scope);
    }

    identifier = ast_node_get_symbol(identifier_node);
    if(identifier->type != symbol_variable) {
        semantic_errors++;
        fprintf(stderr, "%s: cannot assign value directly to %s '%s' of type %s\n", 
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : 
                    scope->value,
                symbol_type_to_string(identifier->type),
                identifier->value,
                data_type_to_string(identifier->data_type));
    }

    semantic_errors += check_expression_type(expression_node, identifier->data_type, st, scope);

    return semantic_errors;
}


int check_vector_assignment(ast_node_t* assignment_node, symbol_table_t* st, symbol_t* scope) {
    assert(assignment_node != NULL);
    assert(ast_node_get_type(assignment_node) == ast_vector_assign);

    int semantic_errors = 0;
    list_t* children = ast_node_get_children(assignment_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);
    ast_node_t* index_node = list_current(list_next(&it));
    ast_node_t* expression_node = list_current(list_next(&it));
    symbol_t* identifier = ast_node_get_symbol(identifier_node);

    if(!is_identifier_valid_in_scope(st, identifier, scope)) {
        semantic_errors++;
        fprintf(stderr, "%s: '%s' not declared in this scope\n", 
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : 
                    scope->value,
                    identifier->value);
    }

    if(identifier->type != symbol_vector) {
        semantic_errors++;
        fprintf(stderr, "%s: %s '%s' is not a vector and cannot be indexed\n", 
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : 
                    scope->value,
                    symbol_type_to_string(identifier->type),
                    identifier->value);
    }

    semantic_errors += check_vector_index_type(index_node, st, scope);

    semantic_errors += check_expression_type(expression_node, identifier->data_type, st, scope);

    return semantic_errors;
}


bool identifier_already_declared(symbol_table_t* st, symbol_t* identifier, symbol_t* scope) {
    symbol_t* symbol = symbol_table_get(st, identifier->value, scope);
    return symbol != NULL && symbol->type != symbol_identifier;
}

bool identifier_declared_in_any_other_scope(symbol_table_t* st, symbol_t* identifier, symbol_t* except_scope) {
    if(identifier_already_declared(st, identifier, SYMBOL_SCOPE_GLOBAL)) {
        return true;
    }

    for(symbol_table_iterator_t it = symbol_table_begin(st); symbol_table_current(it); symbol_table_next(&it)) {
        symbol_t* symbol = symbol_table_current(it);
        if(symbol->type == symbol_function && symbol != except_scope) {
            if(identifier_already_declared(st, identifier, symbol)) {
                return true;
            }
        }
    }
    return false;
}

bool is_identifier_valid_in_scope(symbol_table_t* st, symbol_t* identifier, symbol_t* scope) {
    symbol_t* symbol = symbol_table_get(st, identifier->value, scope);
    if(symbol == NULL && scope != SYMBOL_SCOPE_GLOBAL) {
        symbol = symbol_table_get(st, identifier->value, SYMBOL_SCOPE_GLOBAL);
    }
    return symbol != NULL && symbol->type != symbol_identifier;
}

int check_expression_type(ast_node_t* expression_node, data_type_t expected_data_type, symbol_table_t* st, symbol_t* scope) {
    assert(expression_node != NULL);
    assert(is_valid_expression_node_type(expression_node));

    int semantic_errors = 0;

    semantic_errors += check_identifiers_in_expression(expression_node, st, scope);

    data_type_t evaluated_data_type = evaluate_expression_data_type(expression_node, st, scope);
    ast_node_set_evaluated_data_type(expression_node, evaluated_data_type);
    if(!are_compatible_data_types(evaluated_data_type, expected_data_type)) {
        fprintf(stderr, "%s: expression incompatible with type %s\n",
            scope->value,
            data_type_to_string(expected_data_type));
        semantic_errors++;
    }
    return semantic_errors;
}

int check_vector_index_type(ast_node_t* index_node, symbol_table_t* st, symbol_t* scope) {
    assert(index_node != NULL);
    assert(is_valid_expression_node_type(index_node));

    int semantic_errors = 0;

    semantic_errors += check_identifiers_in_expression(index_node, st, scope);

    data_type_t evaluated_data_type = evaluate_expression_data_type(index_node, st, scope);
    ast_node_set_evaluated_data_type(index_node, evaluated_data_type);
    if(evaluated_data_type != data_type_int) {
        fprintf(stderr, "%s: index of vector must be an integer\n",
            scope->value);
        semantic_errors++;
    }
    return semantic_errors;
}

int check_identifiers_in_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    list_iterator_t it = list_begin(ast_node_get_children(expression_node));
    switch(ast_node_get_type(expression_node)) {
    case ast_func_call:
        semantic_errors += check_function_call(expression_node, st, scope);
        break;
    case ast_vector_index:;
        ast_node_t* vector_identifier_node = list_current(it);
        ast_node_t* vector_index_node = list_current(list_next(&it));
        symbol_t* vector_identifier = ast_node_get_symbol(vector_identifier_node);
        if(!is_identifier_valid_in_scope(st, vector_identifier, scope)) {
            semantic_errors++;
            fprintf(stderr, "%s: undefined identifier '%s'\n",
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
                vector_identifier->value);
            break;
        }
        semantic_errors += check_vector_index_type(vector_index_node, st, scope);
        if(vector_identifier->type != symbol_vector) {
            semantic_errors++;
            fprintf(stderr, "%s: identifier %s is not a vector type\n",
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
                vector_identifier->value);
        }
        break;
        
    case ast_symbol:;
        symbol_t* identifier = ast_node_get_symbol(expression_node);
        if(!is_identifier_valid_in_scope(st, identifier, scope)) {
            semantic_errors++;
            fprintf(stderr, "%s: undefined identifier '%s'\n",
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
                identifier->value);
            break;
        } else {
            symbol_t* symbol_in_scope = symbol_table_get(st, identifier->value, scope);
            if(symbol_in_scope != NULL) {
                ast_node_set_symbol(expression_node, symbol_in_scope);
            }
        }
        if(identifier->type == symbol_function ||
           identifier->type == symbol_vector) {
            semantic_errors++;
            fprintf(stderr, "%s: trying to dereference %s identifier %s\n",
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
                symbol_type_to_string(identifier->type),
                identifier->value);
        }
        break;

    default:;
        ast_node_t* child = list_current(it);
        while(child != NULL) {
            semantic_errors += check_identifiers_in_expression(child, st, scope);
            child = list_current(list_next(&it));
        }
        break;
    }
    return semantic_errors;
}

int check_function_call(ast_node_t* function_node, symbol_table_t* st, symbol_t* scope) {
    int semantic_errors = 0;
    list_t* children = ast_node_get_children(function_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);
    ast_node_t* arguments = list_current(list_next(&it));
    symbol_t* identifier = ast_node_get_symbol(identifier_node);
    

    if(!is_identifier_valid_in_scope(st, identifier, scope)) {
        semantic_errors++;
        fprintf(stderr, "%s: undefined identifier '%s'\n",
                scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
                identifier->value);
        return semantic_errors;
    }

    if(identifier->type != symbol_function) {
        semantic_errors++;
        fprintf(stderr, "%s: trying to call non-function identifier %s\n",
            scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
            identifier->value);
        return semantic_errors;
    }

    ast_node_set_evaluated_data_type(function_node, identifier->data_type);

    list_iterator_t parameter_it = list_begin(identifier->parameters);
    while(list_current(parameter_it) != NULL && arguments != NULL) {
        list_iterator_t argument_it = list_begin(ast_node_get_children(arguments));
        ast_node_t* expression = list_current(argument_it);
        symbol_t* current_parameter = list_current(parameter_it);
        
        semantic_errors += check_expression_type(expression, current_parameter->data_type, st, scope);

        arguments = list_current(list_next(&argument_it));
        list_next(&parameter_it);
    }
    if(list_current(parameter_it) != NULL) {
        semantic_errors++;
        fprintf(stderr, "%s: too few arguments passed to function %s\n",
            scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
            identifier->value);
    } else if (arguments != NULL) {
        semantic_errors++;
        fprintf(stderr, "%s: too many arguments passed to function %s\n",
            scope == SYMBOL_SCOPE_GLOBAL ? "GLOBAL" : scope->value,
            identifier->value);
    }

    return semantic_errors;
}


bool data_type_is_numeric(data_type_t data_type) {
    switch(data_type) {
        case data_type_bool:
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


data_type_t evaluate_expression_data_type(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope){
    data_type_t type = data_type_undefined;
    switch(ast_node_get_type(expression_node)) {
        case ast_symbol:
            type = evaluate_symbol_data_type(ast_node_get_symbol(expression_node), st, scope);
            break;

        case ast_vector_index:
            type = evaluate_vector_data_type(expression_node, st, scope);
            break;

        case ast_func_call:
            type = evaluate_function_data_type(expression_node, st, scope);
            break;

        case ast_read:
            type = data_type_char;
            break;

        case ast_sum:
        case ast_sub:
        case ast_mul:
        case ast_div:
            type = evaluate_arithmetic_expression(expression_node, st, scope);
            break;

        case ast_eq:
        case ast_dif:
        case ast_lt:
        case ast_gt:
        case ast_le:
        case ast_ge:
            type = evaluate_boolean_expression(expression_node, st, scope);
            break;

        default:
            break;
    }
    ast_node_set_evaluated_data_type(expression_node, type);
    return type;
}


data_type_t evaluate_symbol_data_type(symbol_t* symbol, symbol_table_t* st, symbol_t* scope) {
    symbol_t* symbol_in_scope = symbol_table_get(st, symbol->value, scope);
    if(symbol_in_scope != NULL) {
        symbol = symbol_in_scope;
    }
    switch(symbol->type) {
        case symbol_variable:
        case symbol_vector:
        case symbol_function:
        case symbol_parameter:
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


data_type_t evaluate_arithmetic_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope) {
    list_t* operands = ast_node_get_children(expression_node);
    list_iterator_t it = list_begin(operands);
    ast_node_t* op1 = list_current(it);
    ast_node_t* op2 = list_current(list_next(&it));
    
    data_type_t op1_data_type = evaluate_expression_data_type(op1, st, scope);
    data_type_t op2_data_type = evaluate_expression_data_type(op2, st, scope);

    if(data_type_is_numeric(op1_data_type) && data_type_is_numeric(op2_data_type)) {
        return resulting_data_type(op1_data_type, op2_data_type);
    } else {
        return data_type_undefined;
    }
}


data_type_t evaluate_boolean_expression(ast_node_t* expression_node, symbol_table_t* st, symbol_t* scope) {
    list_t* operands = ast_node_get_children(expression_node);
    list_iterator_t it = list_begin(operands);
    ast_node_t* op1 = list_current(it);
    ast_node_t* op2 = list_current(list_next(&it));
    
    data_type_t op1_data_type = evaluate_expression_data_type(op1, st, scope);
    data_type_t op2_data_type = evaluate_expression_data_type(op2, st, scope);

    if((data_type_is_numeric(op1_data_type) && data_type_is_numeric(op2_data_type))) {
        return data_type_bool;
    } else {
        return data_type_undefined;
    }
}


data_type_t evaluate_vector_data_type(ast_node_t* vector_node, symbol_table_t* st, symbol_t* scope) {
    list_t* children = ast_node_get_children(vector_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);

    return evaluate_symbol_data_type(ast_node_get_symbol(identifier_node), st, scope);
}


data_type_t evaluate_function_data_type(ast_node_t* function_node, symbol_table_t* st, symbol_t* scope) {
    list_t* children = ast_node_get_children(function_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);

    return evaluate_symbol_data_type(ast_node_get_symbol(identifier_node), st, scope);
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


symbol_t* get_identifier_definition_symbol(ast_node_t* identifier_definition_node) {
    assert(identifier_definition_node != NULL);
    assert(ast_node_get_type(identifier_definition_node) == ast_int_id_def ||
           ast_node_get_type(identifier_definition_node) == ast_char_id_def ||
           ast_node_get_type(identifier_definition_node) == ast_float_id_def);

    list_t* children = ast_node_get_children(identifier_definition_node);
    list_iterator_t it = list_begin(children);
    ast_node_t* identifier_node = list_current(it);
    return ast_node_get_symbol(identifier_node);
}


bool is_valid_expression_node_type(ast_node_t* expression_node) {
    return ast_node_get_type(expression_node) == ast_vector_index ||
           ast_node_get_type(expression_node) == ast_func_call ||
           ast_node_get_type(expression_node) == ast_symbol ||
           ast_node_get_type(expression_node) == ast_read ||
           ast_node_get_type(expression_node) == ast_sum ||
           ast_node_get_type(expression_node) == ast_sub ||
           ast_node_get_type(expression_node) == ast_mul ||
           ast_node_get_type(expression_node) == ast_div ||
           ast_node_get_type(expression_node) == ast_dif ||
           ast_node_get_type(expression_node) == ast_eq ||
           ast_node_get_type(expression_node) == ast_lt ||
           ast_node_get_type(expression_node) == ast_gt ||
           ast_node_get_type(expression_node) == ast_le ||
           ast_node_get_type(expression_node) == ast_ge;
}


const char* symbol_type_to_string(symbol_type_t type) {
    switch(type) {
        case symbol_int_literal:
            return "int literal";
            break;
        case symbol_char_literal:
            return "char literal";
            break;
        case symbol_string_literal:
            return "string literal";
            break;
        case symbol_identifier:
            return "identifier";
            break;
        case symbol_variable:
            return "variable";
            break;
        case symbol_vector:
            return "vector";
            break;
        case symbol_function:
            return "function";
            break;
        case symbol_parameter:
            return "parameter";
            break;
        case symbol_label:
            return "label";
            break;
        default:
            return "";
            break;
    }
}


const char* data_type_to_string(data_type_t type) {
    switch(type) {
        case data_type_undefined:
            return "undefined";
            break;
        case data_type_int:
            return "int";
            break;
        case data_type_char:
            return "char";
            break;
        case data_type_float:
            return "float";
            break;
        case data_type_string:
            return "string";
            break;
        case data_type_bool:
            return "bool";
            break;
        default:
            return "";
            break;
    }
}
