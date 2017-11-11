#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include <stack>

#include "ast.h"
#include "symbol.h"
#include "parser.tab.h"

typedef std::stack<cpBaseNode*> cpNodeStack;

#define IS_LOGICAL_OPERATOR(x) ((x)==AND || (x)==OR || (x)==EQ || \
                                (x)==NEQ || (x)=='<'|| (x)==LEQ|| \
                                (x)=='>' || (x)==GEQ||)

int semantic_check( node *ast);

int semantic_check(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);

int semantic_check_function_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_operator(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_condition(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_constructor_call(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_vector_index(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_initialization(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_assignment(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);
int semantic_check_predifined_variable(cpScopeNode* in_pNode, cpSymbolTableNode* in_pSymbolTable);


#endif