#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "syntax_tree.h"

typedef struct ast_node_list ast_node_list_t;

typedef struct ast_node_list_node ast_node_list_node_t;

struct ast {
    ast_node_t* root;
};

struct ast_node {
    ast_node_type_t type;
    symbol_t* symbol;
    ast_node_list_t* children;
};

struct ast_node_list_node {
    ast_node_t* node;
    struct ast_node_list_node* next;
};

struct ast_node_list {
    ast_node_list_node_t* first;
    size_t size;
};

ast_node_list_t* new_ast_node_list();
void delete_ast_node_list(ast_node_list_t* list);
void ast_node_list_insert(ast_node_list_t* list, ast_node_t* node);
ast_node_t* ast_node_list_get(ast_node_list_t* list, size_t index);
void ast_node_print(FILE* stream, ast_node_t* node, int depth);
const char* ast_type_to_string(ast_node_type_t type);
void ast_node_decompile(FILE* stream, ast_node_t* node);
void ast_abstract_node_list_print(FILE* stream, ast_node_t* node, 
                                  const char* separator);

void decompile(FILE* stream, ast_t* ast) {
    ast_node_decompile(stream, ast->root);
}

ast_t* new_ast() {
    ast_t* ast = malloc(sizeof(ast_t));
    ast->root = NULL;
    return ast;
}

void ast_set_root(ast_t* ast, ast_node_t* root) {
    ast->root = root;
}

void delete_ast(ast_t* ast) {
    delete_ast_node(ast->root);
    free(ast);
}

ast_node_t* new_ast_node(ast_node_type_t type, size_t children_quantity,...) {
    ast_node_t* new_node = malloc(sizeof(ast_node_t));
    new_node->children = new_ast_node_list();
    new_node->type = type;
    new_node->symbol = NULL;

    va_list ap;
    va_start(ap, children_quantity);
    for(int i = 0; i < children_quantity; i++) {
        ast_node_t* child = (ast_node_t*)va_arg(ap, ast_node_type_t*);
        if(child != NULL) {
            ast_node_list_insert(new_node->children, child);
        }
    }
    va_end(ap);
    return new_node;
}

ast_node_t* new_ast_symbol_node(symbol_t* symbol) {
    ast_node_t* new_node = malloc(sizeof(ast_node_t));

    new_node->symbol = symbol;
    new_node->type = ast_symbol;
    new_node->children = new_ast_node_list();

    return new_node;
}

void delete_ast_node(ast_node_t* node) {
    delete_ast_node_list(node->children);
    free(node);
}

ast_node_list_t* new_ast_node_list() {
    ast_node_list_t* new_list = malloc(sizeof(ast_node_list_t));
    new_list->first = NULL;
    new_list->size = 0;
    return new_list;
}

void delete_ast_node_list(ast_node_list_t* list) {
    ast_node_list_node_t* list_node = list->first;
    while (list_node != NULL) {
        ast_node_list_node_t* list_node_to_delete = list_node;
        list_node = list_node->next;
        delete_ast_node(list_node_to_delete->node);
        free(list_node_to_delete);
    }
    free(list);
}
void ast_node_list_insert(ast_node_list_t* list, ast_node_t* node) {
    ast_node_list_node_t* new_list_node = malloc(sizeof(ast_node_list_node_t));
    new_list_node->node = node;
    new_list_node->next = list->first;

    list->first = new_list_node;
    list->size++;
}

ast_node_t* ast_node_list_get(ast_node_list_t* list, size_t index) {
    ast_node_list_node_t* list_node = list->first;
    index = list->size - index - 1;
    for (int i = 0; i < index; i++) {
        if (list_node == NULL) {
            return NULL;
        }
        list_node = list_node->next;
    }
    return list_node->node;    
}

void ast_print(FILE* stream, ast_t* ast) {
    fprintf(stream, "Printing Abstract Syntax Tree:\n");
    if (ast == NULL) {
        return;
    }
    ast_node_print(stream, ast->root, 0);
}

void ast_node_print(FILE* stream, ast_node_t* node, int depth) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        fprintf(stream, "  ");
    }

    if (node->type == ast_symbol && node->symbol != NULL) {
        fprintf(stream, "( %d ) - %s %s.\n", depth, 
                ast_type_to_string(node->type), node->symbol->value);
    } else {
        fprintf(stream, "( %d ) - %s.\n", depth, 
                ast_type_to_string(node->type));
    }
    
    for(int child_index = 0; child_index < node->children->size; child_index++) {
        ast_node_print(stream, ast_node_list_get(node->children, child_index), 
                       depth+1);
    }
}

