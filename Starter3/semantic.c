#include "semantic.h"
#include "parser.tab.h"
#include "common.h"
#include <vector>
//questions: why do we have int_Node,Float_node instead of literal node?
//           why unknown?should be currentNodeType
//           type node type stored in m.value
//           we need to initialize 'const' child node for declaration
//           why dont we have argument node?If reduced expressions into one level?we should have more than two childs for construction node(added argument node at enum for now)
//           should change assignment node child num to 4 (assignment under statement and assigment under declaration)

std::string gErrorMessages[ecpSemanticErrorType_ErrorCount] = {
    "", //No error
    "invalid constant assignment fron non-uniform variable",
    "invalid conversion",
    "invalid type",
    "invalid variable",
    "invalid variable: variable is read before write",
    "invalid vecter index",
    "invalid arguments",
    "invalid expression",
    "invalid assignment",
    "invalid used of arithmatic expression in logical operation",
    "invalid comparison between different type",
    "invalid target size",
    "invalid arguments: not enough arguments",
    "invalid arguments: too much arguments",
    "invalid declaration. Identifier has been declared already",
    "invalid use of result in If statement",
    "invalid access, target is read-only",
    "invalid reassignment for constant identifier",
    "invalid expression: use of logical operand in arithmatic operation",
    "invalid expression: use of arithmatic operand in logical operation",
    "invalid type: unsupported operand type"
};

void cpSemanticError::cleanError(){
    m_pTargetNode = NULL;
    m_eType = ecpSemanticErrorType_None;
    m_iRowNumber = -1;
    m_iColNumber = -1;
}

void cpSemanticError::print(){
    //m_pTargetNode->print();
    fprintf(errorFile,"%d:%d: error: %s in %s \n",m_iRowNumber,m_iColNumber,gErrorMessages[this->m_eType].c_str(),this->m_pTargetNode->toString().c_str());
}

void cpSemanticError::setError(ecpSemanticErrorType in_eType, cpBaseNode* in_pNode){
    m_eType = in_eType;
    m_pTargetNode = in_pNode;
    in_pNode->getLineAndCol(&m_iRowNumber,&m_iColNumber);
}


bool semantic_check(cpBaseNode *in_pNode, cpSymbolTableNode *in_pSymbolTable, cpSemanticError& io_SemanticError)
{
    int i;
    bool ret = true;
    if (in_pNode == NULL || (in_pNode->getNodeType() == ecpBaseNodeType_Leaf && in_pNode->getNodeKind()!=IDENT_NODE))
    {
        ret = true;
    }
    else
    {
        if(in_pNode->getNodeKind()!=IDENT_NODE){
            cpNormalNode *normal_node = (cpNormalNode *)in_pNode;
            for (i = 0; i < normal_node->getNumOfChildNodes(); i++)
            {
                if(!semantic_check(normal_node->getChildNode(i), in_pSymbolTable,io_SemanticError)){
                    ret = false;
                }
                else{
                    io_SemanticError.cleanError();
                }
            }
        }
        cpCheckNode(in_pNode, in_pSymbolTable,io_SemanticError);
        if(io_SemanticError.hasError()){
            io_SemanticError.print();
            ret = false;
        }
        io_SemanticError.cleanError();
    }
    return ret;
}

void cpCheckNode(cpBaseNode *in_pNode, cpSymbolTableNode *in_pTable, cpSemanticError& io_SemanticError)
{
    switch (in_pNode->getNodeKind())
    {
    case FUNCTION_NODE:{
        cpCheckNode((cpFunctionNode *)in_pNode, in_pTable, io_SemanticError);
        break;
    }
    case BINARY_EXPRESSION_NODE:{
        cpCheckNode((cpBinaryExpressionNode *)in_pNode, in_pTable,io_SemanticError);
        break;
    }
    case UNARY_EXPRESION_NODE:
    {
        cpCheckNode((cpUnaryExpressionNode *)in_pNode, in_pTable,io_SemanticError);
        return;
    }
    case CONSTRUCTOR_NODE:
    {
        cpCheckNode((cpConstructorNode *)in_pNode, in_pTable,io_SemanticError);
        return;
    }
    case IDENT_NODE:
    {
        cpCheckNode((cpIdentifierNode* )in_pNode, in_pTable,io_SemanticError);
        return;
    }
    case ASSIGNMENT_NODE:
    {
        cpCheckNode((cpAssignmentNode*)in_pNode, in_pTable,io_SemanticError);
        return;
    }
    case IF_STATEMENT_NODE:
    {
        cpCheckNode((cpIfStatementNode*)in_pNode, in_pTable,io_SemanticError);
        return;
    }
    case DECLARATION_NODE:
    {
        cpCheckNode((cpDeclarationNode*)in_pNode, in_pTable,io_SemanticError);
        return;
    }
    default:
    {
        return;
    }
    }
}

