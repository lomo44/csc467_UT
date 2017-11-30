#ifndef _AST_H_
#define _AST_H_

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

#include "cpIR.h"
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
    ecpTerminalType_Unknown,
    ecpTerminalType_Invalid,
    ecpTerminalType_TypeCount
};

enum ecpOperand{
    ecpOperand_U_NEG,
    ecpOperand_U_NOT,
    ecpOperand_B_AND,
    ecpOperand_B_OR,
    ecpOperand_B_EQ,
    ecpOperand_B_NEQ,
    ecpOperand_B_LT,
    ecpOperand_B_LEQ,
    ecpOperand_B_GT,
    ecpOperand_B_GEQ,
    ecpOperand_B_PLUS,
    ecpOperand_B_MINUS,
    ecpOperand_B_MUL,
    ecpOperand_B_DIV,
    ecpOperand_B_BOR,
    ecpOperand_Count,
    ecpOperand_Unset
};

enum ecpFunctionQualifier{
    ecpFunctionQualifier_None,
    ecpFunctionQualifier_Const,
    ecpFunctionQualifier_Attribute,
    ecpFunctionQualifier_Result,
    ecpFunctionQualifier_Uniform,
    ecpFunctionQualifier_TypeCount
};


enum ecpFunctionType
{
    ecpFunctionType_dp3,
    ecpFunctionType_rsq,
    ecpFunctionType_lit,
    ecpFunctionType_count
};


enum ecpPredefinedVariable{
    ecp_Reserve,
    ecp_gl_FragColor,
    ecp_gl_FragDepth,
    ecp_gl_FragCoord,
    ecp_gl_TexCoord,
    ecp_gl_Color,
    ecp_gl_Secondary,
    ecp_gl_FogFragCoord,
    ecp_gl_Light_Half,
    ecp_gl_Light_Ambient,
    ecp_gl_Material_Shininess,
    ecp_env1,
    ecp_env2,
    ecp_env3,
    ecpPredifinedVariable_Count
};


std::string toString(ecpFunctionQualifier in_eQualifier);
std::string toString(ecpTerminalType in_eTerminalType);
std::string toString(ecpOperand in_eOperand);
std::string toString(ecpFunctionType in_eFuncType);
std::string toString(ecpPredefinedVariable in_eVariableID);

class cpBaseNode
{
  public:
    cpBaseNode(){};
    cpBaseNode(eNodeKind in_NodeKind) : m_TerminalKind(ecpTerminalType_Unknown),
                                        m_NodeKind(in_NodeKind),
                                        m_pParentNode(NULL),
                                        m_eNodeType(ecpBaseNodeType_Normal),
                                        m_pIROutput(NULL){};
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
    virtual std::string toString() = 0;
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
    void setLineNumber(int in_iLineNumber){m_iLineNumber = in_iLineNumber;}
    void setLineAndCol(int in_iLine, int in_iCol){m_iLineNumber = in_iLine, m_iColNumber = in_iCol;}
    void getLineAndCol(int* in_pLine, int* in_pCol){*in_pLine = m_iLineNumber;*in_pCol=m_iColNumber;}
    virtual cpIRRegister* getIROutput(){return m_pIROutput;};
    virtual void generateIR(cpIRList& in_IRList) = 0;
  protected:
    int m_iLineNumber;
    int m_iColNumber;
    ecpTerminalType m_TerminalKind;
    eNodeKind m_NodeKind;
    cpBaseNode *m_pParentNode;
    ecpBaseNodeType m_eNodeType;
  protected:
    cpIRRegister* m_pIROutput;
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
    cpIntNode() : cpLeafNode(INT_NODE){m_TerminalKind = ecpTerminalType_int1;};
    virtual ~cpIntNode(){};
    virtual void print();
    virtual std::string toString() {return "constant int";}
    virtual void initialize(va_list in_pArguments);
    virtual void generateIR(cpIRList& in_IRList);
    int m_value;
};