const char* ast_type_to_string(ast_node_type_t type) {
    switch (type) {
        case ast_symbol:
            return "Symbol";
            break;
        case ast_sum:
            return "Sum";
            break;
        case ast_sub:
            return "Subtraction";
            break;
        case ast_mul:
            return "Multiplication";
            break;
        case ast_div:
            return "Division";
            break;
        case ast_lt:
            return "Less than";
            break;
        case ast_gt:
            return "Greater than";
            break;
        case ast_le:
            return "Less than or equal";
            break;
        case ast_ge:
            return "Greater than or equal";
            break;
        case ast_eq:
            return "Equal";
            break;
        case ast_dif:
            return "Different";
            break;
        case ast_vector_index:
            return "Vector indexation";
            break;
        case ast_func_call:
            return "Function call";
            break;
        case ast_read:
            return "Read";
            break;
        case ast_assign:
            return "Assignment";
            break;
        case ast_func_arg:
            return "Function argument";
            break;
        case ast_vector_init_value:
            return "Vector initialization";
            break;
        case ast_func_param:
            return "Function definition parameter";
            break;
        case ast_program:
            return "Program";
            break;
        case ast_decl:
            return "Global declaration";
            break;
        case ast_int_decl:
            return "Int declaration";
            break;
        case ast_char_decl:
            return "Char declaration";
            break;
        case ast_float_decl:
            return "Float declaration";
            break;
        case ast_vector_decl:
            return "Vector declaration";
            break;
        case ast_vector_init_decl:
            return "Vector declaration";
            break;
        case ast_func_decl:
            return "Function declaration";
            break;
        case ast_int_id_def:
            return "Int definition";
            break;
        case ast_char_id_def:
            return "Char definition";
            break;
        case ast_float_id_def:
            return "Float definition";
            break;
        case ast_cmd_block:
            return "Command block";
            break;
        case ast_print_type:
            return "Print";
            break;
        case ast_return:
            return "Return";
            break;
        case ast_cmd:
            return "Command";
            break;
        case ast_label:
            return "Label";
            break;
        case ast_vector_assign:
            return "Vector assignment";
            break;
        case ast_print_arg:
            return "Printable";
            break;
        case ast_if:
            return "If";
            break;
        case ast_if_else:
            return "If else";
            break;
        case ast_while:
            return "While";
            break;
        case ast_goto:
            return "Goto";
            break;
        default:
            return "TEXT_NOT_DEFINED";
            break;
    }
}  

void ast_node_decompile(FILE* stream, ast_node_t* node) {
    if (node == NULL) {
        return;
    }
    switch (node->type) {
        case ast_symbol:
            fprintf(stream, "%s", node->symbol->value);
            break;
        case ast_program:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;

        case ast_decl:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_int_decl:
            fprintf(stream, "int ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, ": ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, ";\n");
            break;
        case ast_char_decl:
            fprintf(stream, "char ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, ": ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, ";\n");
            break;
        case ast_float_decl:
            fprintf(stream, "float ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, ": ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, "/");
            ast_node_decompile(stream, ast_node_list_get(node->children, 2));
            fprintf(stream, ";\n");
            break;
        case ast_vector_decl:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "[");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, "]");
            fprintf(stream, ";\n");
            break;
        case ast_vector_init_decl:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "[");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, "]");
            fprintf(stream, ": ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 2));
            fprintf(stream, ";\n");
            break;
        case ast_vector_init_value:
            ast_abstract_node_list_print(stream, node, " ");
            break;
        case ast_func_decl:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "(");
            if(node->children->size == 3) {
                ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            }
            fprintf(stream, ") ");
            if(node->children->size < 3) {
                ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            } else {
                ast_node_decompile(stream, ast_node_list_get(node->children, 2));
            }
            fprintf(stream, "\n");
            break;
        case ast_func_param:
            ast_abstract_node_list_print(stream, node, ", ");
            break;
        case ast_int_id_def:
            fprintf(stream, "int ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;
        case ast_char_id_def:
            fprintf(stream, "char ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;
        case ast_float_id_def:
            fprintf(stream, "float ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;

        case ast_cmd_block:
            fprintf(stream, "{\n");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "}");
            break;
        case ast_cmd:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, ";\n");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_label:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, ":\n");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        
        case ast_sum:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " + ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_sub:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " - ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_mul:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " * ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_div:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " / ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_lt:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " < ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_gt:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " > ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_le:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " <= ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_ge:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " >= ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_eq:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " == ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_dif:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " != ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;

        case ast_assign:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " = ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_vector_assign:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "[");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, "] = ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 2));
            break;
        case ast_vector_index:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "[");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, "]");
            break;
        case ast_print_type:
            fprintf(stream, "print ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;
        case ast_print_arg:
            ast_abstract_node_list_print(stream, node, ", ");
            break;
        case ast_return:
            fprintf(stream, "return ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;
        case ast_goto:
            fprintf(stream, "goto ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            break;
        case ast_read:
            fprintf(stream, "read");
            break;
        case ast_func_call:
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, "(");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            fprintf(stream, ")");
            break;
        case ast_func_arg:
            ast_abstract_node_list_print(stream, node, ", ");
            break;
        case ast_if:
            fprintf(stream, "if ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " then ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        case ast_if_else:
            fprintf(stream, "if ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " then ");
            if(node->children->size == 3) {
                ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            }
            fprintf(stream, " else ");
            if(node->children->size < 3) {
                ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            } else {
                ast_node_decompile(stream, ast_node_list_get(node->children, 2));
            }
            break;
        case ast_while:
            fprintf(stream, "while ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 0));
            fprintf(stream, " ");
            ast_node_decompile(stream, ast_node_list_get(node->children, 1));
            break;
        default:
            break;
    }
}

void ast_abstract_node_list_print(FILE* stream, ast_node_t* node, 
                                  const char* separator) {
    ast_node_t* element = node;
    ast_node_decompile(stream, ast_node_list_get(element->children, 0));
    ast_node_t* next_element = ast_node_list_get(element->children, 1);
    while(next_element != NULL) {
        fprintf(stream, "%s", separator);
        element = next_element;
        ast_node_decompile(stream, ast_node_list_get(element->children, 0));
        next_element = ast_node_list_get(element->children, 1);
    }
}

