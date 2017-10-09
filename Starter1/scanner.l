%{
/**********************************************************************
 *
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"
#include "math.h"
#include <stdint.h>
#include <float.h>
#include <math.h>
#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS          [ \t]
DIGIT       [0-9]
WORD        [A-Za-z0-9_]
ALPHABET    [A-Za-z_]
NONE_ZERO   [1-9]
%%

    /*******************KEYWORD--DATATYPE*********************/

"bool"        { yTRACE(BOOL);  return(BOOL);}
"bvec2"       { yTRACE(BVEC2); return(BVEC2);}
"bvec3"       { yTRACE(BVEC3); return(BVEC3);}
"bvec4"       { yTRACE(BVEC4); return(BVEC4);}
"int"         { yTRACE(INT);   return(INT);}
"ivec2"       { yTRACE(IVEC2); return(IVEC2);}
"ivec3"       { yTRACE(IVEC3); return(IVEC3);}
"ivec4"       { yTRACE(IVEC4); return(IVEC4);}
"float"       { yTRACE(FLOAT); return(FLOAT);}
"vec2"        { yTRACE(FVEC2); return(FVEC2);}
"vec3"        { yTRACE(FVEC3); return(FVEC3);}
"vec4"        { yTRACE(FVEC4); return(FVEC4);}

    /*******************FUNCTION******************************/
"dp3"         { yTRACE(FUNC_DP3);return(FUNC_DP3);}
"lit"         { yTRACE(FUNC_LIT);return(FUNC_LIT);}
"rsq"         { yTRACE(FUNC_RSQ);return(FUNC_RSQ);}

    /*******************KEYWORD--STATEMENT********************/

"if"          { yTRACE(STMT_IF);     return(STMT_IF);}
"else"        { yTRACE(STMT_ELSE);   return(STMT_ELSE);}
"while"       { yTRACE(STMT_WHILE);  return(STMT_WHILE);}
    
    /*******************KEYWORD--QUALIFIER********************/

"const"       { yTRACE(QUALIFIER_CONST); return(QUALIFIER_CONST);}
    
    /************************CONSTANT*************************/
0{DIGIT}+                               {yERROR("Unknown Token");}
{DIGIT}+{ALPHABET}+                    {yERROR("Unknown Token");}
"."{DIGIT}*{ALPHABET}+{DIGIT}*         {yERROR("Unknown Token");}
{DIGIT}*{ALPHABET}+{DIGIT}*"."          {yERROR("Unknown Token");}

0|{NONE_ZERO}{DIGIT}*   {
                    long num = atol(yytext);
                    if(num > INT32_MAX || num < INT32_MIN){
                        yERROR("Exceeding Integer Limits");
                    }
                    else{
                        yTRACE(CONST_INT);
                        yylval.num = atoi(yytext); return(CONST_INT);
                    }

            }
((([1-9][0-9]*)|0)"."[0-9]*)|("."[0-9]+)  {
                            double num = atof(yytext);
                            double range = abs(num);
                            if((FLT_MAX-range)>=FLT_EPSILON){
                                yTRACE(CONST_FLOAT);
                                yylval.FP = num;
                                return(CONST_FLOAT);
                            }
                            else{
                                yERROR("Exceeding Float Limits");
                            }

                         }
"true"                {yTRACE(CONST_TRUE);  yylval.num = 1;            return(CONST_TRUE);}
"false"               {yTRACE(CONST_FALSE); yylval.num = 0;            return(CONST_FALSE);}    
    
    /************************OPERATOR*************************/

"+"           { yTRACE(OP_PLUS);   return(OP_PLUS);}
"-"           { yTRACE(OP_MINUS);  return(OP_MINUS);}
"*"           { yTRACE(OP_MULT);   return(OP_MULT);}
"/"           { yTRACE(OP_DIV);    return(OP_DIV);}
"="           { yTRACE(OP_ASSIGN); return(OP_ASSIGN);}
"||"          { yTRACE(LOGIC_OR);  return(LOGIC_OR);}
"&&"          { yTRACE(LOGIC_AND); return(LOGIC_AND);}
"!"           { yTRACE(LOGIC_NOT); return(LOGIC_NOT);}
"=="          { yTRACE(OP_EQ);     return(OP_EQ);}
"^"           { yTRACE(OP_XOR);    return(OP_XOR);}    //power or xor
"!="          { yTRACE(OP_NEQ);    return(OP_NEQ);}
">="          { yTRACE(OP_GTE);    return(OP_GTE);}
"<="          { yTRACE(OP_LTE);    return(OP_LTE);}
">"           { yTRACE(OP_GT);     return(OP_GT);}
"<"           { yTRACE(OP_LT);     return(OP_LT);}
"?"           { yTRACE(OP_QST);    return(OP_QST);}

    /*************************SYMBOLS*************************/

";"           { yTRACE(SEMICOLON); return(SEMICOLON);}
","           { yTRACE(COMMA);     return(COMMA);}                                    
"{"           { yTRACE(OPEN_CURLY_BRACKET); return(OPEN_CURLY_BRACKET);}
"}"           { yTRACE(CLOSE_CURLY_BRACKET);return(CLOSE_CURLY_BRACKET);}
"("           { yTRACE(OPEN_ROUND_BRACKET); return(OPEN_ROUND_BRACKET);}
")"           { yTRACE(CLOSE_ROUND_BRACKET);return(CLOSE_ROUND_BRACKET);}
"["           { yTRACE(OPEN_SQUARE_BRACKET);return(OPEN_SQUARE_BRACKET);}
"]"           { yTRACE(CLOSE_SQUARE_BRACKET);return(CLOSE_SQUARE_BRACKET);}



    /***********************IDENTIFIER************************/

[A-Za-z_]{WORD}*    {yTRACE(IDENTIFIER); return(IDENTIFIER);}

    /*********************************************************/

[ \t]             /*eat white space*/

[\n]                  {yyline++;}

<<EOF>>               {yyterminate();}

"/*"        {
              int c1      = 0;
              int c2      = yyinput();
              int curline = yyline;
              for(;;) {
                if (c2 == EOF) {
                  fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  errorOccurred = TRUE;
                  yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                  break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                  yyline++;
              }
              yTRACE(COMMENT); 
              return(COMMENT);
            }


.           {yERROR("Unknown token"); }

%%