#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"
#include "symbol.h"

#define DEBUG_PRINT_TREE 0

cpBaseNode *gAST;
std::string gTerminalTypeToStringMap[ecpTerminalType_TypeCount] = {
    "int",
    "ivec2",
    "ivec3",
    "ivec4",
    "bool",
    "bvec2",
    "bvec3",
    "bvec4",
    "float",
    "fvec2",
    "fvec3",
    "fvec4",
};

std::string gQualifierTypeToStringMap[ecpFunctionQualifier_TypeCount] = {
    "",
    "Const",
    "Attribute",
    "Result",
    "Uniform",
};

std::string gOperandTypeToStringMap[ecpOperand_Count] = {
    "-",
    "!",
    "&&",
    "||",
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "+",
    "-",
    "*",
    "/",
    "^"
};

std::string gFunctionTypeToStringMap[ecpFunctionType_count] = {
    "dp3",
    "rsq",
    "lit"
};

std::string gPredefinedVariableString[ecpPredifinedVariable_Count] = {
    "",
    "gl_FragColor",
    "gl_FragDepth",
    "gl_FragCoord",
    "gl_TexCoord",
    "gl_Color",
    "gl_Secondary",
    "gl_FogFragCoord",
    "gl_Light_Half",
    "gl_Light_Ambient",
    "gl_Material_Shininess",
    "env1",
    "env2",
    "env3",
};



std::string toString(ecpFunctionQualifier in_eQualifier)
{ 
    return gQualifierTypeToStringMap[in_eQualifier];
}
std::string toString(ecpTerminalType in_eTerminalType){
    return gTerminalTypeToStringMap[in_eTerminalType];
}

std::string toString(ecpOperand in_eOperand){
    return gOperandTypeToStringMap[in_eOperand];
}

std::string toString(ecpFunctionType in_eFunctionType){
    return gFunctionTypeToStringMap[in_eFunctionType];
}

void cpNormalNode::initChildNodes(int in_iNumOfNodes)
{
    if (in_iNumOfNodes > 0 && m_pChildNodes == NULL)
    {
        m_iNumOfChildNodes = in_iNumOfNodes;
        m_pChildNodes = new cpBaseNode *[in_iNumOfNodes];
        // Initialize child nodes ptr
        for (int i = 0; i < in_iNumOfNodes; i++)
        {
            m_pChildNodes[i] = NULL;
        }
    }
}


cpNormalNode::~cpNormalNode()
{
    if (m_iNumOfChildNodes > 0 && m_pChildNodes != NULL)
    {
        // Starting calling constructor of the child nodes
        for (int i = 0; i < m_iNumOfChildNodes; i++)
        {
            delete m_pChildNodes[i];
        }
        delete[] m_pChildNodes;
    }
}

cpBaseNode *cpNormalNode::getChildNode(int in_iNodeIndex)
{
    if (m_pChildNodes != NULL && in_iNodeIndex >= 0)
    {
        return m_pChildNodes[in_iNodeIndex];
    }
    return NULL;
}

void cpNormalNode::setChildNodes(cpBaseNode *in_pNode, int in_iNodeIndex)
{
    if (m_pChildNodes != NULL && in_iNodeIndex >= 0)
    {
        if(in_pNode!=NULL){
            in_pNode->setParentNode(this);
        }
        m_pChildNodes[in_iNodeIndex] = in_pNode;
    }
}

void cpNormalNode::print()
{
    // In order print
    printSelf();
    if (m_iNumOfChildNodes > 0 && m_pChildNodes != NULL)
    {
        for (int i = 0; i < m_iNumOfChildNodes; i++)
        {
            if(m_pChildNodes[i]!=NULL){
                m_pChildNodes[i]->print();
            }
        }
    }
}

void cpFunctionNode::printSelf()
{
    printf(" ");
}

