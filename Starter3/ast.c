#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

cpBaseNode *gAST;

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
    printf("Function \n");
}

void cpFunctionNode::initialize(va_list in_pArguments)
{
    m_Operand = va_arg(in_pArguments, int);
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpConstructorNode::printSelf()
{
    printf("Constructor \n");
}

void cpConstructorNode::initialize(va_list in_pArguments)
{
    m_Operand = va_arg(in_pArguments, int);
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpArgumentsNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpArgumentsNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpNormalNode *), 0);
}

void cpArgumentsNode::printSelf()
{
    printf("Arguments Node: ");
}

void cpBinaryExpressionNode::printSelf()
{
    printf("Binary Expresion Node, Operand %d\n", m_Operand);
}
void cpBinaryExpressionNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
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

void cpWhileStatmentNode::printSelf()
{
    printf("While Node\n");
}
void cpWhileStatmentNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
}

void cpIfStatementNode::printSelf()
{
    printf("If Node\n");
}
void cpIfStatementNode::initialize(va_list in_pArguments)
{
    initChildNodes(3);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 2);
}

void cpDeclarationNode::printSelf()
{
    //printf("Declaration Node\n",m_Operand);
    //TODO: reImplement this
}
void cpDeclarationNode::initialize(va_list in_pArguments)
{
    initChildNodes(1);
    m_eQualifier = (ecpFunctionQualifier)va_arg(in_pArguments, int);
    m_eTargetType = (ecpTerminalType)va_arg(in_pArguments, int);
    m_sIdentifierName = std::string(va_arg(in_pArguments, char*));
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpUnaryExpressionNode::printSelf()
{
    printf("Unary Expression Node, Operand %d\n", m_Operand);
}
void cpUnaryExpressionNode::initialize(va_list in_pArguments)
{
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpAssignmentNode::printSelf()
{
    printf("Assignment Node\n");
}

void cpAssignmentNode::initialize(va_list in_pArguments)
{
    initChildNodes(1);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

void cpVariableNode::printSelf()
{
    printf("Variable Node, index %d\n", m_Operand);
}

void cpVariableNode::initialize(va_list in_pArguments)
{
    initChildNodes(1);
    m_Operand = va_arg(in_pArguments, int);
    setChildNodes(va_arg(in_pArguments, cpBaseNode *), 0);
}

/** Leaf nodes **/
void cpFloatNode::print()
{
    printf("Float Value: %f\n", m_value);
}
void cpFloatNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, double);
}

void cpIdentifierNode::print()
{
    printf("ID : %s\n", m_value.c_str());
}
void cpIdentifierNode::initialize(va_list in_pArguments)
{
    m_value = (std::string)(va_arg(in_pArguments, char*));
    setAccessIndex(va_arg(in_pArguments, int));
}

void cpIntNode::print()
{
    printf("Int Value : %d\n", m_value);
}
void cpIntNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, int);
}

void cpBoolNode::print()
{
    printf("ID : %d\n", m_value);
}
void cpBoolNode::initialize(va_list in_pArguments)
{
    m_value = va_arg(in_pArguments, int);
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
    printf("Statements: ");
}

void cpDeclarationsNode::printSelf()
{
    printf("Declarations: ");
}

void cpDeclarationsNode::initialize(va_list in_pArguments)
{
    initChildNodes(2);
    setChildNodes(va_arg(in_pArguments, cpDeclarationsNode *), 0);
    setChildNodes(va_arg(in_pArguments, cpStatementsNode *), 1);
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
        CHECK_AND_ALLOCATE(VAR_NODE, cpVariableNode)
        CHECK_AND_ALLOCATE(SCOPE_NODE, cpScopeNode)
        CHECK_AND_ALLOCATE(CONSTRUCTOR_NODE, cpConstructorNode)
        CHECK_AND_ALLOCATE(WHILE_STATEMENT_NODE, cpWhileStatmentNode)
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
