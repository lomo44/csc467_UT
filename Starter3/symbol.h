#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <vector>
#include <tr1/unordered_map>
#include "ast.h"

struct cpSymbolAttribute{
    std::string m_sIdentifierName;
    ecpTerminalType m_iType;
    bool m_bIsConst;
    int m_iVariableSize;
};

typedef std::vector<cpSymbolAttribute*> cpSymbolTable;

struct cpSymbolTableNode {
    cpSymbolTableNode* m_pParentScope;
    int m_iNumOfChildScopes;
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
cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpBaseNode* in_pNode);

void initSymbolAttributeFromDeclarationNode(cpDeclarationNode* in_pNode, cpSymbolAttribute* in_pAttribute);
cpSymbolAttribute* SearchInTable(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode); 
cpSymbolAttribute* SearchInScope(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode);

typedef std::tr1::unordered_map<cpBaseNode*,cpSymbolTableNode*> cpSymbolLookUpTable;
typedef std::tr1::unordered_map<cpBaseNode*,cpSymbolTableNode*>::iterator cpSymbolLookUpTableItor;

#endif