void cpFunctionNode::print(){
    printf("Call %s(",::toString(m_eFunctionType).c_str());
    getChildNodes()[0]->print();
    printf(")\n");

}

void cpFunctionNode::initialize(va_list in_pArguments)
{
    switch(va_arg(in_pArguments, int)){
        case 0: {
            m_eFunctionType = ecpFunctionType_dp3;
            break;
        }
        case 1:{
            m_eFunctionType = ecpFunctionType_lit;
            break;
        }
        case 2:{
            m_eFunctionType = ecpFunctionType_rsq;
            break;
        }
    }
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpFunctionNode::generateIR(cpIRList& in_IRList){
    // grabing all of the arguments ir
    cpIRRegisterList tempList;
    cpArgumentsNode* arguments = getArguments();
    arguments->generateIR(in_IRList);
    while(arguments!=NULL){
        cpNormalNode* currentArgument = arguments->getCurrentArgument();
        tempList.push_back(currentArgument->getIROutput());
        arguments = arguments->getNextArguments();
    }
    cpIR* ret = NULL;
    switch(m_eFunctionType){
        case ecpFunctionType_dp3:{
            ret = new cpIR(ecpIR_DP3,tempList[0],tempList[1]);
            break;
        }
        case ecpFunctionType_lit:{
            ret = new cpIR(ecpIR_LIT,tempList[0],NULL);
            break;  
        }
        case ecpFunctionType_rsq:{
            ret = new cpIR(ecpIR_RSQ,tempList[0],NULL);
            break;
        }
        default:{
            break;
        }
    }
    m_pIROutput = in_IRList.insert(ret);
}


void cpConstructorNode::printSelf()
{
    printf("Constructor ");
}

void cpConstructorNode::print(){
    printf("%s (",::toString(m_eConstructorType).c_str());
    getArgumentsNode()->print();
    printf(")");
}

void cpConstructorNode::initialize(va_list in_pArguments)
{
    setConstructorType((ecpTerminalType)va_arg(in_pArguments, int));
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpConstructorNode::generateIR(cpIRList& in_IRList){
    getArgumentsNode()->generateIR(in_IRList);
    cpIRRegisterList temp_list;
    cpArgumentsNode* node = getArgumentsNode();
    while(node!=NULL){
        temp_list.push_back(node->getCurrentArgument()->getIROutput());
        node = node->getNextArguments();
    }
    cpIRRegister* output = NULL;
    if(temp_list.size()>=2){
        cpIR_CONST_I* mask_x = new cpIR_CONST_I();
        mask_x->m_ix = 0;
        mask_x->m_iy = 0;
        mask_x->m_iz = 0;
        mask_x->m_iw = 1;
        in_IRList.insert(mask_x);
        cpIR_CONST_I* mask_y = new cpIR_CONST_I();
        mask_y->m_ix = 0;
        mask_y->m_iy = 0;
        mask_y->m_iz = 1;
        mask_y->m_iw = 0;
        in_IRList.insert(mask_y);
        cpIR* com_x = new cpIR(ecpIR_MUL,temp_list[0],mask_x->getDst());
        in_IRList.insert(com_x);
        cpIR* com_y = new cpIR(ecpIR_MUL,temp_list[1],mask_y->getDst());
        in_IRList.insert(com_y);
        cpIR* sum_xy = new cpIR(ecpIR_ADD,com_x->getDst(),com_y->getDst());
        output = in_IRList.insert(sum_xy);
        if(temp_list.size()>=3){
            cpIR_CONST_I* mask_z = new cpIR_CONST_I();
            mask_z->m_ix = 0;
            mask_z->m_iy = 1;
            mask_z->m_iz = 0;
            mask_z->m_iw = 0;
            in_IRList.insert(mask_z);
            cpIR* com_z = new cpIR(ecpIR_MUL,temp_list[2],mask_z->getDst());
            in_IRList.insert(com_z);
            cpIR* sum_xyz = new cpIR(ecpIR_ADD,sum_xy->getDst(),com_z->getDst());
            output = in_IRList.insert(sum_xyz);
            if(temp_list.size()==4){
                cpIR_CONST_I* mask_w = new cpIR_CONST_I();
                mask_w->m_ix = 1;
                mask_w->m_iy = 0;
                mask_w->m_iz = 0;
                mask_w->m_iw = 0;
                in_IRList.insert(mask_w);
                cpIR* com_w = new cpIR(ecpIR_MUL,temp_list[3],mask_w->getDst());
                in_IRList.insert(com_w);
                cpIR* sum_wxyz = new cpIR(ecpIR_ADD,sum_xyz->getDst(),com_w->getDst());
                output = in_IRList.insert(sum_wxyz);
            }
        }
        m_pIROutput = output;
    }
    else{
        printf("Constructor error\n");
    }
}

void cpArgumentsNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpArgumentsNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpNormalNode *), 1);
}

