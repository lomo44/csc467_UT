%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
  int num;
  float FP;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token         
        BOOL
        BVEC2
        BVEC3
        BVEC4
        INT
        IVEC2
        IVEC3
        IVEC4
        FLOAT
        FVEC2
        FVEC3
        FVEC4
        SEMICOLON
        COMMA
        OPEN_CURLY_BRACKET
        CLOSE_CURLY_BRACKET
        OPEN_ROUND_BRACKET
        CLOSE_ROUND_BRACKET
        //OPEN_COMMENT
        //CLOSE_COMMENT
        COMMENT
        OPEN_SQUARE_BRACKET
        CLOSE_SQUARE_BRACKET
        OP_ASSIGN
        OP_MULT
        OP_MINUS
        OP_PLUS
        //UNARY_MINUS
        //BINARY_MINUS
        OP_DIV
        LOGIC_NOT
        LOGIC_AND
        LOGIC_OR
        OP_XOR
        OP_EQ
        OP_NEQ
        OP_LT
        OP_LTE
        OP_GT
        OP_GTE
        OP_QST
        FUNC_DP3
        FUNC_LIT
        FUNC_RSQ
        QUALIFIER_CONST
        STMT_WHILE
        STMT_IF
        STMT_ELSE
        //SYMBOL_QUOTES
        //COLON
        IDENTIFIER
        CONST_INT
        CONST_FLOAT
        CONST_TRUE//SYMBOL_TRUE
        CONST_FALSE//SYMBOL_FALSE

%start    program


%left LOGIC_OR
%left LOGIC_AND
%left OP_EQ OP_NEQ OP_LTE OP_LT OP_GT OP_GTE
%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIV
%right OP_XOR
%left LOGIC_NOT UMINUS
%left OPEN_SQUARE_BRACKET
      CLOSE_SQUARE_BRACKET
      OPEN_ROUND_BRACKET
      CLOSE_ROUND_BRACKET
%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  : scope
  ;
scope
  : OPEN_CURLY_BRACKET declarations statements CLOSE_CURLY_BRACKET
  ;
declarations
  : declarations declaration
  | declaration
  ;
statements
  : statements statement
  | statement
  ;
declaration
  : type IDENTIFIER SEMICOLON
  | type IDENTIFIER OP_ASSIGN expression SEMICOLON
  | QUALIFIER_CONST type IDENTIFIER OP_EQ expression SEMICOLON
  | %empty
  ;
statement
  : variable OP_ASSIGN expression SEMICOLON
  | STMT_IF OPEN_ROUND_BRACKET expression CLOSE_ROUND_BRACKET statement else_statement
  | STMT_WHILE OPEN_ROUND_BRACKET expression CLOSE_ROUND_BRACKET statement
  | SEMICOLON
  | scope
  ;
else_statement
  : STMT_ELSE statement
  | %empty
  ;
type
  : INT
  | IVEC2
  | IVEC3
  | IVEC4
  | BOOL
  | BVEC2
  | BVEC3
  | BVEC4
  | FLOAT
  | FVEC2
  | FVEC3
  | FVEC4
  ;
expression
  : constructor
  | function
  | CONST_INT
  | CONST_FLOAT
  | variable
  | unary_op expression %prec UMINUS
  | expression binary_op expression
  | CONST_TRUE
  | CONST_FALSE
  | OPEN_ROUND_BRACKET expression CLOSE_ROUND_BRACKET
  ;
variable
  : IDENTIFIER
  | IDENTIFIER OPEN_SQUARE_BRACKET CONST_INT CLOSE_SQUARE_BRACKET
  ;
unary_op
  : LOGIC_NOT
  | OP_MINUS
  ;
binary_op
  : LOGIC_AND
  | LOGIC_OR
  | OP_EQ
  | OP_NEQ
  | OP_LT
  | OP_LTE
  | OP_GT
  | OP_GTE
  | OP_PLUS
  | OP_MINUS
  | OP_MULT
  | OP_DIV
  | OP_XOR
  ;
constructor
  : type OPEN_ROUND_BRACKET arguments CLOSE_ROUND_BRACKET
  ;
function
  : function_name OPEN_ROUND_BRACKET arguments_opt CLOSE_ROUND_BRACKET
  ;
function_name
  : FUNC_DP3
  | FUNC_LIT
  | FUNC_RSQ
  ;
arguments_opt
  : arguments
  | %empty
  ;
arguments
  : arguments COMMA expression
  | expression
  ;
%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

