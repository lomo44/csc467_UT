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

bool semantic_check(cpBaseNode *in_pNode, cpSymbolTableNode *in_pSymbolTable, cpSemanticError& io_SemanticError)
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
                if (!semantic_check(normal_node->getChildNode(i), in_pSymbolTable,io_SemanticError))
                    return false;
            }
        }
        if (getExpressionTerminalType(normal_node, in_pSymbolTable,io_SemanticError) == ecpTerminalType_Invalid)
            return false;
        return true;
    }
}

void cpPrintSemanticError(cpSemanticError in_pSemanticError){
    return; //TODO: Implement
}

ecpTerminalType getExpressionTerminalType(cpBaseNode *in_pNode, cpSymbolTableNode *in_pTable, cpSemanticError& io_SemanticError)
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
            return getExpressionTerminalType((cpFunctionNode *)in_pNode, in_pTable, io_SemanticError);
        case BINARY_EXPRESSION_NODE:
            return getExpressionTerminalType((cpBinaryExpressionNode *)in_pNode, in_pTable,io_SemanticError);
        case UNARY_EXPRESION_NODE:
            return getExpressionTerminalType((cpUnaryExpressionNode *)in_pNode, in_pTable,io_SemanticError);
        case CONSTRUCTOR_NODE:
            return getExpressionTerminalType((cpConstructorNode *)in_pNode, in_pTable,io_SemanticError);
        case IDENT_NODE:
            return getExpressionTerminalType((cpConstructorNode* )in_pNode, in_pTable,io_SemanticError);
        case ASSIGNMENT_NODE:
            return getExpressionTerminalType((cpAssignmentNode*)in_pNode, in_pTable,io_SemanticError);
        case IF_STATEMENT_NODE:
            return getExpressionTerminalType((cpIfStatementNode*)in_pNode, in_pTable,io_SemanticError);
        case DECLARATION_NODE:
            return getExpressionTerminalType((cpDeclarationNode*)in_pNode, in_pTable,io_SemanticError);
        default:
            return ecpTerminalType_Unknown;
        }
    }
}