class cpBoolNode : public cpLeafNode
{
  public:
    cpBoolNode() : cpLeafNode(BOOL_NODE){m_TerminalKind = ecpTerminalType_bool1;};
    virtual ~cpBoolNode(){};
    virtual void print();
    virtual std::string toString() {return "constant bool";}
    virtual void initialize(va_list in_pArguments);
    virtual void generateIR(cpIRList& in_IRList);
    bool m_value;
};

class cpFloatNode : public cpLeafNode
{
  public:
    cpFloatNode() : cpLeafNode(FLOAT_NODE){m_TerminalKind = ecpTerminalType_float1;};
    virtual ~cpFloatNode(){};
    virtual void print();
    virtual std::string toString() {return "constant float";}
    virtual void initialize(va_list in_pArguments);
    virtual void generateIR(cpIRList& in_IRList);
    float m_value;
};

class cpIdentifierNode : public cpLeafNode
{
  public:
    cpIdentifierNode() : cpLeafNode(IDENT_NODE){m_iAccessIndex =0;};
    virtual ~cpIdentifierNode(){};
    virtual void print();
    virtual void initialize(va_list in_pArguments);
    virtual std::string toString() {return ::toString(m_TerminalKind)+" "+m_value;}
    std::string m_value;
    int getAccessIndex(){return m_iAccessIndex;}
    void setAccessIndex(int in_iAccessIndex){m_iAccessIndex = in_iAccessIndex;}
    void EnableIndex(){m_bEnableIndex=true;}
    void DisableIndex(){m_bEnableIndex=false;}
    void generateIR(cpIRList& in_IRList);
    bool isIndexEnable(){return m_bEnableIndex;}
  private:
    bool m_bEnableIndex;
    int m_iAccessIndex;
};

class cpNormalNode : public cpBaseNode
{
  public:
    cpNormalNode(){};
    cpNormalNode(eNodeKind in_NodeKind) : cpBaseNode(in_NodeKind),
                                          m_eOperand(ecpOperand_Unset),
                                          m_iNumOfChildNodes(-1),
                                          m_pChildNodes(NULL){};
    virtual ~cpNormalNode();
    void initChildNodes(int in_iNumOfNodes);
    int getNumOfChildNodes() { return m_iNumOfChildNodes; };
    void setChildNodes(cpBaseNode *in_pNode, int in_iNodeIndex);
    cpBaseNode *getChildNode(int in_iNodeIndex);
    cpBaseNode **getChildNodes() { return m_pChildNodes; };
    ecpOperand getOperand() { return m_eOperand; }
    virtual void print();
    /** These are some of the interfaces that we need to implements **/
    // Self-print for any non-leaf node
    virtual void printSelf() = 0;
    // Initialization function for any non-leaf node
    virtual void initialize(va_list in_pArguments) = 0;

  protected:
    ecpOperand m_eOperand;
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
    virtual void generateIR(cpIRList& in_IRList);
    virtual std::string toString() {return "scope";}
};


class cpStatementsNode : public cpNormalNode{
  public:
    cpStatementsNode() : cpNormalNode(STATEMENTS_NODE){};
    virtual ~cpStatementsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual cpStatementsNode* getNextStatementsNode(){return (cpStatementsNode*)m_pChildNodes[0];}
    virtual cpBaseNode* getCurrentStatementNode(){return m_pChildNodes[1];}
    virtual void generateIR(cpIRList& in_IRList);
    virtual std::string toString() {return "statements";}
};

class cpAssignmentNode : public cpNormalNode
{
  public:
    cpAssignmentNode() : cpNormalNode(ASSIGNMENT_NODE){};
    virtual ~cpAssignmentNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual std::string toString() {return getVariable()->toString()+" = "+::toString(getExpression()->getTerminalType())
    +"("+getExpression()->toString()+")";}
    virtual void generateIR(cpIRList& in_IRList);
    cpIdentifierNode* getVariable(){return (cpIdentifierNode*)m_pChildNodes[0];}
    cpNormalNode* getExpression(){return (cpNormalNode*)m_pChildNodes[1]; }
};

