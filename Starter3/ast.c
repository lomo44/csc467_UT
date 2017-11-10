#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

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