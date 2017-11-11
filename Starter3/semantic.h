#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include <stack>

#include "ast.h"
#include "symbol.h"
#include "parser.tab.h"

typedef std::stack<cpBaseNode*> cpNodeStack;

#define IS_LOGICAL_OPERATOR(x) ((x)==AND || (x)==OR || (x)==EQ || \
                                (x)==NEQ || (x)=='<'|| (x)==LEQ|| \
                                (x)=='>' || (x)==GEQ||)

#define IS_S_A(x) ((x)==FLOAT_T || (x) == INT_T)
#define IS_V_A(x) ((x)==VEC_T || (x) == IVEC_T)
#define IS_S_L(x) ((x)==BOOL_T)
#define IS_V_L(x) ((x)==BVEC_T)
#define IS_SS_A(x,y) (((x)==FLOAT_T && (y)==FLOAT_T)||((x)==INT_T && (y)==INT_T))
#define IS_SV_A(x,y) (((x)==FLOAT_T && (y)==VEC_T)||((x)==INT_T && (y)==IVEC_T))
#define IS_VV_A(x,y) (((x)==VEC_T && (y)==VEC_T)||((x)==IVEC_T && (y)==IVEC_T))
#define IS_VS_A(x,y) (((x)==VEC_T && (y)==FLOAT_T)||((x)==IVEC_T && (y)==INT_T))
#define IS_VV_L(x,y) (((x)==BVEC_T && (y)==BVEC_T)||((x)==BVEC_T && (y)==BVEC_T))
#define IS_SS_L(x,y) (((x)==BOOL_T && (y)==BOOL_T)||((x)==BOOL_T && (y)==BOOL_T))

int semantic_check( node *ast);

int semantic_check(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);

int semantic_check_function_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_operator(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_condition(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_constructor_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_vector_index(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_initialization(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_assignment(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_predifined_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);

int getExpresstionTerminalType(cpBaseNode* in_pNode, cpSymbolTableNode in_pTable);
int getExpresstionTerminalType(cpBinaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);
int getExpresstionTerminalType(cpFunctionNode* in_pNode, cpSymbolTableNode in_pTable);
int getExpresstionTerminalType(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);

#endif