void cpCheckNode(cpBinaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    cpBaseNode *leftNode = in_pNode->getChildNode(0);
    cpBaseNode *rightNode = in_pNode->getChildNode(1);
    ecpTerminalType leftNodeKind = leftNode->getTerminalType();
    ecpTerminalType rightNodeKind = rightNode->getTerminalType();
    switch (in_pNode->getOperand())
    {
        case ecpOperand_B_LT:
        case ecpOperand_B_LEQ:
        case ecpOperand_B_GT:
        case ecpOperand_B_GEQ:
        {
            
            if(IS_Any(leftNodeKind) || IS_Any(rightNodeKind)){
                // Either left of right node is any type
                if(IS_Any(leftNodeKind) && !IS_Any(rightNodeKind) && !IS_S_A(rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A,in_pNode);
                }
                else if(!IS_Any(leftNodeKind) && IS_Any(rightNodeKind) && !IS_S_A(leftNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A,in_pNode);
                }
                else{
                    io_SemanticError.cleanError();   
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(leftNodeKind != rightNodeKind){
                    io_SemanticError.setError(ecpSemanticErrorType_Comparision_Between_Different_Type, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
                else{   
                    if(!IS_S_A(leftNodeKind)){
                        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A,in_pNode);
                        in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    }
                    else{
                        io_SemanticError.cleanError();
                        in_pNode->setTerminalType(ecpTerminalType_bool1);                        
                    }
                }
            }
            break;
        }
        case ecpOperand_B_EQ:
        case ecpOperand_B_NEQ:
        {
            if(IS_Any(leftNodeKind) || IS_Any(rightNodeKind)){
                io_SemanticError.cleanError();
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                // Left and right are both known type, need to check if the type are equal
                if(leftNodeKind == rightNodeKind){
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(ecpTerminalType_bool1);
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Comparision_Between_Different_Type, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
            }
            break;
        }
        case ecpOperand_B_AND:
        case ecpOperand_B_OR:
        {
            if(IS_Any(leftNodeKind) || IS_Any(rightNodeKind)){
                if(!IS_Any(leftNodeKind) && IS_Any(rightNodeKind) && !IS_Bool(leftNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expreesion_A_In_L, in_pNode);                 
                }
                else if(IS_Any(leftNodeKind) && !IS_Any(rightNodeKind) && !IS_Bool(rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expreesion_A_In_L, in_pNode);
                }
                else{
                    io_SemanticError.cleanError();
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown); 
            }
            else{
                // None of the node are any, check for type comparison
                if(!IS_SS_L(leftNodeKind,rightNodeKind) || IS_VV_L(leftNodeKind, rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Operands_Type, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
                else{
                    in_pNode->setTerminalType(ecpTerminalType_bool1);
                }
            }
            break;
        }
        case ecpOperand_B_DIV:
        case ecpOperand_B_BOR:
        {
            if(IS_Any(leftNodeKind)||IS_Any(rightNodeKind)){
                if(!IS_Any(leftNodeKind) && IS_Any(rightNodeKind) && !IS_S_A(leftNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                                       
                }
                else if(IS_Any(leftNodeKind) && !IS_Any(rightNodeKind) && !IS_S_A(rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                   
                }
                else{
                    io_SemanticError.cleanError();
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(!IS_SS_A(leftNodeKind,rightNodeKind)){
                    // Could add more information regardin the type miss match
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Operands_Type, in_pNode); 
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
                else{
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(leftNodeKind);
                }
            }
            break;
        }
        case ecpOperand_B_MUL:
        {
            if(IS_Any(leftNodeKind)||IS_Any(rightNodeKind)){
                if(!IS_Any(leftNodeKind) && IS_Any(rightNodeKind) && IS_S_L(leftNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                                       
                }
                else if(IS_Any(leftNodeKind) && !IS_Any(rightNodeKind) && IS_S_L(rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                   
                }
                else{
                    io_SemanticError.cleanError();
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(IS_SS_A(leftNodeKind,rightNodeKind) || IS_VS_A(leftNodeKind,rightNodeKind) || IS_VV_A(leftNodeKind,rightNodeKind)){
                    in_pNode->setTerminalType(leftNodeKind);
                    io_SemanticError.cleanError();
                }
                else if(IS_SV_A(leftNodeKind,rightNodeKind)){
                    in_pNode->setTerminalType(rightNodeKind);
                    io_SemanticError.cleanError();
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Operands_Type, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
            }
            break;
        }
        case ecpOperand_B_PLUS:
        case ecpOperand_B_MINUS:
        {
            if(IS_Any(leftNodeKind)||IS_Any(rightNodeKind)){
                if(!IS_Any(leftNodeKind) && IS_Any(rightNodeKind) && !IS_S_A(leftNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                                       
                }
                else if(IS_Any(leftNodeKind) && !IS_Any(rightNodeKind) && !IS_S_A(rightNodeKind)){
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression_L_In_A, in_pNode);                   
                }
                else{
                    io_SemanticError.cleanError();
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(IS_SS_A(leftNodeKind,rightNodeKind) || IS_VV_A(leftNodeKind,rightNodeKind)){
                    in_pNode->setTerminalType(leftNodeKind);
                    io_SemanticError.cleanError();
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Operands_Type, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
            }
            break;
        }
        default:
        {
            printf("Invalid Binary Operator\n");
        }
    }
}
void cpCheckNode(cpFunctionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    cpArgumentsNode* arguments_node = in_pNode->getArguments();
    std::vector<ecpTerminalType> argument_terminal_types;
    cpArgumentsNode* currentNode = arguments_node;
    while(currentNode!=NULL){
        argument_terminal_types.push_back(currentNode->getCurrentArgument()->getTerminalType());
        currentNode = currentNode->getNextArguments();
    }

    switch (in_pNode->m_eFunctionType)
    {
    case ecpFunctionType_dp3:
    {
        if(argument_terminal_types.size()==0){
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else if(argument_terminal_types.size()>2){
            io_SemanticError.setError(ecpSemanticErrorType_Too_Much_Arguments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else{
            if(IS_Any(argument_terminal_types[0]) || IS_Any(argument_terminal_types[1])){
                io_SemanticError.cleanError();
                if(!IS_Any(argument_terminal_types[0]) || IS_Any(argument_terminal_types[1])){
                    if(argument_terminal_types[0]!=ecpTerminalType_float3 ||
                    argument_terminal_types[0]!=ecpTerminalType_float4 ||
                    argument_terminal_types[0]!=ecpTerminalType_int3 ||
                    argument_terminal_types[0]!=ecpTerminalType_int4){
                        // Invalid arguments
                        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    }
                }
                else if(IS_Any(argument_terminal_types[0]) || !IS_Any(argument_terminal_types[1])){
                    if(argument_terminal_types[1]!=ecpTerminalType_float3 ||
                    argument_terminal_types[1]!=ecpTerminalType_float4 ||
                    argument_terminal_types[1]!=ecpTerminalType_int3 ||
                    argument_terminal_types[1]!=ecpTerminalType_int4){
                        // Invalid arguments
                        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    }
                }
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(argument_terminal_types[0] == argument_terminal_types[1]){
                    if(argument_terminal_types[0] == ecpTerminalType_int3 ||argument_terminal_types[0] == ecpTerminalType_int4){
                        in_pNode->setTerminalType(ecpTerminalType_int1);
                        io_SemanticError.cleanError();
                    }
                    else if(argument_terminal_types[0] == ecpTerminalType_float3 ||argument_terminal_types[0] == ecpTerminalType_float4){
                        in_pNode->setTerminalType(ecpTerminalType_float1);
                        io_SemanticError.cleanError();
                    }
                    else{
                        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments,in_pNode);
                        in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    }   
                }
            }
        }    
        break;
    }
    case ecpFunctionType_lit:
    {
        //lit
        if(argument_terminal_types.size()==0){
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else if(argument_terminal_types.size()>1){
            io_SemanticError.setError(ecpSemanticErrorType_Too_Much_Arguments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else{
            if(IS_Any(argument_terminal_types[0])){
                io_SemanticError.cleanError();
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(argument_terminal_types[0] == ecpTerminalType_float4){
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(ecpTerminalType_float4);   
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments,in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
            }
        }
        break;    
    }
    case ecpFunctionType_rsq:
    {
        //rsq
        if(argument_terminal_types.size()==0){
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else if(argument_terminal_types.size()>1){
            io_SemanticError.setError(ecpSemanticErrorType_Too_Much_Arguments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
        }
        else{
            if(IS_Any(argument_terminal_types[0])){
                io_SemanticError.cleanError();
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if(argument_terminal_types[0] == ecpTerminalType_int1 ||
                argument_terminal_types[0] == ecpTerminalType_float1 ){
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(ecpTerminalType_float1);   
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments,in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
            }
        }
        break; 
    }
    default:{
        break;
    }
    }
}

void cpCheckNode(cpUnaryExpressionNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    cpBaseNode *childNode = in_pNode->getChildNode(0);
    ecpTerminalType childNodeKind = childNode->getTerminalType();
    if(childNodeKind==ecpTerminalType_Unknown){
        io_SemanticError.cleanError();
        in_pNode->setTerminalType(ecpTerminalType_Unknown);
    }
    else{
        switch (in_pNode->getOperand())
        {
            case ecpOperand_U_NEG:
            {
                if(IS_S_A(childNodeKind) || IS_V_A(childNodeKind)){
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(childNodeKind);
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
                break;
            }
            case ecpOperand_U_NOT:
            {
                if(IS_S_L(childNodeKind)){
                    io_SemanticError.cleanError();
                    in_pNode->setTerminalType(ecpTerminalType_bool1);
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                }
                break;
            }
            default:{
                printf("Invalid operator for unary expression\n");
            }
        }    
    }
}

void cpCheckNode(cpConstructorNode *in_pNode, cpSymbolTableNode *in_pTable,cpSemanticError& io_SemanticError)
{
    cpArgumentsNode *argument_terminal_typess_node  = (cpArgumentsNode *)in_pNode->getChildNode(0);
    
    cpNormalNode *argument_terminal_types = argument_terminal_typess_node->getCurrentArgument();
    cpArgumentsNode* next_arguments = argument_terminal_typess_node->getNextArguments();
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
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
            break;
        }
        else{
            ecpTerminalType exprType = argument_terminal_types->getTerminalType();
            if(exprType == ecpTerminalType_Unknown){
                // Bypass assignment
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    break;
                }
            }
            argument_terminal_typess_node = next_arguments;
            argument_terminal_types = argument_terminal_typess_node->getCurrentArgument();
            next_arguments = argument_terminal_typess_node->getNextArguments();
        }
    }
    case ecpTerminalType_float3:
    case ecpTerminalType_bool3:
    case ecpTerminalType_int3:
    {
        if (next_arguments==NULL)
        {
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
            break;
        }
        else{
            ecpTerminalType exprType = argument_terminal_types->getTerminalType();
            if(exprType == ecpTerminalType_Unknown){
                // Bypass assignment
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    break;
                }
                
            }
            argument_terminal_typess_node = next_arguments;
            argument_terminal_types = argument_terminal_typess_node->getCurrentArgument();
            next_arguments = argument_terminal_typess_node->getNextArguments();    
        }
    }
    case ecpTerminalType_float2:
    case ecpTerminalType_bool2:
    case ecpTerminalType_int2:
    {
        if (next_arguments==NULL)
        {
            io_SemanticError.setError(ecpSemanticErrorType_Not_Enough_Arugments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
            break;
        }
        else{
            ecpTerminalType exprType = argument_terminal_types->getTerminalType();
            if(exprType == ecpTerminalType_Unknown){
                // Bypass assignment
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    break;
                }
            }
            argument_terminal_typess_node = next_arguments;
            argument_terminal_types = argument_terminal_typess_node->getCurrentArgument();
            next_arguments = argument_terminal_typess_node->getNextArguments();
        }
    }
    case ecpTerminalType_float1:
    case ecpTerminalType_bool1:
    case ecpTerminalType_int1:
    {
        if (next_arguments!=NULL)
        {
            io_SemanticError.setError(ecpSemanticErrorType_Too_Much_Arguments, in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
            break;
        }
        else{
            ecpTerminalType exprType = argument_terminal_types->getTerminalType();
            if(exprType == ecpTerminalType_Unknown){
                // Bypass assignment
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
            }
            else{
                if (!(IS_SV_L(exprType, type) || IS_SV_A(exprType, type)))
                {
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Arguments, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    break;
                }
                else{
                    in_pNode->setTerminalType(type);
                }
            }
        }
        break;
    }
    default:
    {
        printf("Invalid constructor type");
    }
    }
}

void cpCheckNode(cpIdentifierNode* in_pNode,cpSymbolTableNode* in_pTable, cpSemanticError& io_SemanticError){
    // Based on the type of the in_pNode, check different things
    ecpTerminalType type = in_pNode->getTerminalType();
    cpNormalNode* parent = (cpNormalNode*)in_pNode->getParentNode();
    eNodeKind parentKind=parent->getNodeKind();
    
    if(type==ecpTerminalType_Invalid){
        // Invalid declarartion
        in_pNode->setTerminalType(ecpTerminalType_Unknown);
        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Variable, in_pNode);
    }
    else if(!((parentKind==ASSIGNMENT_NODE) && (parent->getChildNode(0)==in_pNode))){
        cpSymbolAttribute* attr = lookupSymbolTable(in_pNode->m_value,in_pNode);
        if (attr->m_isWrite==0){
            io_SemanticError.setError(ecpSemanticErrorType_Variable_Not_Defined,in_pNode);
            in_pNode->setTerminalType(ecpTerminalType_Unknown);
            return;
        }
    }
    else{
        if(in_pNode->isIndexEnable()){
            int access_index = in_pNode->getAccessIndex();
        // Index enable, need to reduce its type;
            if(access_index>=0 && access_index <4){
                io_SemanticError.cleanError();   
                if(IS_Int(type)&& access_index <= (type-ecpTerminalType_int1)){
                    in_pNode->setTerminalType(ecpTerminalType_int1);
                }
                else if(IS_Flt(type)&& access_index <= (type-ecpTerminalType_float1)){
                    in_pNode->setTerminalType(ecpTerminalType_float1);
                }
                else if(IS_Bool(type)&& access_index <= (type-ecpTerminalType_bool1)){
                    in_pNode->setTerminalType(ecpTerminalType_bool1);
                }
                else{
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    io_SemanticError.setError(ecpSemanticErrorType_Invalid_Vecter_Index,in_pNode);
                }
            }
            else{
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
                io_SemanticError.setError(ecpSemanticErrorType_Invalid_Vecter_Index,in_pNode);
            }
        }
    }
}

void cpCheckNode(cpAssignmentNode* in_pNode,cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    
    cpIdentifierNode* variable = in_pNode->getVariable();
    cpBaseNode* expression = in_pNode->getExpression();
    
    ecpTerminalType expression_type = variable->getTerminalType();
    ecpTerminalType variable_type = expression->getTerminalType();

    if(IS_Any(expression_type) || IS_Any(variable_type)){
        io_SemanticError.cleanError();
        in_pNode->setTerminalType(ecpTerminalType_Unknown);
    }
    else{
        cpSymbolAttribute* variable_attribute = lookupSymbolTable(variable->m_value, variable);
        ecpFunctionQualifier variable_qualifier = variable_attribute->m_eQualifier;
        // Check write_only
        switch(variable_qualifier){
            case ecpFunctionQualifier_Result:{
                // Check if the result variable is used within an if node
                cpBaseNode* current_node = in_pNode;
                while(current_node!=NULL){
                    if(current_node->getNodeKind() == IF_STATEMENT_NODE){
                        io_SemanticError.setError(ecpSemanticErrorType_Result_In_If_Statement,in_pNode);
                        in_pNode->setTerminalType(ecpTerminalType_Unknown);
                        return;
                    }
                    current_node = current_node->getParentNode();
                }
                // Keek checking assignment correctness, should not break if the it is not within an if statement
            }
            case ecpFunctionQualifier_Const:{
                if(in_pNode->getParentNode()->getNodeKind() == DECLARATION_NODE){
                    // Declaration node, allow const assign if 
                    if(expression->getNodeType()!= ecpBaseNodeType_Leaf){
                        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Assignment, in_pNode);
                        in_pNode->setTerminalType(ecpTerminalType_Unknown);
                        return;
                    }
                    else{
                        if(expression->getNodeKind()==IDENT_NODE){
                            cpSymbolAttribute* expression_attribute = lookupSymbolTable(((cpIdentifierNode*)expression)->m_value, expression);
                            if(expression_attribute->m_eQualifier!=ecpFunctionQualifier_Uniform){
                                io_SemanticError.setError(ecpSemanticErrorType_Invalid_Const_Assignment, in_pNode);
                                in_pNode->setTerminalType(ecpTerminalType_Unknown);
                                return;
                            }
                        }
                    }
                }
                else{
                    io_SemanticError.setError(ecpSemanticErrorType_Const_Reassign, in_pNode);
                    in_pNode->setTerminalType(ecpTerminalType_Unknown);
                    return;
                }
            } // Const should not be re-assigned.
            case ecpFunctionQualifier_None:{
                break;
            }
            case ecpFunctionQualifier_Attribute: // All of these variable are predefined and read-only
            case ecpFunctionQualifier_Uniform:
            default:{
                io_SemanticError.setError(ecpSemanticErrorType_Invalid_Assignment, in_pNode);
                in_pNode->setTerminalType(ecpTerminalType_Unknown);
                return;
            }
        }
        if(variable_type >= expression_type){
            if((IS_Int(variable_type) && IS_Int(expression_type)) ||
            (IS_Flt(variable_type) && IS_Flt(expression_type)) ||
            (IS_Bool(variable_type) && IS_Bool(expression_type))){
                io_SemanticError.cleanError();
                cpSymbolAttribute* var = lookupSymbolTable(variable->m_value,variable);
                if (var!=NULL) var->m_isWrite=1;
            }
            else{
                io_SemanticError.setError(ecpSemanticErrorType_Invalid_Assignment, in_pNode);
            }
        }
        else{
            io_SemanticError.setError(ecpSemanticErrorType_Invalid_Assignment, in_pNode);
        }
        in_pNode->setTerminalType(ecpTerminalType_Unknown);
    }
}

void cpCheckNode(cpIfStatementNode* in_pNode, cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    cpBaseNode* expreesionNode = in_pNode->getExpression();
    ecpTerminalType expression_type = expreesionNode->getTerminalType();
    if(IS_Any(expression_type) || IS_S_L(expression_type)){
        io_SemanticError.cleanError();
    }
    else{
        io_SemanticError.setError(ecpSemanticErrorType_Invalid_Expression, in_pNode);
    }
    in_pNode->updateTerminalType(ecpTerminalType_Unknown);
    return;
}

void cpCheckNode(cpDeclarationNode* in_pNode, cpSymbolTableNode* in_pTable,cpSemanticError& io_SemanticError){
    //check if there are duplicate declarations in current scope
    if(in_pNode->getTerminalType() == ecpTerminalType_Invalid){
        // Unknown declarartion type, indicate multipler declarartion
        io_SemanticError.setError(ecpSemanticErrorType_Duplicate_Declaration, in_pNode);
    }
    else{
        io_SemanticError.cleanError();
    } 
}
