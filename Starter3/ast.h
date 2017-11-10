
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <string>

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

typedef enum {
  INVALID = -1,
  UNKNOWN = 0,

  SCOPE_NODE = (1 << 0),

  EXPRESSION_NODE = (1 << 2),
  UNARY_EXPRESION_NODE = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE = (1 << 2) | (1 << 4),
  INT_NODE = (1 << 2) | (1 << 5),
  FLOAT_NODE = (1 << 2) | (1 << 6),
  IDENT_NODE = (1 << 2) | (1 << 7),
  VAR_NODE = (1 << 2) | (1 << 8),
  FUNCTION_NODE = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE = (1 << 2) | (1 << 10),
  BOOL_NODE = (1<<2) | (1<<11),

  STATEMENT_NODE = (1 << 1),
  IF_STATEMENT_NODE = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE = (1 << 1) | (1 << 14),

  DECLARATION_NODE = (1 << 15)
} node_kind;

struct node_
{

  // an example of tagging each node with a type
  node_kind kind;

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

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node *ast);


enum ecpVisitorTraversalType{
  ecpVisitorTraversalType_InOrder,
  ecpVisitorTraversalType_PostOrder
};

enum ecpBaseNodeType{
  ecpBaseNodeType_Normal,
  ecpBaseNodeType_Leaf
};

class cpBaseNode{
public:
  cpBaseNode(){};
  cpBaseNode(node_kind in_NodeKind):m_NodeKind(in_NodeKind){};
  virtual ~cpBaseNode(){};
  void setNodeType(ecpBaseNodeType in_eNodeType){m_eNodeType = in_eNodeType;};
  ecpBaseNodeType getNodeType(){return m_eNodeType;};
  /** Interfaces that we need to implements **/
  // Initialize a node, this node could be a leaf node or a non-leaf node, 
  // But they should share the same interface
  virtual void initialize(va_list in_pArguments) = 0;
  // Print function for each node
  // For non-leaf node, we need to implement the printSelf() function to print
  // its functionality. For leaf node, overwrite this to print the value or type
  virtual void print() = 0;

  virtual bool isTerminalType(node_kind in_Kind){return m_TerminalKind == in_Kind;};
  virtual void setTerminalType(node_kind in_Kind){m_TerminalKind = in_Kind;};
  virtual node_kind getTerminalType(){return m_TerminalKind;};
protected:
  node_kind     m_TerminalKind = UNKNOWN;
  node_kind     m_NodeKind;
  ecpBaseNodeType m_eNodeType;
};

class cpNormalNode : public cpBaseNode
{
public:
  cpNormalNode(){};
  cpNormalNode(node_kind in_NodeKind):cpBaseNode(in_NodeKind){};
  virtual ~cpNormalNode();
  void    initChildNodes(int in_iNumOfNodes);
  int     getNumOfChildNodes(){return m_iNumOfChildNodes;};
  void    setChildNodes(cpNormalNode* in_pNode, int in_iNodeIndex);
  cpBaseNode* getChildNode(int in_iNodeIndex);
  cpBaseNode** getChildNodes(){return m_pChildNodes;};
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

#define DEFINE_CPLEAFNODE(__node_name,__node_kind, __node_value_type)\
class __node_name : public cpBaseNode{\
public:\
  __node_name():cpBaseNode(__node_kind){};\
  virtual ~__node_name(){};\
  virtual void print();\
  virtual void initialize(va_list in_pArguments);\
  __node_value_type m_value;\
}

#define DEFINE_CPNORMAL_NODE(__node_name, __node_kind)\
class __node_name : public cpNormalNode{\
public:\
  __node_name():cpNormalNode(__node_kind){};\
  virtual ~__node_name(){};\
  virtual void initialize(va_list in_pArguments);\
  virtual void printSelf();\
}

DEFINE_CPNORMAL_NODE(cpStatementNode,STATEMENT_NODE);
DEFINE_CPNORMAL_NODE(cpExpressionNode,EXPRESSION_NODE);
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
cpBaseNode* allocate_cpNode(node_kind in_nodeKind, ...);
void free_cpNode(cpBaseNode* in_pNode);
void print_cpNode(cpBaseNode* in_pNode);

#endif /* AST_H_ */
