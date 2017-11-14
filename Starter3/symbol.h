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

struct cpSymbolTableNode {
    cpSymbolTableNode* m_pParentScope = NULL;
    int m_iNumOfChildScopes = -1;
    std::vector<cpSymbolTableNode*> m_pChildScopes;
    cpSymbolTable m_vSymbolTable;

};

struct FindInsideTable:std::unary_function<cpSymbolAttribute*, bool>
{
   std::string id;
   FindInsideTable(std::string Identifier): id(Identifier){ } 
   bool operator ()(cpSymbolAttribute* const  &attr) const
    {
        return id==attr->m_sIdentifierName;
    }

};

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode);
cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpScopeNode* in_pNode);
cpSymbolAttribute* lookupCurrentSymbolTable(std::string in_sIdentifier, cpScopeNode* in_pNode);



#endif

