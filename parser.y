%{
#include "lex.yy.h"
#include "symbol_table.h"
#include "syntax_tree.h"

int yyerror(const char*);
int yylex(void);

ast_t* ast = NULL;
%}

%union {
    symbol_t* symbol;
    ast_node_t* ast;
}

%token KW_CHAR
%token KW_INT
%token KW_FLOAT

%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_WHILE
%token KW_GOTO
%token KW_READ
%token KW_PRINT
%token KW_RETURN

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

%token <symbol> TK_IDENTIFIER

%token <symbol> LIT_INTEGER
%token <symbol> LIT_CHAR
%token <symbol> LIT_STRING

%token TOKEN_ERROR

%type <ast> program
%type <ast> decl_list
%type <ast> decl_list_rest
%type <ast> decl
%type <ast> identifier_def
%type <ast> integer_list
%type <ast> integer_list_rest
%type <ast> integer_literal
%type <ast> parameter_list
%type <ast> parameter_list_rest
%type <ast> cmd
%type <ast> cmd_block
%type <ast> cmd_list
%type <ast> attribution
%type <ast> printable_list
%type <ast> printable_list_rest
%type <ast> flux_control
%type <ast> expr
%type <ast> args_list
%type <ast> args_list_rest
%type <ast> identifier
%type <ast> int_literal
%type <ast> char_literal
%type <ast> string_literal

%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%left '+' '-'
%left '*' '/'

%start program

%%

program: decl_list  
       { 
           ast_set_root(ast, new_ast_node(ast_program, 1, $1)); 
       }
       ;

decl_list: decl decl_list_rest
         { 
             $$ = new_ast_node(ast_decl, 2, $1, $2); 
         }
         |                  
         { 
             $$ = NULL; 
         }
         ;

decl_list_rest: decl decl_list_rest 
              { 
                  $$ = new_ast_node(ast_decl, 2, $1, $2); 
              }
              |                     
              { 
                  $$ = NULL; 
              }
              ;

decl: KW_INT identifier ':' integer_literal ';'  
    { 
        $$ = new_ast_node(ast_int_decl, 2, $2, $4); 
    }
    | KW_CHAR identifier ':' integer_literal ';' 
    { 
        $$ = new_ast_node(ast_char_decl, 2, $2, $4); 
    }
    | KW_FLOAT identifier ':' int_literal '/' int_literal ';'    
    { 
        $$ = new_ast_node(ast_float_decl, 3, $2, $4, $6); 
    }
    | identifier_def '[' int_literal ']' ';'
    { 
        $$ = new_ast_node(ast_vector_decl, 2, $1, $3); 
    }
    | identifier_def '[' int_literal ']' ':' integer_list ';'
    { 
        $$ = new_ast_node(ast_vector_init_decl, 3, $1, $3, $6); 
    }
    | identifier_def '(' parameter_list ')' cmd
    { 
        $$ = new_ast_node(ast_func_decl, 3, $1, $3, $5); 
    }
    ;

identifier_def: KW_INT identifier    
              { 
                  $$ = new_ast_node(ast_int_id_def, 1, $2); 
              }
              | KW_CHAR identifier
              { 
                  $$ = new_ast_node(ast_char_id_def, 1, $2); 
              }
              | KW_FLOAT identifier
              { 
                  $$ = new_ast_node(ast_float_id_def, 1, $2); 
              }
              ;

integer_list: integer_literal integer_list_rest
            {
                $$ = new_ast_node(ast_vector_init_value, 2, $1, $2);
            }
            | 
            {
                $$ = NULL;
            }
            ;

integer_list_rest: integer_literal integer_list_rest
                 {
                     $$ = new_ast_node(ast_vector_init_value, 2, $1, $2);
                 }
                 | 
                 {
                     $$ = NULL;
                 }
                 ;

integer_literal: char_literal       { $$ = $1; }
               | int_literal    { $$ = $1; }
               ;

parameter_list: identifier_def parameter_list_rest
              {
                  $$ = new_ast_node(ast_func_param, 2, $1, $2);
              }
              | 
              {
                  $$ = NULL;
              }
              ;

parameter_list_rest: ',' identifier_def parameter_list_rest
                   {
                       $$ = new_ast_node(ast_func_param, 2, $2, $3);
                   }
                   | 
                   {
                       $$ = NULL;
                   }
                   ;

cmd: cmd_block
   {
       $$ = $1;
   }
   | attribution
   {
       $$ = $1;
   }
   | flux_control
   {
       $$ = $1;
   }
   | KW_PRINT printable_list
   {
       $$ = new_ast_node(ast_print_type, 1, $2);
   }
   | KW_RETURN expr
   {
       $$ = new_ast_node(ast_return, 1, $2);
   }
   |
   {
       $$ = NULL;
   }
   ;

cmd_block: '{' cmd_list '}'
         {
             $$ = new_ast_node(ast_cmd_block, 1, $2);
         }
         ;

