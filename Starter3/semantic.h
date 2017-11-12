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

#define IS_S_A(x) ((x)==ecpTerminalType_int1 || (x) == ecpTerminalType_float1)
#define IS_V_A(x) (((x) >= ecpTerminalType_int2 && (x) <= ecpTerminalType_int4) || ((x) >= ecpTerminalType_float2 && (x) <= ecpTerminalType_float4))
#define IS_S_L(x) ((x)==ecpTerminalType_bool1)
#define IS_V_L(x) ((x) >= ecpTerminalType_bool2 && (x) <= ecpTerminalType_bool4)
#define IS_SS_A(x,y) (((x)==(y)) && IS_S_A(x))
#define IS_SV_A(x,y) (IS_S_A(x) && IS_V_A(y))
#define IS_VV_A(x,y) (((x)==(y)) && IS_V_A(x))
#define IS_VS_A(x,y) (IS_V_A(x) && IS_S_A(y))
#define IS_VV_L(x,y) (((x)==(y)) && IS_V_L(x))
#define IS_SS_L(x,y) (((x)==(y)) && IS_S_L(x))

int semantic_check(node *ast);

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

ecpTerminalType getExpresstionTerminalType(cpBaseNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpresstionTerminalType(cpBinaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpresstionTerminalType(cpFunctionNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpresstionTerminalType(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);

#endif