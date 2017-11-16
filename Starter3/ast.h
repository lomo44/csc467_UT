
#ifndef _AST_H_
#define _AST_H_

#include <cstdarg>
//#include <stdarg.h>
#include <string>
#include <vector>
// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

enum eNodeKind
{
    SCOPE_NODE = 0,
    UNARY_EXPRESION_NODE,
    BINARY_EXPRESSION_NODE,
    INT_NODE,
    FLOAT_NODE,
    IDENT_NODE,
    VAR_NODE,
    FUNCTION_NODE,
    CONSTRUCTOR_NODE,
    BOOL_NODE,
    STATEMENT_NODE,
    IF_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    ASSIGNMENT_NODE,
    NESTED_SCOPE_NODE,
    DECLARATION_NODE,
    DECLARATIONS_NODE,
    STATEMENTS_NODE,
    ARG_NODE,
    NUM_OF_KIND,
    INVALID = -1,
    UNKNOWN = -2
};

enum ecpVisitorTraversalType
{
    ecpVisitorTraversalType_InOrder,
    ecpVisitorTraversalType_PostOrder
};

enum ecpBaseNodeType
{
    ecpBaseNodeType_Normal,
    ecpBaseNodeType_Leaf
};

enum ecpTerminalType
{

    ecpTerminalType_int1,
    ecpTerminalType_int2,
    ecpTerminalType_int3,
    ecpTerminalType_int4,
    ecpTerminalType_bool1,
    ecpTerminalType_bool2,
    ecpTerminalType_bool3,
    ecpTerminalType_bool4,
    ecpTerminalType_float1,
    ecpTerminalType_float2,
    ecpTerminalType_float3,
    ecpTerminalType_float4,
    ecpTerminalType_Invalid,
    ecpTerminalType_Unknown
};

enum ecpFunctionQualifier{
    ecpFunctionQualifier_None,
    ecpFunctionQualifier_Const,
    ecpFunctionQualifier_Attribute,
    ecpFunctionQualifier_Result,
    ecpFunctionQualifier_Uniform
};

#define IS_WRITE_ONLY(x) ((x)==ecpFunctionQualifier_Result)
#define IS_READ_ONLY(x) ((x)==ecpFunctionQualifier_Attribute || (x)==ecpFunctionQualifier_Uniform)

enum ecpFunctionType
{
    ecpFunctionType_dp3,
    ecpFunctionType_rsq,
    ecpFunctionType_lit
};

class cpBaseNode
{
  public:
    cpBaseNode(){};
    cpBaseNode(eNodeKind in_NodeKind) : m_TerminalKind(ecpTerminalType_Unknown),
                                        m_NodeKind(in_NodeKind),
                                        m_pParentNode(NULL),
                                        m_eNodeType(ecpBaseNodeType_Normal){};
    virtual ~cpBaseNode(){};
    void setNodeType(ecpBaseNodeType in_eNodeType) { m_eNodeType = in_eNodeType; };
    ecpBaseNodeType getNodeType() { return m_eNodeType; };
    eNodeKind getNodeKind() { return m_NodeKind; }
    /** Interfaces that we need to implements **/
    // Initialize a node, this node could be a leaf node or a non-leaf node,
    // But they should share the same interface
    virtual void initialize(va_list in_pArguments) = 0;
    // Print function for each node
    // For non-leaf node, we need to implement the printSelf() function to print
    // its functionality. For leaf node, overwrite this to print the value or type
    virtual void print() = 0;
    void updateTerminalType(ecpTerminalType in_Kind)
    {
        if (m_TerminalKind == ecpTerminalType_Unknown)
            m_TerminalKind = in_Kind;
    }
    bool isTerminalType(ecpTerminalType in_Kind) { return m_TerminalKind == in_Kind; };
    void setTerminalType(ecpTerminalType in_Kind) { m_TerminalKind = in_Kind; };
    ecpTerminalType getTerminalType() { return m_TerminalKind; };

