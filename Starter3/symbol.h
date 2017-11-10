#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <vector>
#include "ast.h"

struct cpSymbolAttribute{
    std::string m_sIdentifierName;
    int m_iType;
    int m_iAttribute;
};

typedef std::vector<cpSymbolAttribute> cpSymbolTable;


struct cpSymbolTableNode{
    cpSymbolTableNode* m_pParentScope = NULL;
    int m_iNumOfChildScopes = -1;
    cpSymbolTableNode** m_pChileScopes = NULL;
    cpSymbolTable m_vSymbolTable;
};

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode);



#endif

