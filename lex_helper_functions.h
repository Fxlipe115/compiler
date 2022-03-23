#ifndef LEX_HELPER_FUNCTIONS_H
#define LEX_HELPER_FUNCTIONS_H

#include <stdbool.h>

#include "symbol_table.h"

int getLineNumber(void);

bool isRunning(void);

void lex_init(symbol_table_t* table, int print_parser_steps, 
              int print_scanner_steps);

#endif
