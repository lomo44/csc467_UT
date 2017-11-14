#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "symbol.h"
#include "parser.tab.h"
#include <stack>
typedef std::stack<cpBaseNode*> cpNodeStack;

#define IS_LOGICAL_OPERATOR(x) ((x)==AND || (x)==OR || (x)==EQ || \
                                (x)==NEQ || (x)=='<'|| (x)==LEQ|| \
                                (x)=='>' || (x)==GEQ||)

#define IS_S_Int(x)((x)==ecpTerminalType_int1)
#define IS_S_Flt(x)((x) == ecpTerminalType_float1)
#define IS_V_Int(x)(((x) >= ecpTerminalType_int2) && ((x) <= ecpTerminalType_int4))
#define IS_V_Flt(x)(((x) >= ecpTerminalType_float2) && ((x) <= ecpTerminalType_float4))
#define IS_S_A(x) (IS_S_Int(x) || IS_S_Flt(x))
#define IS_V_A(x) (IS_V_Int(x) || IS_V_Flt(x))
#define IS_S_L(x) ((x)==ecpTerminalType_bool1)
#define IS_V_L(x) (((x) >= ecpTerminalType_bool2) && ((x) <= ecpTerminalType_bool4))
#define IS_SS_A(x,y) (((x)==(y)) && (IS_S_A(x)))
#define IS_SV_A(x,y) ((IS_S_Int(x) && IS_V_Int(y))||(IS_S_Flt(x)&&IS_V_Flt(y)))
#define IS_VV_A(x,y) (((x)==(y)) && (IS_V_A(x)))
#define IS_VS_A(x,y) (((IS_V_Int(x)) && (IS_S_Int(y)))||((IS_V_Flt(x))&&(IS_S_Flt(y))))
#define IS_VV_L(x,y) (((x)==(y)) && (IS_V_L(x)))
#define IS_SS_L(x,y) (((x)==(y)) && (IS_S_L(x)))
#define IS_SV_L(x,y) ((IS_S_L(x)) && (IS_V_L(y)))

// int semantic_check( node *ast);

bool semantic_check(cpBaseNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);

int semantic_check_function_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_operator(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_condition(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_constructor_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_vector_index(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_initialization(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_assignment(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_predifined_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);

ecpTerminalType getExpressionTerminalType(cpBaseNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpressionTerminalType(cpBinaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpressionTerminalType(cpFunctionNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpressionTerminalType(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable);
ecpTerminalType getExpressionTerminalType(cpConstructorNode* in_pNode,cpSymbolTableNode in_pTable);

#endif