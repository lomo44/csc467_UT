
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
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

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;
enum eNodeKind{
  SCOPE_NODE=0,
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
  ARG_NODE,
  NUM_OF_KIND,
  INVALID = -1,
  UNKNOWN = -2
};

struct node_
{
  // an example of tagging each node with a type
  eNodeKind kind;

  union {
    struct
    {
      // declarations?
      // statements?
    } scope;

    struct
    {
      int op;
      node *right;
    } unary_expr;

    struct
    {
      int op;
      node *left;
      node *right;
    } binary_expr;

    // etc.
  };
};

// node *ast_allocate(eNodeKind type, ...);
// void ast_free(node *ast);
// void ast_print(node *ast);


enum ecpVisitorTraversalType{
  ecpVisitorTraversalType_InOrder,
  ecpVisitorTraversalType_PostOrder
};

enum ecpBaseNodeType{
  ecpBaseNodeType_Normal,
  ecpBaseNodeType_Leaf
};

enum ecpTerminalType{
  ecpTerminalType_Invalid,
  ecpTerminalType_Unknown,
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
};

class cpBaseNode;

typedef std::vector<cpBaseNode*> cpNodeList;
extern cpNodeList gGlobalNodeList[NUM_OF_KIND];


class cpBaseNode{
public:
  cpBaseNode(){};
  cpBaseNode(eNodeKind in_NodeKind):m_NodeKind(in_NodeKind){pushNodeList();};
  virtual ~cpBaseNode(){};
  void setNodeType(ecpBaseNodeType in_eNodeType){m_eNodeType = in_eNodeType;};
  ecpBaseNodeType getNodeType(){return m_eNodeType;};
  eNodeKind getNodeKind(){return m_NodeKind;}
  /** Interfaces that we need to implements **/
  // Initialize a node, this node could be a leaf node or a non-leaf node, 
  // But they should share the same interface
  virtual void initialize(va_list in_pArguments) = 0;
  // Print function for each node
  // For non-leaf node, we need to implement the printSelf() function to print
  // its functionality. For leaf node, overwrite this to print the value or type
  virtual void print() = 0;
  void updateTerminalType(ecpTerminalType in_Kind){if(m_TerminalKind==ecpTerminalType_Unknown) m_TerminalKind = in_Kind;}
  bool isTerminalType(ecpTerminalType in_Kind){return m_TerminalKind == in_Kind;};
  void setTerminalType(ecpTerminalType in_Kind){m_TerminalKind = in_Kind;};
  ecpTerminalType getTerminalType(){return m_TerminalKind;};
  
  cpBaseNode* getParentNode(){return m_pParentNode;}
  void setParentNode(cpBaseNode* in_pParentNode){m_pParentNode = in_pParentNode;}
  void pushNodeList(){gGlobalNodeList[m_NodeKind].push_back(this);}
protected:
  ecpTerminalType m_TerminalKind = ecpTerminalType_Unknown;
  eNodeKind       m_NodeKind;
  cpBaseNode*     m_pParentNode = NULL;
  ecpBaseNodeType m_eNodeType;
};

class cpNormalNode : public cpBaseNode
{
public:
  cpNormalNode(){};
  cpNormalNode(eNodeKind in_NodeKind):cpBaseNode(in_NodeKind){};
  virtual ~cpNormalNode();
  void    initChildNodes(int in_iNumOfNodes);
  int     getNumOfChildNodes(){return m_iNumOfChildNodes;};
  void    setChildNodes(cpBaseNode* in_pNode, int in_iNodeIndex);
  cpBaseNode* getChildNode(int in_iNodeIndex);
  cpBaseNode** getChildNodes(){return m_pChildNodes;};
  int     getOperand(){return m_Operand;}
  virtual void print();
  /** These are some of the interfaces that we need to implements **/
  // Self-print for any non-leaf node
  virtual void printSelf() = 0;
  // Initialization function for any non-leaf node
  virtual void initialize(va_list in_pArguments) = 0;
protected:
  int           m_Operand = -1;
  int           m_iNumOfChildNodes = -1;
  cpBaseNode**  m_pChildNodes = NULL;
};

#define DEFINE_CPLEAFNODE(__node_name,__eNodeKind, __node_value_type)\
class __node_name : public cpBaseNode{\
public:\
  __node_name():cpBaseNode(__eNodeKind){};\
  virtual ~__node_name(){};\
  virtual void print();\
  virtual void initialize(va_list in_pArguments);\
  __node_value_type m_value;\
}

#define DEFINE_CPNORMAL_NODE(__node_name, __eNodeKind)\
class __node_name : public cpNormalNode{\
public:\
  __node_name():cpNormalNode(__eNodeKind){};\
  virtual ~__node_name(){};\
  virtual void initialize(va_list in_pArguments);\
  virtual void printSelf();\
}

DEFINE_CPNORMAL_NODE(cpUnaryExpressionNode,UNARY_EXPRESION_NODE); 
DEFINE_CPNORMAL_NODE(cpBinaryExpressionNode,BINARY_EXPRESSION_NODE);
DEFINE_CPNORMAL_NODE(cpFunctionNode,FUNCTION_NODE); 
DEFINE_CPNORMAL_NODE(cpDeclarationNode,DECLARATION_NODE);
DEFINE_CPNORMAL_NODE(cpIfStatementNode,IF_STATEMENT_NODE);
DEFINE_CPNORMAL_NODE(cpWhileStatmentNode,WHILE_STATEMENT_NODE);
DEFINE_CPNORMAL_NODE(cpConstructorNode,CONSTRUCTOR_NODE);
DEFINE_CPNORMAL_NODE(cpScopeNode,SCOPE_NODE); 
DEFINE_CPNORMAL_NODE(cpVariableNode,VAR_NODE); 
DEFINE_CPNORMAL_NODE(cpAssignmentNode,ASSIGNMENT_NODE);

DEFINE_CPLEAFNODE(cpFloatNode,FLOAT_NODE,float);
DEFINE_CPLEAFNODE(cpIdentifierNode,IDENT_NODE,std::string);
DEFINE_CPLEAFNODE(cpIntNode,INT_NODE,int);
DEFINE_CPLEAFNODE(cpBoolNode,BOOL_NODE, bool);

/** Factory function to create different kind of nodes**/
cpBaseNode* allocate_cpNode(eNodeKind in_nodeKind, ...);
void free_cpNode(cpBaseNode* in_pNode);
void print_cpNode(cpBaseNode* in_pNode);


#endif /* AST_H_ */
