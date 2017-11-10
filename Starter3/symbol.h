#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <vector>
#include <unordered_map>
#include "ast.h"

struct cpSymbolAttribute{
    std::string m_sIdentifierName;
    int m_iType;
    int m_iAttribute;
};

typedef std::vector<cpSymbolAttribute*> cpSymbolTable;


struct cpSymbolTableNode{
    cpSymbolTableNode* m_pParentScope = NULL;
    int m_iNumOfChildScopes = -1;
    cpSymbolTableNode** m_pChileScopes = NULL;
    cpSymbolTable m_vSymbolTable;
};
typedef std::unordered_map<cpBaseNode*,cpSymbolTableNode*> cpNodeTable;

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode);
cpSymbolAttribute* getSymbolAttributes(std::string in_sIdentifier, cpScopeNode* in_pNode);


extern cpNodeTable gNodeTable;

#endif

