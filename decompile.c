#include "decompile.h"

#include <stdio.h>

void ast_node_to_text(ast_node_t* node, char* text) {

void decompile(ast_t* ast, char* code) {
    code[0] = '\0';

    ast_node_to_text(ast->root, code);
}

void ast_node_to_text(ast_node_t* node, char* text) {
    sprintf(text, "%s", "a");
}
