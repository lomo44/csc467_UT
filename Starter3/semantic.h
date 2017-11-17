#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "symbol.h"
#include <stack>

#define IS_S_Int(x)((x)==ecpTerminalType_int1)
#define IS_S_Flt(x)((x) == ecpTerminalType_float1)

#define IS_Int(x)(((x) >= ecpTerminalType_int1) && ((x) <= ecpTerminalType_int4))
#define IS_Flt(x)(((x) >= ecpTerminalType_float1) && ((x) <= ecpTerminalType_float4))
#define IS_Bool(x)(((x) >= ecpTerminalType_bool1) && ((x) <= ecpTerminalType_bool4))

#define IS_V_Int(x)(((x) >= ecpTerminalType_int2) && ((x) <= ecpTerminalType_int4))
#define IS_V_Flt(x)(((x) >= ecpTerminalType_float2) && ((x) <= ecpTerminalType_float4))
#define IS_V_Bool(x)(((x) >= ecpTerminalType_bool2) && ((x) <= ecpTerminalType_bool4))
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

enum ecpSemanticErrorType{
    ecpSemanticErrorType_None,
    ecpSemanticErrorType_Invalid_Const_Assignment,
    ecpSemanticErrorType_Invalid_Conversion,
    ecpSemanticErrorType_Target_Invalid_Size,
    ecpSemanticErrorType_Invalid_Arguments,
    ecpSemanticErrorType_Duplicate_Declaration,
    ecpSemanticErrorType_Result_In_If_Statement,
    ecpSemanticErrorType_Target_Read_Only
};

class cpSemanticError{
public:
    cpSemanticError():m_eType(ecpSemanticErrorType_None),m_iRowNumber(-1),m_iColNumber(-1){}
    bool hasError(){return m_eType != ecpSemanticErrorType_None;}
    ecpSemanticErrorType m_eType;
    int m_iRowNumber;
    int m_iColNumber;
};

void cpPrintSemanticError(cpNormalNode* in_pNode,const cpSemanticError& in_pSemanticError);

bool semantic_check(cpBaseNode* in_pNode, cpSymbolTableNode* in_pSymbolTable,cpSemanticError& io_SemanticError);

void cpCheckNode(cpBaseNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpBinaryExpressionNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpFunctionNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpConstructorNode* in_pNode,cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);

void cpCheckNode(cpIdentifierNode* in_pNode,cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpAssignmentNode* in_pNode,cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError); 
void cpCheckNode(cpIfStatementNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
void cpCheckNode(cpDeclarationNode* in_pNode, cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError);
#endif