    cpBaseNode *getParentNode() { return m_pParentNode; }
    void setParentNode(cpBaseNode *in_pParentNode) { m_pParentNode = in_pParentNode; }

  protected:
    ecpTerminalType m_TerminalKind;
    eNodeKind m_NodeKind;
    cpBaseNode *m_pParentNode;
    ecpBaseNodeType m_eNodeType;
};

class cpLeafNode : public cpBaseNode
{
  public:
    cpLeafNode(){};
    cpLeafNode(eNodeKind in_NodeKind) : cpBaseNode(in_NodeKind) { m_eNodeType = ecpBaseNodeType_Leaf; }
    virtual ~cpLeafNode(){};
    virtual void print() = 0;
    virtual void initialize(va_list in_pArguments) = 0;
};

class cpIntNode : public cpLeafNode
{
  public:
    cpIntNode() : cpLeafNode(INT_NODE){};
    virtual ~cpIntNode(){};
    virtual void print();
    virtual void initialize(va_list in_pArguments);
    int m_value;
};

class cpBoolNode : public cpLeafNode
{
  public:
    cpBoolNode() : cpLeafNode(BOOL_NODE){};
    virtual ~cpBoolNode(){};
    virtual void print();
    virtual void initialize(va_list in_pArguments);
    bool m_value;
};

class cpFloatNode : public cpLeafNode
{
  public:
    cpFloatNode() : cpLeafNode(FLOAT_NODE){};
    virtual ~cpFloatNode(){};
    virtual void print();
    virtual void initialize(va_list in_pArguments);
    float m_value;
};

class cpIdentifierNode : public cpLeafNode
{
  public:
    cpIdentifierNode() : cpLeafNode(IDENT_NODE){};
    virtual ~cpIdentifierNode(){};
    virtual void print();
    virtual void initialize(va_list in_pArguments);
    std::string m_value;
    int getAccessIndex(){return m_iAccessIndex;}
    void setAccessIndex(int in_iAccessIndex){m_iAccessIndex = in_iAccessIndex;}
  private:
    int m_iAccessIndex;
};

class cpNormalNode : public cpBaseNode
{
  public:
    cpNormalNode(){};
    cpNormalNode(eNodeKind in_NodeKind) : cpBaseNode(in_NodeKind),
                                          m_Operand(-1),
                                          m_iNumOfChildNodes(-1),
                                          m_pChildNodes(NULL){};
    virtual ~cpNormalNode();
    void initChildNodes(int in_iNumOfNodes);
    int getNumOfChildNodes() { return m_iNumOfChildNodes; };
    void setChildNodes(cpBaseNode *in_pNode, int in_iNodeIndex);
    cpBaseNode *getChildNode(int in_iNodeIndex);
    cpBaseNode **getChildNodes() { return m_pChildNodes; };
    int getOperand() { return m_Operand; }
    virtual void print();
    /** These are some of the interfaces that we need to implements **/
    // Self-print for any non-leaf node
    virtual void printSelf() = 0;
    // Initialization function for any non-leaf node
    virtual void initialize(va_list in_pArguments) = 0;

  protected:
    int m_Operand;
    int m_iNumOfChildNodes;
    cpBaseNode **m_pChildNodes;
};



class cpScopeNode : public cpNormalNode
{
  public:
    cpScopeNode() : cpNormalNode(SCOPE_NODE){};
    virtual ~cpScopeNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
};

class cpVariableNode : public cpNormalNode
{
  public:
    cpVariableNode() : cpNormalNode(WHILE_STATEMENT_NODE){};
    virtual ~cpVariableNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
};
class cpStatementsNode : public cpNormalNode{
  public:
    cpStatementsNode() : cpNormalNode(STATEMENTS_NODE){};
    virtual ~cpStatementsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual cpStatementsNode* getNextStatementsNode(){return (cpStatementsNode*)m_pChildNodes[0];}
    virtual cpBaseNode* getCurrentStatementNode(){return m_pChildNodes[1];}
};

