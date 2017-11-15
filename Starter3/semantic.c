#include "semantic.h"
#include "parser.tab.h"
// int semantic_check( node *ast) {
//   return 0; // failed checks
// }

//questions: why do we have int_Node,Float_node instead of literal node?
//           why unknown?should be currentNodeType
//           type node type stored in m.value
//           we need to initialize 'const' child node for declaration
//           why dont we have argument node?If reduced expressions into one level?we should have more than two childs for construction node(added argument node at enum for now)
//           should change assignment node child num to 4 (assignment under statement and assigment under declaration)

bool semantic_check(cpBaseNode *in_pNode, cpSymbolTableNode *in_pSymbolTable)
{
    int i;
    if (in_pNode == NULL || in_pNode->getNodeType() == ecpBaseNodeType_Leaf)
    {
        return true;
    }
    else
    {
        cpNormalNode *normal_node = (cpNormalNode *)in_pNode;
        for (i = 0; i < normal_node->getNumOfChildNodes(); i++)
        {
            if(normal_node->getChildNode(i)!=NULL){
                if (!semantic_check(normal_node->getChildNode(i), in_pSymbolTable))
                    return false;
            }
        }
        if (getExpressionTerminalType(normal_node, in_pSymbolTable) == ecpTerminalType_Invalid)
            return false;
        return true;
    }
}

