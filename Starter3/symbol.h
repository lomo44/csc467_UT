#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <vector>
#include <tr1/unordered_map>
#include "ast.h"

class cpSymbolAttribute{
public:
    cpSymbolAttribute(){};
    cpSymbolAttribute(std::string in_sName, ecpTerminalType in_eType, ecpFunctionQualifier in_eQualifier, int in_iIsWrite) : m_sIdentifierName(in_sName)
                                                                                                          , m_iType(in_eType)
                                                                                                          , m_eQualifier(in_eQualifier)
                                                                                                          , m_iVariableSize(-1)
                                                                                                          , m_isWrite(in_iIsWrite){};
    void print();
    std::string m_sIdentifierName;
    ecpTerminalType m_iType;
    ecpFunctionQualifier m_eQualifier;
    int m_iVariableSize;
    int m_isWrite;
};

typedef std::vector<cpSymbolAttribute*> cpSymbolTable;

class cpSymbolTableNode {
public:
    cpSymbolTableNode():m_pParentScope(NULL){};
    virtual ~cpSymbolTableNode();
    void print();
    cpSymbolTableNode* m_pParentScope;
    std::vector<cpSymbolTableNode*> m_pChildScopes;
    cpSymbolTable m_vSymbolTable;
};

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode,cpSymbolTableNode* table);
cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpBaseNode* in_pNode);

void initSymbolAttributeFromDeclarationNode(cpDeclarationNode* in_pNode, cpSymbolAttribute* in_pAttribute,int i);
cpSymbolAttribute* SearchInTable(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode); 
cpSymbolAttribute* SearchInScope(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode);

typedef std::tr1::unordered_map<cpBaseNode*,cpSymbolTableNode*> cpSymbolLookUpTable;
typedef std::tr1::unordered_map<cpBaseNode*,cpSymbolTableNode*>::iterator cpSymbolLookUpTableItor;
typedef std::tr1::unordered_map<std::string,cpIRRegister*> cpSymbolIRLookUpTable;
typedef cpSymbolIRLookUpTable::iterator cpSymbolIRLookUpTableItor;

extern cpSymbolTableNode* gSymbolTable;
extern cpSymbolIRLookUpTable gSymbolIRLookUpTable;;
#endif