class cpAssignmentNode : public cpNormalNode
{
  public:
    cpAssignmentNode() : cpNormalNode(ASSIGNMENT_NODE){};
    virtual ~cpAssignmentNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    cpIdentifierNode* getVariable(){return (cpIdentifierNode*)m_pChildNodes[0];}
    cpNormalNode* getExpression(){return (cpNormalNode*)m_pChildNodes[1]; }
};

class cpConstructorNode : public cpNormalNode
{
  public:
    cpConstructorNode() : cpNormalNode(CONSTRUCTOR_NODE){};
    virtual ~cpConstructorNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    void setConstructorType(ecpTerminalType in_eConstructorType){m_eConstructorType = in_eConstructorType;}
    ecpTerminalType getConstructorType(){return m_eConstructorType;}
private:
    ecpTerminalType m_eConstructorType;
};

class cpWhileStatmentNode : public cpNormalNode
{
  public:
    cpWhileStatmentNode() : cpNormalNode(WHILE_STATEMENT_NODE){};
    virtual ~cpWhileStatmentNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
};

class cpIfStatementNode : public cpNormalNode
{
  public:
    cpIfStatementNode() : cpNormalNode(IF_STATEMENT_NODE){};
    virtual ~cpIfStatementNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    cpBaseNode* getExpression(){return m_pChildNodes[0];}
    cpStatementsNode* getIfStatements(){return (cpStatementsNode*)m_pChildNodes[1];}
    cpStatementsNode* getElseStatements(){return (cpStatementsNode*)m_pChildNodes[2];}

};

class cpDeclarationNode : public cpNormalNode
{
  public:
    cpDeclarationNode() : cpNormalNode(DECLARATION_NODE){};
    virtual ~cpDeclarationNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    cpAssignmentNode* getAssignmentNode(){return (cpAssignmentNode*)m_pChildNodes[0];};
    ecpFunctionQualifier m_eQualifier;
    int m_iVariableSize;
    std::string m_sIdentifierName;
};

class cpDeclarationsNode : public cpNormalNode{
  public:
    cpDeclarationsNode() : cpNormalNode(DECLARATIONS_NODE){};
    virtual ~cpDeclarationsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual cpDeclarationsNode* getNextDeclarationsNode(){return (cpDeclarationsNode*)m_pChildNodes[0];}
    virtual cpDeclarationNode* getCurrentDeclarationNode(){return (cpDeclarationNode*)m_pChildNodes[1];}
};


class cpUnaryExpressionNode : public cpNormalNode
{
  public:
    cpUnaryExpressionNode() : cpNormalNode(UNARY_EXPRESION_NODE){};
    virtual ~cpUnaryExpressionNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
};

class cpBinaryExpressionNode : public cpNormalNode
{
  public:
    cpBinaryExpressionNode() : cpNormalNode(BINARY_EXPRESSION_NODE){};
    virtual ~cpBinaryExpressionNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
};

class cpArgumentsNode : public cpNormalNode
{
    public:
    cpArgumentsNode() : cpNormalNode(BINARY_EXPRESSION_NODE){};
    virtual ~cpArgumentsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    cpArgumentsNode* getNextArguments(){return (cpArgumentsNode*)m_pChildNodes[0];}
    cpNormalNode* getCurrentArgument(){return (cpArgumentsNode*)m_pChildNodes[1];}
};

class cpFunctionNode : public cpNormalNode
{
  public:
    cpFunctionNode() : cpNormalNode(FUNCTION_NODE){};
    virtual ~cpFunctionNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    ecpFunctionType m_eFunctionType;
};

/** Factory function to create different kind of nodes**/
cpBaseNode *allocate_cpNode(eNodeKind in_nodeKind, ...);
void free_cpNode(cpBaseNode *in_pNode);
void print_cpNode(cpBaseNode *in_pNode);

#endif /* AST_H_ */