ecpTerminalType getExpressionTerminalType(cpBaseNode *in_pNode, cpSymbolTableNode *in_pTable)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        switch (in_pNode->getNodeKind())
        {
        case FUNCTION_NODE:
            return getExpressionTerminalType((cpFunctionNode *)in_pNode, in_pTable);
        case BINARY_EXPRESSION_NODE:
            return getExpressionTerminalType((cpBinaryExpressionNode *)in_pNode, in_pTable);
        case UNARY_EXPRESION_NODE:
            return getExpressionTerminalType((cpUnaryExpressionNode *)in_pNode, in_pTable);
        case CONSTRUCTOR_NODE:
            return getExpressionTerminalType((cpConstructorNode *)in_pNode, in_pTable);
        default:
            return ecpTerminalType_Unknown;
        }
    }
}
ecpTerminalType getExpressionTerminalType(cpBinaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        cpBaseNode *leftNode = in_pNode->getChildNode(0);
        cpBaseNode *rightNode = in_pNode->getChildNode(1);
        ecpTerminalType leftNodeKind = getExpressionTerminalType(leftNode, in_pTable);
        ecpTerminalType rightNodeKind = getExpressionTerminalType(rightNode, in_pTable);
        int operand = in_pNode->getOperand();
        switch (operand)
        {
        case '<':
        case LEQ:
        case '>':
        case GEQ:
        {
            (IS_SS_A(leftNodeKind, rightNodeKind)) ? currentNodeType = ecpTerminalType_bool1 : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case EQ:
        case NEQ:
        {
            (IS_SS_A(leftNodeKind, rightNodeKind) || IS_VV_A(leftNodeKind, rightNodeKind) || IS_SS_L(leftNodeKind, rightNodeKind) || IS_VV_L(leftNodeKind, rightNodeKind)) ? currentNodeType = ecpTerminalType_bool1 : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case AND:
        case OR:
        {
            (IS_SS_L(leftNodeKind, rightNodeKind) || IS_VV_L(leftNodeKind, rightNodeKind)) ? currentNodeType = ecpTerminalType_bool1 : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case '/':
        case '^':
        {
            (IS_SS_A(leftNodeKind, rightNodeKind)) ? currentNodeType = leftNodeKind : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case '*':
        {
            (IS_SS_A(leftNodeKind, rightNodeKind)) ? currentNodeType = leftNodeKind : currentNodeType = ecpTerminalType_Invalid;
            (IS_VV_A(leftNodeKind, rightNodeKind)) ? currentNodeType = leftNodeKind : currentNodeType = ecpTerminalType_Invalid;
            (IS_VS_A(leftNodeKind, rightNodeKind)) ? currentNodeType = leftNodeKind : currentNodeType = ecpTerminalType_Invalid;
            (IS_SV_A(leftNodeKind, rightNodeKind)) ? currentNodeType = rightNodeKind : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case '+':
        case '-':
        {
            (IS_SS_A(leftNodeKind, rightNodeKind) || IS_VV_A(leftNodeKind, rightNodeKind)) ? currentNodeType = rightNodeKind : currentNodeType = ecpTerminalType_Invalid;
        }
        default:
        {
            currentNodeType = ecpTerminalType_Invalid;
        }
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return currentNodeType;
}
ecpTerminalType getExpressionTerminalType(cpFunctionNode *in_pNode, cpSymbolTableNode *in_pTable)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        int operand = in_pNode->getOperand();
        switch (operand)
        {
        case 0:
        {
            // dp3
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            cpBaseNode *second_param = in_pNode->getChildNode(1);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable);
            ecpTerminalType rightNodeKind = getExpressionTerminalType(second_param, in_pTable);
            if (IS_VV_A(leftNodeKind, rightNodeKind))
            {
                if (leftNodeKind == ecpTerminalType_float3 || leftNodeKind == ecpTerminalType_float4)
                {
                    currentNodeType = ecpTerminalType_float1;
                }
                else if (leftNodeKind == ecpTerminalType_int3 || leftNodeKind == ecpTerminalType_int4)
                {
                    currentNodeType = ecpTerminalType_int1;
                }
                else
                {
                    currentNodeType = ecpTerminalType_Invalid;
                }
            }
            else
            {
                currentNodeType = ecpTerminalType_Invalid;
            }
            break;
        }
        case 1:
        {
            //lit
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable);
            if (leftNodeKind == ecpTerminalType_float4)
            {
                currentNodeType = ecpTerminalType_float4;
            }
            else
            {
                currentNodeType = ecpTerminalType_Invalid;
            }
            break;
        }
        case 2:
        {
            //rsq
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable);
            if (leftNodeKind == ecpTerminalType_float1 || leftNodeKind == ecpTerminalType_int1)
            {
                currentNodeType = ecpTerminalType_float1;
            }
            else
            {
                currentNodeType = ecpTerminalType_Invalid;
            }
            break;
        }
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return currentNodeType;
}

ecpTerminalType getExpressionTerminalType(cpUnaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        cpBaseNode *ChildNode = in_pNode->getChildNode(0);
        ecpTerminalType ChildNodeKind = getExpressionTerminalType(ChildNode, in_pTable);
        int operand = in_pNode->getOperand();
        switch (operand)
        {
        case '-':
        {
            (IS_S_A(ChildNodeKind)) ? currentNodeType = ChildNodeKind : currentNodeType = ecpTerminalType_Invalid;
            (IS_V_A(ChildNodeKind)) ? currentNodeType = ChildNodeKind : currentNodeType = ecpTerminalType_Invalid;
            break;
        }
        case '!':
        {
            (IS_S_L(ChildNodeKind)) ? currentNodeType = ChildNodeKind : currentNodeType = ecpTerminalType_Invalid;
        }
        }
    }
    in_pNode->updateTerminalType(currentNodeType);
    return currentNodeType;
}

//TODO: Added arguments node, need to modify
ecpTerminalType getExpressionTerminalType(cpConstructorNode *in_pNode, cpSymbolTableNode *in_pTable)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        cpNormalNode *leftNode = (cpNormalNode *)in_pNode->getChildNode(0);
        cpNormalNode *rightNode = (cpNormalNode *)in_pNode->getChildNode(1);
        //Asume type node type stored in m.value
        ecpTerminalType type = getExpressionTerminalType(leftNode, in_pTable);
        switch (type)
        {
        case ecpTerminalType_float4:
        case ecpTerminalType_bool4:
        case ecpTerminalType_int4:
        {
            if (rightNode->getNumOfChildNodes() == 1)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            cpNormalNode *expr = (cpNormalNode *)rightNode->getChildNode(1);
            ecpTerminalType exprType = getExpressionTerminalType(expr, in_pTable);
            if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            rightNode = (cpNormalNode *)rightNode->getChildNode(0);
        }
        case ecpTerminalType_float3:
        case ecpTerminalType_bool3:
        case ecpTerminalType_int3:
        {
            if (rightNode->getNumOfChildNodes() == 1)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            cpNormalNode *expr = (cpNormalNode *)(rightNode->getChildNode(1));
            ecpTerminalType exprType = getExpressionTerminalType(expr, in_pTable);
            if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            rightNode = (cpNormalNode *)rightNode->getChildNode(0);
        }
        case ecpTerminalType_float2:
        case ecpTerminalType_bool2:
        case ecpTerminalType_int2:
        {
            if (rightNode->getNumOfChildNodes() == 1)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            cpNormalNode *expr = (cpNormalNode *)(rightNode->getChildNode(1));
            ecpTerminalType exprType = getExpressionTerminalType(expr, in_pTable);
            if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            rightNode = (cpNormalNode *)rightNode->getChildNode(0);
        }
        case ecpTerminalType_float1:
        case ecpTerminalType_bool1:
        case ecpTerminalType_int1:
        {
            if (rightNode->getNodeKind() == ARG_NODE)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            ecpTerminalType exprType = getExpressionTerminalType(rightNode, in_pTable);
            if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            currentNodeType = type;
        }
        default:
        {
            currentNodeType = ecpTerminalType_Invalid;
        }
        }
        in_pNode->updateTerminalType(currentNodeType);
        return currentNodeType;
    }
}

