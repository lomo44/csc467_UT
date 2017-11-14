#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"


/*************************************************************
  construct a symbol table                   
  assign terminal value to identifier node   
  variable semantic check:                                               
      
      1.Every variable must be declared before it is used. 
        A variable’s declaration must appear either within 
        the current scope or an enclosing scope.---CHECKED
      
      2.A variable can only be declared once within the 
        same scope. --CHECKED
      
      3.If a variable is const qualified then it’s value
        cannot be re-assigned. --NOT CHECKED
      
      4. One variable can shadow another variable. --CHECKED
      
      5.Ensure that every variable has been assigned a value 
        before being read. --NOT CHECKED

************************************************************/


std::unordered_map<cpBaseNode*,cpSymbolTableNode*> cp_ST_NodeTable;


cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode,cpSymbolTableNode* table){
    
    //create a scope node whenver enters to a new scope
    if (in_pNode->getNodeKind()==SCOPE_NODE)
    {
    cpSymbolTableNode* node= new cpSymbolTableNode();
    node->m_pParentScope=table;
    table=node;
    }
    
    cp_ST_NodeTable[in_pNode]=table;
    
    //insert into symbol table if current node is declaration node
    if (in_pNode->getNodeKind()==ASSIGNMENT_NODE)
    {
        cpBaseNode* parent = in_pNode->getParentNode();
        if (parent->getNodeKind()==DECLARATION_NODE)
        {
            //check if there are duplicate declarations in current scope
            if(lookupSymbolTable((cpIdentifierNode*)in_pNode->m_value, cpScopeNode* in_pNode)==NULL)
            return NULL;
            //insert identifier and type
            table->m_vSymbolTable.push_back(new cpSymbolAttribute());
            if ((cpNormalNode*)in_pNode->getNumOfChildNodes()<4)
            { 
                table->back()->m_iType=(cpNormalNode*)in_pNode->getChildNode(0)->getNodeKind();//substitute with type node value later
                table->back()->m_sIdentifierName=(cpIdentifierNode*)((cpNormalNode*)in_pNode->getChildNode(1))->m_value;
            }
            //we need to initialize 'const' child node for declaration
            //if ((cpNormalNode*)in_pNode->getNumOfChildNodes()=4)
        }
        
    }

    if (in_pNode->getNodeKind() == IDENT_NODE)
    {
        //check if id is already defined, if null means did not find assignment in scope
        cpSymbolAttribute* attr=lookupSymbolTable((cpIdentifierNode*)in_pNode->m_value, cpScopeNode* in_pNode)
        if ( attr == NULL) return NULL;
        else in_pNode->setTerminalType(attr->m_iType);
    }
    
    //traverse tree, connect child scope table to current scope table
    for (int i=0;i<in_pNode->getNumOfChildNodes();i++)
    {
        cpSymbolTableNode* temp = constructSymbolTable(in_pNode->getChildNode(i),table);
        if (temp!=table) table->m_pChildScopes.push_back(temp);
    } 
    return table;  
}

cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpScopeNode* in_pNode)
{
    cpSymbolTableNode* current = cp_ST_NodeTable.find(in_pNode);
    cpSymbolTable table = current->m_vSymbolTable;
    while(current!=NULL)
    {   
        cpSymbolTable it=std::find_if(table.begin(), table.end(), FindInsideTable(in_sIdentifier))
        if (it != table.end()) return *it;
        else current = current->m_pParentScope;
    }
    return NULL;
    
}

cpSymbolAttribute* lookupCurrentSymbolTable(std::string in_sIdentifier, cpScopeNode* in_pNode){
    
    cpSymbolTableNode* current = cp_ST_NodeTable.find(in_pNode);
    cpSymbolTable table = current->m_vSymbolTable;
    cpSymbolTable it=std::find_if(table.begin(), table.end(), FindInsideTable(in_sIdentifier))
    if (it != table.end()) return *it;
    else return NULL;
    
}
