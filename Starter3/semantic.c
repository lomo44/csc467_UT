#include "semantic.h"

int semantic_check( node *ast) {
  return 0; // failed checks
}


//questions: why do we have int_Node,Float_node instead of literal node?
//           why unknown?should be currentNodeType
//           type node type stored in m.value
//           we need to initialize 'const' child node for declaration
//           why dont we have argument node?If reduced expressions into one level?we should have more than two childs for construction node(added argument node at enum for now)
//           should change assignment node child num to 4 (assignment under statement and assigment under declaration)

bool semantic_check(cpBaseNode* in_pNode, cpSymbolTableNode* in_pSymbolTable){

 int i;
 if (in_pNode == NULL) return true;
 /* visit each child */
 for (i=0; i < in_pNode->getNumOfChildNodes(); i++){
      if(semantic_check(in_pNode->getChildNode(i),in_pSymbolTable)==false)
      return false;
    }
 /* do work at parent */
    if (getExpresstionTerminalType(in_pNode, *in_pSymbolTable)==ecpTerminalType_Invalid)
    return false;

    
 
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
            case CONSTRUCTOR_NODE: return getExpresstionTerminalType((cpConstructorNode*)in_pNode, in_pTable);
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
        ecpTerminalType rightNodeKind = getExpresstionTerminalType(rightNode, in_pTable);
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
    return currentNodeType;
}
ecpTerminalType getExpresstionTerminalType(cpFunctionNode* in_pNode, cpSymbolTableNode in_pTable){
    ecpTerminalType currentNodeType = in_pNode->getTerminalType(); 
    if(currentNodeType!=ecpTerminalType_Unknown){
        return currentNodeType;
    }
    else{
        int operand = in_pNode->getOperand();
        switch(operand){
            case 0:{
                // dp3
                cpBaseNode* first_param = in_pNode->getChildNode(0);
                cpBaseNode* second_param = in_pNode->getChildNode(1);
                ecpTerminalType leftNodeKind = getExpresstionTerminalType(first_param, in_pTable);
                ecpTerminalType rightNodeKind = getExpresstionTerminalType(second_param, in_pTable);
                if(IS_VV_A(leftNodeKind, rightNodeKind)){
                    if(leftNodeKind == ecpTerminalType_float3 || leftNodeKind == ecpTerminalType_float4){
                        currentNodeType = ecpTerminalType_float1;
                    }
                    else if(leftNodeKind == ecpTerminalType_int3 || leftNodeKind == ecpTerminalType_int4){
                        currentNodeType = ecpTerminalType_int1;
                    }
                    else{
                        currentNodeType = ecpTerminalType_Invalid;
                    }
                }
                else{
                    currentNodeType = ecpTerminalType_Invalid;
                }
                break;
            }
            case 1:{
                //lit
                cpBaseNode* first_param = in_pNode->getChildNode(0);
                ecpTerminalType leftNodeKind = getExpresstionTerminalType(first_param, in_pTable);
                if(leftNodeKind == ecpTerminalType_float4){
                    currentNodeType = ecpTerminalType_float4;
                }
                else{
                    currentNodeType = ecpTerminalType_Invalid;
                }
                break;
            }
            case 2:{
                //rsq
                cpBaseNode* first_param = in_pNode->getChildNode(0);
                ecpTerminalType leftNodeKind = getExpresstionTerminalType(first_param, in_pTable);
                if(leftNodeKind == ecpTerminalType_float1 || leftNodeKind == ecpTerminalType_int1){
                    currentNodeType = ecpTerminalType_float1;
                }
                else{
                    currentNodeType = ecpTerminalType_Invalid;
                }
                break;
            }
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return ecpTerminalType_Unknown;//why unknown?should be currentNodeType
}

ecpTerminalType getExpresstionTerminalType(cpUnaryExpressionNode* in_pNode, cpSymbolTableNode in_pTable){
     ecpTerminalType currentNodeType = in_pNode->getTerminalType(); 
    if(currentNodeType!=ecpTerminalType_Unknown){
        return currentNodeType;
    }
    else{
        cpBaseNode* ChildNode = in_pNode->getChildNode(0);
        ecpTerminalType ChildNodeKind = getExpresstionTerminalType(ChildNode, in_pTable);
        int operand = in_pNode->getOperand();
        switch(operand){
          case '-':{
                (IS_S_A(ChildNodeKind))?currentNodeType=ChildNodeKind:currentNodeType=ecpTerminalType_Invalid;
                (IS_V_A(ChildNodeKind))?currentNodeType=ChildNodeKind:currentNodeType=ecpTerminalType_Invalid;
                break;
            }
          case '!':{
                (IS_S_L(ChildNodeKind))?currentNodeType=ChildNodeKind:currentNodeType=ecpTerminalType_Invalid;
          } 
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return currentNodeType;
}

ecpTerminalType getExpresstionTerminalType(cpConstructorNode* in_pNode,cpSymbolTableNode in_pTable){
     ecpTerminalType currentNodeType = in_pNode->getTerminalType(); 
    if(currentNodeType!=ecpTerminalType_Unknown){
        return currentNodeType;
    }
    else{
        cpBaseNode* leftNode = in_pNode->getChildNode(0);
        cpBaseNode* rightNode = in_pNode->getChildNode(1);
        //Asume type node type stored in m.value
        ecpTeriminalType type = leftNode->m_value;
        switch(type){
            case ecpTerminalType_float4: 
            case ecpTerminalType_bool4: 
            case ecpTerminalType_int4:{
                if ((cpNormalNode*)rightNode->getNumOfChildNodes()==1) {
                    currentNodeType=ecpTerminalType_Invalid
                    break;
                }
                cpBaseNode* expr = rightNode->getChildNode(1);
                ecpTerminal exprType=getExpressionTerminalType(expr,in_pTable);
                if (!(IS_SV_L(exprType,type)||IS_SV_A(exprType,type))){
                    currentNodeType=ecpTerminalType_Invalid;
                    break;
                }
                rightNode= rightNode->getChildNode(0);
            }
            case ecpTerminalType_float3: 
            case ecpTerminalType_bool3: 
            case ecpTerminalType_int3:{
                if ((cpNormalNode*)rightNode->getNumOfChildNodes()==1) {
                    currentNodeType=ecpTerminalType_Invalid
                    break;
                }
                cpBaseNode* expr = rightNode->getChildNode(1);
                ecpTerminal exprType=getExpressionTerminalType(expr,in_pTable);
                if (!(IS_SV_L(exprType,type)||IS_SV_A(exprType,type))){
                    currentNodeType=ecpTerminalType_Invalid;
                    break;
                }
                rightNode= rightNode->getChildNode(0);
            }
            case ecpTerminalType_float2: 
            case ecpTerminalType_bool2: 
            case ecpTerminalType_int2:{
                if ((cpNormalNode*)rightNode->getNumOfChildNodes()==1) {
                    currentNodeType=ecpTerminalType_Invalid
                    break;
                }
                cpBaseNode* expr = rightNode->getChildNode(1);
                ecpTerminal exprType=getExpressionTerminalType(expr,in_pTable);
                if (!(IS_SV_L(exprType,type)||IS_SV_A(exprType,type))){
                    currentNodeType=ecpTerminalType_Invalid;
                    break;
                }
                rightNode= rightNode->getChildNode(0);
            }
            case ecpTerminalType_float1: 
            case ecpTerminalType_bool1: 
            case ecpTerminalType_int1:{
                if (rightNode->getNodeKind()==ARG_NODE) {
                    currentNodeType=ecpTerminalType_Invalid;
                    break;
                }
                ecpTerminal exprType=getExpressionTerminalType(rightNode,in_pTable);
                if (!(IS_SV_L(exprType,type)||IS_SV_A(exprType,type))){
                    currentNodeType=ecpTerminalType_Invalid;
                    break;
                }
                currentNodeType=type;
            }
            default:{
                currentNodeType = ecpTerminalType_Invalid;
            }
            in_pNode->updateTerminalType(currentNodeType);
            return currentNodeType;
        }
    }
}