ecpTerminalType getExpressionTerminalType(cpIdentifierNode* in_pNode,cpSymbolTableNode* in_pTable){
    // Based on the type of the in_pNode, check different things
    ecpTerminalType type = in_pNode->getTerminalType();
    int access_index = in_pNode->getAccessIndex();
    if(access_index>=0 && access_index <4){
        if(!(IS_Int(type) && access_index <= (type-ecpTerminalType_int1)) ||
        !(IS_Flt(type) && access_index <= (type-ecpTerminalType_float1)) ||
        !(IS_Bool(type) && access_index <= (type-ecpTerminalType_bool1))){
            in_pNode->updateTerminalType(ecpTerminalType_Unknown);    
        }
        else{
            in_pNode->updateTerminalType(ecpTerminalType_Invalid);
        }
    }
    else{
        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
    }
    return in_pNode->getTerminalType();
}
ecpTerminalType getExpressionTerminalType(cpAssignmentNode* in_pNode,cpSymbolTableNode* in_pTable){
    cpIdentifierNode* variable = in_pNode->getVariable();
    cpNormalNode* expression = in_pNode->getExpression();
    
    ecpTerminalType expression_type = variable->getTerminalType();
    ecpTerminalType variable_type = expression->getTerminalType();

    if((expression_type!=ecpTerminalType_Invalid) && (variable_type!=ecpTerminalType_Invalid)){
        // Check matching      
        if(variable_type >= expression_type){
            if((IS_Int(variable_type) && IS_Int(variable_type)) ||
            (IS_Flt(variable_type) && IS_Flt(variable_type)) ||
            (IS_Bool(variable_type) && IS_Bool(variable_type))){
                in_pNode->updateTerminalType(ecpTerminalType_Unknown);    
            }
            else{
                in_pNode->updateTerminalType(ecpTerminalType_Invalid);    
            }
        }
        else{
            in_pNode->updateTerminalType(ecpTerminalType_Invalid);    
        }
    }
    else{
        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
    }
    return in_pNode->getTerminalType(); 
}
ecpTerminalType getExpressionTerminalType(cpIfStatementNode* in_pNode, cpSymbolTableNode* in_pTable){
    ecpTerminalType expression_type = getExpressionTerminalType(in_pNode->getExpression(),in_pTable);
    if(expression_type!=ecpTerminalType_bool1){
        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
    }
    else{
        in_pNode->updateTerminalType(ecpTerminalType_Unknown);
    }
    return in_pNode->getTerminalType();
}