ecpTerminalType getExpressionTerminalType(cpBinaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
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
        ecpTerminalType leftNodeKind = getExpressionTerminalType(leftNode, in_pTable,io_SemanticError);
        ecpTerminalType rightNodeKind = getExpressionTerminalType(rightNode, in_pTable,io_SemanticError);
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
ecpTerminalType getExpressionTerminalType(cpFunctionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        switch (in_pNode->m_eFunctionType)
        {
        case ecpFunctionType_dp3:
        {
            // dp3
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            cpBaseNode *second_param = in_pNode->getChildNode(1);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable,io_SemanticError);
            ecpTerminalType rightNodeKind = getExpressionTerminalType(second_param, in_pTable,io_SemanticError);
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
        case ecpFunctionType_lit:
        {
            //lit
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable,io_SemanticError);
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
        case ecpFunctionType_rsq:
        {
            //rsq
            cpBaseNode *first_param = in_pNode->getChildNode(0);
            ecpTerminalType leftNodeKind = getExpressionTerminalType(first_param, in_pTable,io_SemanticError);
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

ecpTerminalType getExpressionTerminalType(cpUnaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        cpBaseNode *ChildNode = in_pNode->getChildNode(0);
        ecpTerminalType ChildNodeKind = getExpressionTerminalType(ChildNode, in_pTable,io_SemanticError);
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

ecpTerminalType getExpressionTerminalType(cpConstructorNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    ecpTerminalType currentNodeType = in_pNode->getTerminalType();
    if (currentNodeType != ecpTerminalType_Unknown)
    {
        return currentNodeType;
    }
    else
    {
        cpArgumentsNode *current_arguments_node  = (cpArgumentsNode *)in_pNode->getChildNode(0);
        
        cpNormalNode *current_argument = current_arguments_node->getCurrentArgument();
        cpArgumentsNode* next_arguments = current_arguments_node->getNextArguments();
        //Asume type node type stored in m.value
        ecpTerminalType type = in_pNode->getConstructorType();
        switch (type)
        {
        case ecpTerminalType_float4:
        case ecpTerminalType_bool4:
        case ecpTerminalType_int4:
        {
            if (next_arguments==NULL)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            else{
                ecpTerminalType exprType = getExpressionTerminalType(current_argument, in_pTable,io_SemanticError);
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    currentNodeType = ecpTerminalType_Invalid;
                    break;
                }
                else{
                    current_arguments_node = next_arguments;
                    current_argument = current_arguments_node->getCurrentArgument();
                    next_arguments = current_arguments_node->getNextArguments();
                }    
            }
        }
        case ecpTerminalType_float3:
        case ecpTerminalType_bool3:
        case ecpTerminalType_int3:
        {
            if (next_arguments==NULL)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            else{
                ecpTerminalType exprType = getExpressionTerminalType(current_argument, in_pTable,io_SemanticError);
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    currentNodeType = ecpTerminalType_Invalid;
                    break;
                }
                else{
                    current_arguments_node = next_arguments;
                    current_argument = current_arguments_node->getCurrentArgument();
                    next_arguments = current_arguments_node->getNextArguments();
                }    
            }
        }
        case ecpTerminalType_float2:
        case ecpTerminalType_bool2:
        case ecpTerminalType_int2:
        {
            if (next_arguments==NULL)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            else{
                ecpTerminalType exprType = getExpressionTerminalType(current_argument, in_pTable,io_SemanticError);
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    currentNodeType = ecpTerminalType_Invalid;
                    break;
                }
                else{
                    current_arguments_node = next_arguments;
                    current_argument = current_arguments_node->getCurrentArgument();
                    next_arguments = current_arguments_node->getNextArguments();
                }    
            }
        }
        case ecpTerminalType_float1:
        case ecpTerminalType_bool1:
        case ecpTerminalType_int1:
        {
            if (next_arguments==NULL)
            {
                currentNodeType = ecpTerminalType_Invalid;
                break;
            }
            else{
                ecpTerminalType exprType = getExpressionTerminalType(current_argument, in_pTable,io_SemanticError);
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    currentNodeType = ecpTerminalType_Invalid;
                    break;
                }
                else{
                    current_arguments_node = next_arguments;
                    current_argument = current_arguments_node->getCurrentArgument();
                    next_arguments = current_arguments_node->getNextArguments();
                }    
            }
            break;
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

ecpTerminalType getExpressionTerminalType(cpIdentifierNode* in_pNode,cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
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

ecpTerminalType getExpressionTerminalType(cpAssignmentNode* in_pNode,cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    cpIdentifierNode* variable = in_pNode->getVariable();
    cpBaseNode* expression = in_pNode->getExpression();
    
    ecpTerminalType expression_type = variable->getTerminalType();
    ecpTerminalType variable_type = expression->getTerminalType();

    if((expression_type!=ecpTerminalType_Invalid) && (variable_type!=ecpTerminalType_Invalid)){
        // Check matching
        cpSymbolAttribute* variable_attribute = lookupSymbolTable(variable->m_value, variable);
        ecpFunctionQualifier qualifer = variable_attribute->m_eQualifier;
        switch(qualifer){
            case ecpFunctionQualifier_Result:{
                // Check if the result variable is used within an if node
                cpBaseNode* current_node = in_pNode;
                in_pNode->updateTerminalType(ecpTerminalType_Unknown);
                bool isWithinIfNode=false;  
                while(current_node!=NULL){
                    if(current_node->getNodeKind() == IF_STATEMENT_NODE){
                        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
                        isWithinIfNode =true;
                        break;
                    }
                    current_node = current_node->getParentNode();
                }
                if(isWithinIfNode){
                    break;
                }
                // Keek checking assignment correctness, should not break if the it is not within an if statement
            }
            case ecpFunctionQualifier_Const:{
                if(in_pNode->getParentNode()->getNodeKind() == DECLARATION_NODE){
                    // Declaration node, allow const assign if 
                    if(expression->getNodeType()!= ecpBaseNodeType_Leaf){
                        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
                        break;
                    }
                    else{
                        if(expression->getNodeKind()==IDENT_NODE){
                            cpSymbolAttribute* expression_attribute = lookupSymbolTable(((cpIdentifierNode*)expression)->m_value, expression);
                            if(expression_attribute->m_eQualifier!=ecpFunctionQualifier_Uniform){
                                 in_pNode->updateTerminalType(ecpTerminalType_Invalid);
                                break;
                            }
                        }
                    }
                }
            } // Const should not be re-assigned.
            case ecpFunctionQualifier_None:{
                if(variable_type >= expression_type){
                    if((IS_Int(variable_type) && IS_Int(expression_type)) ||
                    (IS_Flt(variable_type) && IS_Flt(expression_type)) ||
                    (IS_Bool(variable_type) && IS_Bool(expression_type))){
                    in_pNode->updateTerminalType(ecpTerminalType_Unknown);    
                    }
                }
                else{
                    in_pNode->updateTerminalType(ecpTerminalType_Invalid);    
                }
                break;
            }
            case ecpFunctionQualifier_Attribute: // All of these variable are predefined and read-only
            case ecpFunctionQualifier_Uniform:
            
            default:{
                break;
            }
        }
        
    }
    else{
        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
    }
    return in_pNode->getTerminalType(); 
}

cpSemanticError cpCheckAssignmentNode(cpAssignmentNode* in_pNode, cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    cpIdentifierNode* variable = in_pNode->getVariable();
    cpBaseNode* expression = in_pNode->getExpression();
    ecpTerminalType variable_type = getExpressionTerminalType(variable,in_pTable, io_SemanticError);
    ecpTerminalType expression_type  = getExpressionTerminalType(expression,in_pTable, io_SemanticError);
    cpSemanticError ret;
    ret.m_eType = ecpSemanticErrorType_None;
    if((expression_type!=ecpTerminalType_Invalid) && (variable_type!=ecpTerminalType_Invalid)){
        // Check matching
        cpSymbolAttribute* variable_attribute = lookupSymbolTable(variable->m_value, variable);
        ecpFunctionQualifier qualifer = variable_attribute->m_eQualifier;
        switch(qualifer){
            case ecpFunctionQualifier_Result:{
                // Check if the result variable is used within an if node
                cpBaseNode* current_node = in_pNode;
                while(current_node!=NULL){
                    if(current_node->getNodeKind() == IF_STATEMENT_NODE){
                        //TODO: Assign line number
                        ret.m_eType = ecpSemanticErrorType_Result_In_If_Statement;
                        return ret;
                    }
                    current_node = current_node->getParentNode();
                }
                // Keek checking assignment correctness, should not break if the it is not within an if statement
                if(variable_type >= expression_type){
                    if(!((IS_Int(variable_type) && IS_Int(expression_type)) ||
                    (IS_Flt(variable_type) && IS_Flt(expression_type)) ||
                    (IS_Bool(variable_type) && IS_Bool(expression_type)))){
                        ret.m_eType = ecpSemanticErrorType_Invalid_Conversion;
                        return ret;
                    }
                }
                else{
                    ret.m_eType = ecpSemanticErrorType_Target_Invalid_Size;
                    return ret;
                }
                break;
            }
            case ecpFunctionQualifier_Const:{
                if(in_pNode->getParentNode()->getNodeKind() == DECLARATION_NODE){
                    // Declaration node, allow const assign if 
                    if(expression->getNodeType()!= ecpBaseNodeType_Leaf){
                        ret.m_eType = ecpSemanticErrorType_Invalid_Const_Assignment;
                        return ret;
                    }
                    else{
                        if(expression->getNodeKind()==IDENT_NODE){
                            cpSymbolAttribute* expression_attribute = lookupSymbolTable(((cpIdentifierNode*)expression)->m_value, expression);
                            if(expression_attribute->m_eQualifier!=ecpFunctionQualifier_Uniform){
                                ret.m_eType = ecpSemanticErrorType_Invalid_Const_Assignment;
                                return ret;
                            }
                        }
                    }
                }
            } // Const should not be re-assigned.
            case ecpFunctionQualifier_None:{
                if(variable_type >= expression_type){
                    if(!((IS_Int(variable_type) && IS_Int(expression_type)) ||
                    (IS_Flt(variable_type) && IS_Flt(expression_type)) ||
                    (IS_Bool(variable_type) && IS_Bool(expression_type)))){
                        ret.m_eType = ecpSemanticErrorType_Invalid_Conversion;
                        return ret;
                    }
                }
                else{
                    ret.m_eType = ecpSemanticErrorType_Target_Invalid_Size;
                    return ret;
                }
                break;
            }
            case ecpFunctionQualifier_Attribute: // All of these variable are predefined and read-only
            case ecpFunctionQualifier_Uniform:
            default:{
                ret.m_eType = ecpSemanticErrorType_Target_Read_Only;
                return ret;
            }
        }   
    }
    else{
        ret.m_eType = ecpSemanticErrorType_Invalid_Arguments;
    }
    return ret; 
}

ecpTerminalType getExpressionTerminalType(cpIfStatementNode* in_pNode, cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    ecpTerminalType expression_type = getExpressionTerminalType(in_pNode->getExpression(),in_pTable,io_SemanticError);
    if(expression_type!=ecpTerminalType_bool1){
        in_pNode->updateTerminalType(ecpTerminalType_Invalid);
    }
    else{
        in_pNode->updateTerminalType(ecpTerminalType_Unknown);
    }
    return in_pNode->getTerminalType();
}

ecpTerminalType getExpressionTerminalType(cpDeclarationNode* in_pNode, cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    //check if there are duplicate declarations in current scope
    cpAssignmentNode* assignment_node = in_pNode->getAssignmentNode();
    if(lookupSymbolTable(((cpDeclarationNode*)in_pNode)->m_sIdentifierName,in_pNode)!=NULL){
        return ecpTerminalType_Invalid;
    }
    else{
        // Check
        return getExpressionTerminalType(assignment_node,in_pTable,io_SemanticError);
    } 
}