void cpArgumentsNode::printSelf()
{
    printf("Arguments Node: \n");
}

void cpArgumentsNode::print(){
    cpArgumentsNode* next_node = getNextArguments();
    if(next_node!=NULL){
        next_node->print();
        printf(",");
    }
    getCurrentArgument()->print();
}

void cpArgumentsNode::generateIR(cpIRList& in_IRList){
    if(getNextArguments()!=NULL){
        getNextArguments()->generateIR(in_IRList);
    }
    getCurrentArgument()->generateIR(in_IRList);
}

void cpBinaryExpressionNode::printSelf()
{
    printf("Binary Expresion Node, Operand %s\n", ::toString(m_eOperand).c_str());
}

std::string cpBinaryExpressionNode::toString()
{
    return "operation "+::toString(m_eOperand)+" between "+m_pChildNodes[0]->toString()+" and "+
    m_pChildNodes[1]->toString();
}


void cpBinaryExpressionNode::print(){
    printf("Binary ");
    getChildNodes()[0]->print();
    printf(" %s ",::toString(m_eOperand).c_str());
    getChildNodes()[1]->print();
}
void cpBinaryExpressionNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    m_eOperand = (ecpOperand)va_arg(in_pArguments,int);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
}

void cpBinaryExpressionNode::generateIR(cpIRList& in_IRList){
    m_pChildNodes[0]->generateIR(in_IRList);
    m_pChildNodes[1]->generateIR(in_IRList);
    cpIR* newIR = NULL;
    ecpIROpcode targetOpcode;
    cpIRRegister* srcA = m_pChildNodes[0]->getIROutput();
    cpIRRegister* srcB = m_pChildNodes[1]->getIROutput();
    cpIRRegister* srcC = NULL;
    switch(m_eOperand){
        case ecpOperand_B_AND:{targetOpcode=ecpIR_AND;break;}
        case ecpOperand_B_OR:{targetOpcode=ecpIR_OR;break;}
        case ecpOperand_B_EQ:{targetOpcode=ecpIR_EQ;break;}
        case ecpOperand_B_NEQ:{targetOpcode=ecpIR_NEQ;break;}
        case ecpOperand_B_LEQ:{targetOpcode=ecpIR_LEQ;break;}
        case ecpOperand_B_LT:{targetOpcode=ecpIR_SLT;break;}
        case ecpOperand_B_GT:{targetOpcode=ecpIR_GT;break;}
        case ecpOperand_B_GEQ:{targetOpcode=ecpIR_SGE;break;}
        case ecpOperand_B_PLUS:{targetOpcode=ecpIR_ADD;break;}
        case ecpOperand_B_MINUS:{targetOpcode=ecpIR_SUB;break;}
        case ecpOperand_B_MUL:{targetOpcode=ecpIR_MUL;break;}
        case ecpOperand_B_DIV:{targetOpcode=ecpIR_DIV;break;}
        case ecpOperand_B_BOR:{targetOpcode=ecpIR_POW;break;}
        default:{
            break;
        }
    }

    if(m_pChildNodes[0]->getIROutput()->hasMasks()){
        srcA = in_IRList.insert(new cpIR(ecpIR_EPD,srcA,NULL));
    }
    if(m_pChildNodes[1]->getIROutput()->hasMasks()){
        srcB = in_IRList.insert(new cpIR(ecpIR_EPD,srcB,NULL));
    }
    if(targetOpcode == ecpIR_EQ)
    {
        /**********srcA >= srcB and srcB >= srcA ----> srcA == SrcB***********/
        cpIRRegister* temp_a = in_IRList.insert(new cpIR(ecpIR_SGE,srcA,srcB));
        cpIRRegister* temp_b = in_IRList.insert(new cpIR(ecpIR_SGE,srcB,srcA));
        srcA=temp_a;
        srcB=temp_b;
        targetOpcode = ecpIR_MUL;
    }
    if(targetOpcode == ecpIR_NEQ)
    {
        /**********srcA >= srcB and srcB >= srcA ----> srcA == SrcB***********/
        cpIRRegister* temp_a = in_IRList.insert(new cpIR(ecpIR_SGE,srcA,srcB));
        cpIRRegister* temp_b = in_IRList.insert(new cpIR(ecpIR_SGE,srcB,srcA));
        cpIRRegister* equal = in_IRList.insert(new cpIR(ecpIR_MUL,temp_a,temp_b));
        /*************************! srcA == SrcB******************************/
        equal->not_();
        cpIR_CONST_B* bool0 = new cpIR_CONST_B();
        bool0->setScalar(0);
        cpIR_CONST_B* bool1 = new cpIR_CONST_B();
        bool1->setScalar(1);
        srcA = equal;
        srcB = in_IRList.insert(bool0);
        srcC = in_IRList.insert(bool1);   
        targetOpcode = ecpIR_LRP; 
    }
    if(targetOpcode == ecpIR_LEQ)
    {
        /**********srcA >= srcB and srcB >= srcA ----> srcA == SrcB***********/
        cpIRRegister* temp_a = in_IRList.insert(new cpIR(ecpIR_SGE,srcA,srcB));
        cpIRRegister* temp_b = in_IRList.insert(new cpIR(ecpIR_SGE,srcB,srcA));
        cpIRRegister* equal = in_IRList.insert(new cpIR(ecpIR_MUL,temp_a,temp_b));
        /************************srcA<srcB || srcA == SrcB********************/
        cpIRRegister* lt = in_IRList.insert(new cpIR(ecpIR_SLT,srcA,srcB));
        srcA = equal;
        srcB = lt;
        targetOpcode = ecpIR_ADD;
    }
    if(targetOpcode == ecpIR_GT)
    {
        /**********srcA >= srcB and srcB >= srcA ----> srcA == SrcB***********/
        cpIRRegister* temp_a = in_IRList.insert(new cpIR(ecpIR_SGE,srcA,srcB));
        cpIRRegister* temp_b = in_IRList.insert(new cpIR(ecpIR_SGE,srcB,srcA));
        cpIRRegister* equal = in_IRList.insert(new cpIR(ecpIR_MUL,temp_a,temp_b));
        /*******************srcA < SrcB && srcA == SrcB************************/
        cpIRRegister* lt = in_IRList.insert(new cpIR(ecpIR_SLT,srcA,srcB));
        cpIRRegister* leq = in_IRList.insert(new cpIR(ecpIR_ADD,equal,lt));
        /*******************! srcA <= srcB************************************/
        leq->not_();
        cpIR_CONST_B* bool0 = new cpIR_CONST_B();
        bool0->setScalar(0);
        cpIR_CONST_B* bool1 = new cpIR_CONST_B();
        bool1->setScalar(1);
        srcA = leq;
        srcB = in_IRList.insert(bool0);
        srcC = in_IRList.insert(bool1);   
        targetOpcode = ecpIR_LRP; 
    }
    if(targetOpcode == ecpIR_AND)
        targetOpcode = ecpIR_MUL;
    if(targetOpcode == ecpIR_OR)
    {
        cpIRRegister* add = in_IRList.insert(new cpIR(ecpIR_ADD,srcA,srcB));
        cpIRRegister* mul = in_IRList.insert(new cpIR(ecpIR_MUL,srcA,srcB));
        srcA = add;
        srcB = mul;
        targetOpcode = ecpIR_SUB;
    }
    if(targetOpcode == ecpIR_DIV)
    {
        srcB = in_IRList.insert(new cpIR(ecpIR_RSQ,srcB,NULL));
        targetOpcode = ecpIR_MUL;
    }
    newIR = new cpIR(targetOpcode,srcA,srcB,srcC);
    m_pIROutput = in_IRList.insert(newIR);
}

