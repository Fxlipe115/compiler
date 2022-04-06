%{
#include "stdbool.h"
#include "symbol_table.h"
#include "parser.h"

int line_count;
bool is_running;
symbol_table_t* st;

int yydebug = 0;
%}

ID      [a-z_\-]+
SPECIAL [-,;:\(\)\[\]\{\}+*/<>=]
CHAR    '[^']'
STRING  \"[^"\n]*\"
INT     [0-9]+

%option nounput
%option noinput
%x COMMENT

%%

[\t ]
"\n"        { line_count++; }

char        { return KW_CHAR; }
int         { return KW_INT; }
float       { return KW_FLOAT; }
if          { return KW_IF; }
then        { return KW_THEN; }
else        { return KW_ELSE; }
while       { return KW_WHILE; }
goto        { return KW_GOTO; }
read        { return KW_READ; }
print       { return KW_PRINT; }
return      { return KW_RETURN; }
{SPECIAL}   { return yytext[0]; }
"<="        { return OPERATOR_LE; }
">="        { return OPERATOR_GE; }
"=="        { return OPERATOR_EQ; }
"!="        { return OPERATOR_DIF; }
{ID}        { yylval.symbol = symbol_table_add(st, yytext, symbol_identifier, line_count); 
              return TK_IDENTIFIER; }
{CHAR}      { yylval.symbol = symbol_table_add(st, yytext, symbol_char_literal, line_count); 
              return LIT_CHAR; }
{STRING}    { yylval.symbol = symbol_table_add(st, yytext, symbol_string_literal, line_count);
              return LIT_STRING; }
{INT}       { yylval.symbol = symbol_table_add(st, yytext, symbol_int_literal, line_count);
              return LIT_INTEGER; }

"\\*"           { BEGIN(COMMENT); }
<COMMENT>"*\\"  { BEGIN(INITIAL); }
<COMMENT>"\n"   { line_count++; }
<COMMENT>.
"\\\\".*

.           { return TOKEN_ERROR; }

%%

#include "lex_helper_functions.h"

int getLineNumber(void) {
    return line_count;
}

bool isRunning(void) {
    return is_running;
}

void lex_init(symbol_table_t* table, int print_parser_steps, 
              int print_scanner_steps) {
    yydebug = print_parser_steps;
    yy_flex_debug = print_scanner_steps;

    st = table;
    line_count = 1;
    is_running = true;
}

int yywrap() {
    is_running = false;
    yylex_destroy();
    return 1;
}

