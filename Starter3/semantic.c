
#include "semantic.h"

int semantic_check( node *ast) {
  return 0; // failed checks
}



int semantic_check(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
  return 0;
}

int semantic_check_function_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_operator(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_condition(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_constructor_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_vector_index(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_initialization(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_assignment(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}
int semantic_check_predifined_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){
    return 0;
}

ecpTerminalType getExpresstionTerminalType(cpBaseNode* in_pNode, cpSymbolTableNode in_pTable){
    ecpTerminalType currentNodeType = in_pNode->getTerminalType(); 
    if(currentNodeType!=ecpTerminalType_Unknown){
        return currentNodeType;
    }
    else{
        switch(in_pNode->getNodeKind()){
            case FUNCTION_NODE: return getExpresstionTerminalType((cpFunctionNode*)in_pNode, in_pTable);
            case BINARY_EXPRESSION_NODE: return getExpresstionTerminalType((cpBinaryExpressionNode*)in_pNode, in_pTable);
            case UNARY_EXPRESION_NODE: return getExpresstionTerminalType((cpUnaryExpressionNode*)in_pNode, in_pTable);
            default:return ecpTerminalType_Unknown;
        }
    }
}
ecpTerminalType getExpresstionTerminalType(cpBinaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable){
    ecpTerminalType currentNodeType = in_pNode->getTerminalType(); 
    if(currentNodeType!=ecpTerminalType_Unknown){
        return currentNodeType;
    }
    else{
        cpBaseNode* leftNode = in_pNode->getChildNode(0);
        cpBaseNode* rightNode = in_pNode->getChildNode(1);
        ecpTerminalType leftNodeKind = getExpresstionTerminalType(leftNode, in_pTable);
        ecpTerminalType rightNodeKind = getExpresstionTerminalType(leftNode, in_pTable);
        leftNode->updateTerminalType(leftNodeKind);
        rightNode->updateTerminalType(rightNodeKind);
        int operand = in_pNode->getOperand();
        switch(operand){
            case '<':
            case LEQ:
            case '>':
            case GEQ:{
                (IS_SS_A(leftNodeKind,rightNodeKind))?currentNodeType = ecpTerminalType_bool1:currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            case EQ:
            case NEQ:{
                (IS_SS_A(leftNodeKind,rightNodeKind)||IS_VV_A(leftNodeKind,rightNodeKind)
                ||IS_SS_L(leftNodeKind,rightNodeKind)||IS_VV_L(leftNodeKind,rightNodeKind))?currentNodeType = ecpTerminalType_bool1:currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            case AND:
            case OR:{
                (IS_SS_L(leftNodeKind,rightNodeKind)||IS_VV_L(leftNodeKind,rightNodeKind))?currentNodeType = ecpTerminalType_bool1:currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            case '/':
            case '^':{
                (IS_SS_A(leftNodeKind,rightNodeKind))?currentNodeType=leftNodeKind:currentNodeType=ecpTerminalType_Invalid;
                break;
            }
            case '*':{
                (IS_SS_A(leftNodeKind,rightNodeKind))?currentNodeType=leftNodeKind:currentNodeType=ecpTerminalType_Invalid;
                (IS_VV_A(leftNodeKind,rightNodeKind))?currentNodeType=leftNodeKind:currentNodeType=ecpTerminalType_Invalid;
                (IS_VS_A(leftNodeKind,rightNodeKind))?currentNodeType=leftNodeKind:currentNodeType=ecpTerminalType_Invalid;
                (IS_SV_A(leftNodeKind,rightNodeKind))?currentNodeType=rightNodeKind:currentNodeType=ecpTerminalType_Invalid;
                break;
            }
            case '+':
            case '-':{
                (IS_SS_A(leftNodeKind,rightNodeKind)||IS_VV_A(leftNodeKind,rightNodeKind))?currentNodeType=rightNodeKind:currentNodeType=ecpTerminalType_Invalid;
            }
            default:{
                currentNodeType = ecpTerminalType_Invalid;
            }
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return ecpTerminalType_Unknown;
}
ecpTerminalType getExpresstionTerminalType(cpFunctionNode* in_pNode, cpSymbolTableNode in_pTable){
    return ecpTerminalType_Unknown;
}
ecpTerminalType getExpresstionTerminalType(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable){
    return ecpTerminalType_Unknown;
}