void cpScopeNode::printSelf()
{
    printf("Scope Node\n");
}
void cpScopeNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
}

void cpScopeNode::generateIR(cpIRList& in_IRList){
    if(m_pChildNodes[0]!=NULL){
        m_pChildNodes[0]->generateIR(in_IRList);
    }
    if(m_pChildNodes[1]!=NULL){
        m_pChildNodes[1]->generateIR(in_IRList);
    }
}

void cpIfStatementNode::printSelf()
{
    printf("If Node\n");
}

void cpIfStatementNode::print(){
    printf("If ");
    getExpression()->print();
    printf("\nThen: ");
    getIfStatements()->print();
    cpStatementsNode* statments = getElseStatements();
    if(statments){
        printf("Else: ");
        statments->print();
    }
    
}

void cpIfStatementNode::initialize(va_list in_pArguments)
{
    initChildNodes(3);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 2);
}

void cpIfStatementNode::generateIR(cpIRList& in_IRList){
    cpBaseNode* expression = getExpression();
    expression->generateIR(in_IRList);
    cpIRRegister* expression_reg = expression->getIROutput();
    
    cpStatementsNode* if_statements = getIfStatements();
    cpStatementsNode* else_statements = getElseStatements();
    
    in_IRList.pushIfCondition(expression_reg);
    if_statements->generateIR(in_IRList);
    if(else_statements!=NULL){
        else_statements->generateIR(in_IRList);
    }
    in_IRList.popIfCondition();
}
void cpDeclarationNode::printSelf()
{
    printf("Declaration %s %s %s\n",::toString(m_eQualifier).c_str(),::toString(m_eTargetType).c_str(),m_sIdentifierName.c_str());
}
void cpDeclarationNode::initialize(va_list in_pArguments)
{
    initChildNodes(1);
    m_eQualifier = (ecpFunctionQualifier)va_arg(in_pArguments, int);
    m_eTargetType = (ecpTerminalType)va_arg(in_pArguments, int);
    m_sIdentifierName = std::string(va_arg(in_pArguments, char*));
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpDeclarationNode::generateIR(cpIRList& in_IRList){
    // Allocate an register
    
    cpAssignmentNode* node = getAssignmentNode();
    cpIRRegister* out = NULL;
    if(node!=NULL){
        node->getExpression()->generateIR(in_IRList);
        out = node->getExpression()->getIROutput();
        if(node->getExpression()->getNodeKind()==IDENT_NODE){
            cpIR* ret = NULL;
            switch(m_eTargetType){
                case ecpTerminalType_bool1:
                case ecpTerminalType_bool2:
                case ecpTerminalType_bool3:
                case ecpTerminalType_bool4:{
                    ret = new cpIR_CONST_B();
                    break;
                }
                case ecpTerminalType_float1:
                case ecpTerminalType_float2:
                case ecpTerminalType_float3:
                case ecpTerminalType_float4:{
                    ret = new cpIR_CONST_F();
                    break;
                }
                case ecpTerminalType_int1:
                case ecpTerminalType_int2:
                case ecpTerminalType_int3:
                case ecpTerminalType_int4:{
                    ret = new cpIR_CONST_I();
                    break;
                }
                default:{
                    break;
                }
            }
            cpIRRegister* new_var = in_IRList.insert(ret);
            in_IRList.insert(new cpIR(ecpIR_MOVE,new_var,out));
            out = new_var;
        }
    }
    else{
        cpIR* ret = NULL;
        switch(m_eTargetType){
            case ecpTerminalType_bool1:
            case ecpTerminalType_bool2:
            case ecpTerminalType_bool3:
            case ecpTerminalType_bool4:{
                ret = new cpIR_CONST_B();
                break;
            }
            case ecpTerminalType_float1:
            case ecpTerminalType_float2:
            case ecpTerminalType_float3:
            case ecpTerminalType_float4:{
                ret = new cpIR_CONST_F();
                break;
            }
            case ecpTerminalType_int1:
            case ecpTerminalType_int2:
            case ecpTerminalType_int3:
            case ecpTerminalType_int4:{
                ret = new cpIR_CONST_I();
                break;
            }
            default:{
                break;
            }
        }
        out = in_IRList.insert(ret);
    }
    
    gSymbolIRLookUpTable[m_sIdentifierName] = out;
    m_pIROutput = out;
}

void cpUnaryExpressionNode::printSelf()
{
    printf("Unary %s\n", ::toString(m_eOperand).c_str());
}

void cpUnaryExpressionNode::print(){
    printf("Unary %s(", ::toString(m_eOperand).c_str());
    getChildNodes()[0]->print();
    printf(")");
}
std::string cpUnaryExpressionNode::toString() {
    return "unary operation "+::toString(m_eOperand)+" with "+getChildNodes()[0]->toString();
}



void cpUnaryExpressionNode::initialize(va_list in_pArguments)
{
    m_eOperand = (ecpOperand)va_arg(in_pArguments, int);
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpUnaryExpressionNode::generateIR(cpIRList& in_IRList){
    m_pChildNodes[0]->generateIR(in_IRList);
    cpIR* ret = NULL;
    ecpIROpcode targetOpcode;
    cpIRRegister* reg = NULL;
    if(m_pChildNodes[0]->getIROutput()->hasMasks())
        reg = in_IRList.insert(new cpIR(ecpIR_EPD,m_pChildNodes[0]->getIROutput(),NULL));
    else 
        reg = new cpIRRegister(*(m_pChildNodes[0]->getIROutput()));
    switch(m_eOperand){
        case ecpOperand_U_NEG:{
            targetOpcode = ecpIR_NEG;
            reg->negate();
            ret = new cpIR(targetOpcode,reg,NULL);
            m_pIROutput = in_IRList.insert(ret);
            break;
        }
        case ecpOperand_U_NOT:{
            targetOpcode = ecpIR_NOT;
            reg->not_();
            cpIR_CONST_B* bool0 = new cpIR_CONST_B();
            bool0->setScalar(0);
            cpIR_CONST_B* bool1 = new cpIR_CONST_B();
            bool1->setScalar(1);
            ret = new cpIR (ecpIR_LRP,reg,in_IRList.insert(bool0),in_IRList.insert(bool1));
            m_pIROutput = in_IRList.insert(ret);
            break;
        }
        default:{
            break;
        }
    }
}

void cpAssignmentNode::printSelf()
{
    printf("Assignment Node\n");
}

void cpAssignmentNode::print()
{
    getVariable()->print();
    printf(" = ");
    getExpression()->print();
    printf("\n");
}

void cpAssignmentNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
}

void cpAssignmentNode::generateIR(cpIRList& in_IRList){
    m_pChildNodes[0]->generateIR(in_IRList);
    m_pChildNodes[1]->generateIR(in_IRList);
    // Getting output registerss
    cpIRRegister* dst = m_pChildNodes[0]->getIROutput();
    cpIRRegister* src = m_pChildNodes[1]->getIROutput();
    /**
     * Check if any of the instruction has an array index, if yes
     * then we need to expand it and mask it to the correct location
     **/
    if(dst->hasMasks() || src->hasMasks()){
        // Generate inverse masks for src
        if(src->hasMasks()){
            cpIR_CONST_I* src_mask = cpIR_CONST_I::generateMaskIR(src);
            in_IRList.insert(src_mask);
            cpIRRegister* masked_src = in_IRList.insert(new cpIR(ecpIR_MUL,src_mask->getDst(),src));
            masked_src = new cpIRRegister(*masked_src);
            masked_src->copyMasks(src);
            src = in_IRList.insert(new cpIR(ecpIR_EPD,masked_src,NULL));
        }
        if(dst->hasMasks()){
            cpIR_CONST_I* inv_mask = cpIR_CONST_I::generateInvMaskIR(dst); 
            in_IRList.insert(inv_mask);
            cpIRRegister* pre_dst = in_IRList.insert(new cpIR(ecpIR_MUL,inv_mask->getDst(),dst));
            cpIR_CONST_I* dst_mask = cpIR_CONST_I::generateMaskIR(dst);
            in_IRList.insert(dst_mask);
            cpIR* final_v = new cpIR(ecpIR_MUL,src,dst_mask->getDst());
            src = in_IRList.insert(new cpIR(ecpIR_ADD,in_IRList.insert(final_v),pre_dst));
        }
    }
    in_IRList.insert(new cpIR(ecpIR_MOVE,dst,src));  
}
/** Leaf nodes **/
void cpFloatNode::print()
{
    printf("(f1: %f)", m_value);
}
void cpFloatNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, double);
}