class cpArgumentsNode : public cpNormalNode
{
    public:
    cpArgumentsNode() : cpNormalNode(ARG_NODE){};
    virtual ~cpArgumentsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual void generateIR(cpIRList& in_IRList);
    virtual std::string toString() {return "arguments";}
    void setNextArguments(cpArgumentsNode* in_pArguments){setChildNodes(in_pArguments,0);}
    void setCurrentArgument(cpNormalNode* in_pArgument){setChildNodes(in_pArgument,1);}
    cpArgumentsNode* getNextArguments(){return (cpArgumentsNode*)m_pChildNodes[0];}
    cpBaseNode* getCurrentArgument(){return m_pChildNodes[1];}
};

class cpConstructorNode : public cpNormalNode
{
  public:
    cpConstructorNode() : cpNormalNode(CONSTRUCTOR_NODE){};
    virtual ~cpConstructorNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual void generateIR(cpIRList& in_IRList);
    std::string toString(){return "constructor";}
    void setConstructorType(ecpTerminalType in_eConstructorType){m_eConstructorType = in_eConstructorType;}
    ecpTerminalType getConstructorType(){return m_eConstructorType;}
    void setArgumentsNode(cpArgumentsNode* in_pArguments){setChildNodes(in_pArguments,0);}
    cpArgumentsNode* getArgumentsNode(){return (cpArgumentsNode*)getChildNode(0);}
private:
    ecpTerminalType m_eConstructorType;
};


class cpIfStatementNode : public cpNormalNode
{
  public:
    cpIfStatementNode() : cpNormalNode(IF_STATEMENT_NODE){};
    virtual ~cpIfStatementNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual std::string toString() {return "if Statement";}
    virtual void generateIR(cpIRList& in_IRList);
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
    virtual std::string toString() {return "declaration";}
    virtual void generateIR(cpIRList& in_IRList);
    cpAssignmentNode* getAssignmentNode(){return (cpAssignmentNode*)m_pChildNodes[0];};
    ecpFunctionQualifier m_eQualifier;
    ecpTerminalType m_eTargetType;
    int m_iVariableSize;
    std::string m_sIdentifierName;
};

class cpDeclarationsNode : public cpNormalNode{
  public:
    cpDeclarationsNode() : cpNormalNode(DECLARATIONS_NODE){};
    virtual ~cpDeclarationsNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual std::string toString() {return "declarations";}
    virtual void generateIR(cpIRList& in_IRList);
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
    virtual void print();
    virtual void generateIR(cpIRList& in_IRList);
    virtual std::string toString();
};

class cpBinaryExpressionNode : public cpNormalNode
{
  public:
    cpBinaryExpressionNode() : cpNormalNode(BINARY_EXPRESSION_NODE){};
    virtual ~cpBinaryExpressionNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual void generateIR(cpIRList& in_IRList);
    virtual std::string toString(); 
};



class cpFunctionNode : public cpNormalNode
{
  public:
    cpFunctionNode() : cpNormalNode(FUNCTION_NODE){};
    virtual ~cpFunctionNode(){};
    virtual void initialize(va_list in_pArguments);
    virtual void printSelf();
    virtual void print();
    virtual std::string toString() {return "function "+ ::toString(m_eFunctionType);}
    cpArgumentsNode* getArguments(){return (cpArgumentsNode*)getChildNodes()[0];}
    virtual void generateIR(cpIRList& in_IRLIst);
    ecpFunctionType m_eFunctionType;
};

extern cpBaseNode* gAST;
/** Factory function to create different kind of nodes**/
cpBaseNode *allocate_cpNode(eNodeKind in_nodeKind, ...);
void free_cpNode(cpBaseNode *in_pNode);
void print_cpNode(cpBaseNode *in_pNode);
void cpInitSymbolIRLookUpTable();

#endif /* AST_H_ */
