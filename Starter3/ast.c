#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"


#define DEBUG_PRINT_TREE 0

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast);
  ast->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  
  // ...

  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);
    break;

  // ...

  default: break;
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

}

void ast_print(node * ast) {

}

void cpNormalNode::initChildNodes(int in_iNumOfNodes){
  if(in_iNumOfNodes > 0 && m_pChildNodes==NULL){
    m_iNumOfChildNodes = in_iNumOfNodes;
    m_pChildNodes = new cpBaseNode*[in_iNumOfNodes];
    // Initialize child nodes ptr
    for(int i = 0; i < in_iNumOfNodes; i++){
      m_pChildNodes[i] = NULL;
    }
  }
}

cpNormalNode::~cpNormalNode(){
  if(m_iNumOfChildNodes > 0 && m_pChildNodes!=NULL){
    // Starting calling constructor of the child nodes
    for(int i = 0; i < m_iNumOfChildNodes; i++){
      delete m_pChildNodes[i];
    }
    delete []m_pChildNodes;
  }
}

cpBaseNode* cpNormalNode::getChildNode(int in_iNodeIndex){
  if(m_pChildNodes!=NULL && in_iNodeIndex >= 0){
    return m_pChildNodes[in_iNodeIndex];
  }
  return NULL;
}

void cpNormalNode::print(){
  // In order print
  printSelf();
  if(m_iNumOfChildNodes > 0 && m_pChildNodes != NULL){
    for(int i = 0; i < m_iNumOfChildNodes; i ++){
      m_pChildNodes[i]->print();
    }
  }
}

void print_cpNode(cpNormalNode* in_pNode){
  in_pNode->print();
}

void free_cpNode(cpBaseNode* in_pNode){
  delete in_pNode;
}


void cpBinaryExpressionNode::printSelf(){
  printf("Binary Expresion Node, Operand %d\n",m_Operand);
}
void cpBinaryExpressionNode::initialize(va_list in_pArguments){
  // TODO:Inplement initialize function for binary expression node
}
bool cpBinaryExpressionNode::isTerminalType(node_kind in_kind){
}


void cpUnaryExpressionNode::printSelf(){
  printf("Unary Expression Node, Operand %d\n",m_Operand);
}
void cpUnaryExpressionNode::initialize(va_list in_pArguments){
  // TODO:Inplement initialize function for UnaryExpressionNode
}
bool cpUnaryExpressionNode::isTerminalType(node_kind in_kind){
  if(m_iNumOfChildNodes == 1 && m_pChildNodes!=NULL){
    return m_pChildNodes[0]->isTerminalType(in_kind);
  }
  else{
    assert(0);
    return false;
    // Construction of the unary expression node is invalid
  }
}

#define CHECK_AND_ALLOCATE(__kind, __node_class)\
case __kind:{\
  retNode = new __node_class();\
  break;\
}

cpBaseNode* allocate_cpNode(node_kind in_nodekind, ...){
  va_list args;
  va_start(args, in_nodekind);
  cpBaseNode* retNode = NULL;
  switch(in_nodekind){
    CHECK_AND_ALLOCATE(BINARY_EXPRESSION_NODE,cpBinaryExpressionNode);
    CHECK_AND_ALLOCATE(UNARY_EXPRESION_NODE,cpUnaryExpressionNode);
    //TODO: Add more under here, for each node, please implement the printSelf and Initialize interface
    default:{
    }
  }
  retNode->initialize(args);
  va_end(args);
  return retNode;
}