void cpFloatNode::generateIR(cpIRList& in_IRList){
    cpIR_CONST_F* newIR = new cpIR_CONST_F();
    newIR->setScalar(m_value);
    m_pIROutput = in_IRList.insert(newIR);
}

void cpIdentifierNode::print()
{
    if(getAccessIndex()>=0){
        printf("(ID: %s[%d])", m_value.c_str(),getAccessIndex());    
    }
    else{
        printf("(ID: %s)", m_value.c_str());
    }    
}
void cpIdentifierNode::initialize(va_list in_pArguments)
{
    m_value = (std::string)(va_arg(in_pArguments, char*));
    setAccessIndex(va_arg(in_pArguments, int));
    va_arg(in_pArguments,int) == 1?this->EnableIndex():this->DisableIndex();
}

void cpIdentifierNode::generateIR(cpIRList& in_IRList){
    // use identifier should not generate any instruction
    
    // getting the declaration of this identifier
    cpSymbolIRLookUpTableItor itor = gSymbolIRLookUpTable.find(m_value);
    if(itor!=gSymbolIRLookUpTable.end()){
        // Found entry
        cpIRRegister* ir = itor->second;
        m_pIROutput = new cpIRRegister(ir->m_iIRID);
        if(m_bEnableIndex){
            m_pIROutput->m_bMasks[m_iAccessIndex] = true;
        }
    }
    else{
        printf("Undefined identifier\n");
    }
}

