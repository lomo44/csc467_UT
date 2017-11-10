#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "symbol.h"


int semantic_check( node *ast);

int semantic_check(cpBaseNode* in_pRootNode);

int semantic_check_function_call(cpBaseNode* in_pNode);
int semantic_check_operator(cpBaseNode* in_pNode);
int semantic_check_condition(cpBaseNode* in_pNode);
int semantic_check_constructor_call(cpBaseNode* in_pNode);
int semantic_check_vector_index(cpBaseNode* in_pNode);
int semantic_check_initialization(cpBaseNode* in_pNode);
int semantic_check_assignment(cpBaseNode* in_pNode);
int semantic_check_variable(cpBaseNode* in_pNode);
int semantic_check_predifined_variable(cpBaseNode* in_pNode);


#endif