cmd_list: cmd ';' cmd_list
        {
             $$ = new_ast_node(ast_cmd, 2, $1, $3);
         }
        | identifier ':' cmd_list
         {
             $$ = new_ast_node(ast_label, 2, $1, $3);
         }
        |
         {
             $$ = NULL;
         }
        ;

attribution: identifier '=' expr                 
           {
             $$ = new_ast_node(ast_assign, 2, $1, $3);
         }
           | identifier '[' expr ']' '=' expr
         {
             $$ = new_ast_node(ast_vector_assign, 3, $1, $3, $6);
         }
           ;


printable_list: string_literal printable_list_rest
              {
             $$ = new_ast_node(ast_print_arg, 2, $1, $2);
         }
              | expr printable_list_rest
         {
             $$ = new_ast_node(ast_print_arg, 2, $1, $2);
         }
              |
         {
             $$ = NULL;
         }
              ;

printable_list_rest: ',' string_literal printable_list_rest
                   {
             $$ = new_ast_node(ast_print_arg, 2, $2, $3);
         }
                   | ',' expr printable_list_rest
         {
             $$ = new_ast_node(ast_print_arg, 2, $2, $3);
         }
                   |
         {
             $$ = NULL;
         }
                   ;

flux_control: KW_IF expr KW_THEN cmd
            {
             $$ = new_ast_node(ast_if, 2, $2, $4);
         }
            | KW_IF expr KW_THEN cmd KW_ELSE cmd
         {
             $$ = new_ast_node(ast_if_else, 3, $2, $4, $6);
         }
            | KW_WHILE expr cmd
         {
             $$ = new_ast_node(ast_while, 2, $2, $3);
         }
            | KW_GOTO identifier
         {
             $$ = new_ast_node(ast_goto, 1, $2);
         }
            ;

expr: identifier                     
    { 
        $$ = $1; 
    }
    | identifier '[' expr ']'        
    { 
        $$ = new_ast_node(ast_vector_index, 2, $1, $3); 
    }
    | int_literal                       
    { 
        $$ = $1; 
    }
    | char_literal                          
    { 
        $$ = $1; 
    }
    | '(' expr ')'                      
    { 
        $$ = $2; 
    }
    | expr '+' expr                     
    { 
        $$ = new_ast_node(ast_sum, 2, $1, $3); 
    }
    | expr '-' expr                     
    { 
        $$ = new_ast_node(ast_sub, 2, $1, $3); 
    }
    | expr '*' expr                     
    { 
        $$ = new_ast_node(ast_mul, 2, $1, $3); 
    }
    | expr '/' expr                     
    { 
        $$ = new_ast_node(ast_div, 2, $1, $3); 
    }
    | expr '<' expr                    
    { 
        $$ = new_ast_node(ast_lt, 2, $1, $3); 
    }
    | expr '>' expr                     
    { 
        $$ = new_ast_node(ast_gt, 2, $1, $3); 
    }
    | expr OPERATOR_LE expr             
    { 
        $$ = new_ast_node(ast_le, 2, $1, $3); 
    }
    | expr OPERATOR_GE expr             
    { 
        $$ = new_ast_node(ast_ge, 2, $1, $3); 
    }
    | expr OPERATOR_EQ expr             
    { 
        $$ = new_ast_node(ast_eq, 2, $1, $3); 
    }
    | expr OPERATOR_DIF expr            
    { 
        $$ = new_ast_node(ast_dif, 2, $1, $3); 
    }
    | identifier '(' args_list ')'   
    { 
        $$ = new_ast_node(ast_func_call, 2, $1, $3); 
    }
    | KW_READ                           
    { 
        $$ = new_ast_node(ast_read, 0);
    }
    ;

args_list: expr args_list_rest          
         { 
             $$ = new_ast_node(ast_func_arg, 2, $1, $2); 
         }
         |                              
         { 
             $$ = NULL; 
         }
         ;

args_list_rest: ',' expr args_list_rest 
              { 
                  $$ = new_ast_node(ast_func_arg, 2, $2, $3); 
              }
              |                         
              { 
                  $$ = NULL; 
              }
              ;

identifier: TK_IDENTIFIER
          {
              $$ = new_ast_symbol_node($1);
          }
          ;

int_literal: LIT_INTEGER
           {
               $$ = new_ast_symbol_node($1);
           }
           ;

char_literal: LIT_CHAR
            {
                $$ = new_ast_symbol_node($1);
            }
            ;

string_literal: LIT_STRING
              {
                  $$ = new_ast_symbol_node($1);
              }
              ;

%%
#include <stdio.h>
#include <stdlib.h>
#include "lex_helper_functions.h"
#include "yacc_helper_functions.h"

void yacc_init(ast_t* syntax_tree) {
    ast = syntax_tree;
}

int yyerror(const char* error_message) {
    fprintf(stderr, "Line %d: %s\n", getLineNumber(), error_message);
    exit(3);
}