void cpIntNode::print()
{
    printf("(i1: %d)", m_value);
}
void cpIntNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, int);
}

void cpIntNode::generateIR(cpIRList& in_IRList){
    cpIR_CONST_I* newIR = new cpIR_CONST_I();
    newIR->setScalar(m_value);
    m_pIROutput = in_IRList.insert(newIR);
}

void cpBoolNode::print()
{
    std::string output = m_value==1?"true":"false";
    printf("b1: %s ",output.c_str());
}
void cpBoolNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, int);
}

void cpBoolNode::generateIR(cpIRList& in_IRList){
    cpIR_CONST_B* newIR = new cpIR_CONST_B();
    newIR->setScalar(m_value);
    m_pIROutput = in_IRList.insert(newIR);
}

void cpStatementsNode::initialize(va_list in_pArguments)
{
    // First element should be the next statements node,
    // Second element should be the real statement node
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpStatementsNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
}

void cpStatementsNode::printSelf()
{
    printf("Statements: \n");
}

void cpStatementsNode::generateIR(cpIRList& in_IRList){
    if(getNextStatementsNode()!=NULL){
        getNextStatementsNode()->generateIR(in_IRList);
    }
    getCurrentStatementNode()->generateIR(in_IRList);
}

void cpDeclarationsNode::printSelf()
{
    printf("Declarations: \n");
}

void cpDeclarationsNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpDeclarationsNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpStatementsNode *), 1);
}

void cpDeclarationsNode::generateIR(cpIRList& in_IRList){
    if(getNextDeclarationsNode()!=NULL){
        getNextDeclarationsNode()->generateIR(in_IRList);
    }
    getCurrentDeclarationNode()->generateIR(in_IRList);
}



#define CHECK_AND_ALLOCATE(__kind, __node_class) \
    \
case __kind:                                     \
    {                                            \
        retNode = new __node_class();            \
        break;                                   \
    \
}

cpBaseNode *allocate_cpNode(eNodeKind in_nodekind, ...)
{
    va_list args;
    va_start(args, in_nodekind);
    cpBaseNode *retNode = NULL;
    switch (in_nodekind)
    {
        CHECK_AND_ALLOCATE(BINARY_EXPRESSION_NODE, cpBinaryExpressionNode)
        CHECK_AND_ALLOCATE(UNARY_EXPRESION_NODE, cpUnaryExpressionNode)
        CHECK_AND_ALLOCATE(ASSIGNMENT_NODE, cpAssignmentNode)
        CHECK_AND_ALLOCATE(FUNCTION_NODE, cpFunctionNode)
        CHECK_AND_ALLOCATE(SCOPE_NODE, cpScopeNode)
        CHECK_AND_ALLOCATE(CONSTRUCTOR_NODE, cpConstructorNode)
        //CHECK_AND_ALLOCATE(WHILE_STATEMENT_NODE, cpWhileStatmentNode)
        CHECK_AND_ALLOCATE(IF_STATEMENT_NODE, cpIfStatementNode)
        CHECK_AND_ALLOCATE(DECLARATION_NODE, cpDeclarationNode)
        CHECK_AND_ALLOCATE(STATEMENT_NODE, cpStatementsNode)
        CHECK_AND_ALLOCATE(DECLARATIONS_NODE, cpDeclarationsNode)
        CHECK_AND_ALLOCATE(STATEMENTS_NODE, cpStatementsNode)
        CHECK_AND_ALLOCATE(ARG_NODE, cpArgumentsNode)
        // Leaf ndoes
        CHECK_AND_ALLOCATE(FLOAT_NODE, cpFloatNode)
        CHECK_AND_ALLOCATE(IDENT_NODE, cpIdentifierNode)
        CHECK_AND_ALLOCATE(INT_NODE, cpIntNode)
        CHECK_AND_ALLOCATE(BOOL_NODE, cpBoolNode)
    //TODO: Add more under here, for each node, please implement the printSelf and Initialize interface
    default:
    {
        return NULL;
    }
    }
    retNode->initialize(args);
    va_end(args);
    return retNode;
}

void print_cpNode(cpNormalNode *in_pNode)
{
    in_pNode->print();
}

void free_cpNode(cpBaseNode *in_pNode)
{
    delete in_pNode;
}

void cpInitSymbolIRLookUpTable(){
    for(int i = 0; i < ecpPredifinedVariable_Count;i++){
        gSymbolIRLookUpTable[gPredefinedVariableString[i]] = new cpIRRegister(-i);
    }
}
