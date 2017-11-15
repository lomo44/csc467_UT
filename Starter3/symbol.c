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


cpSymbolLookUpTable cp_ST_NodeTable;

void initSymbolAttributeFromDeclarationNode(cpDeclarationNode* in_pNode, cpSymbolAttribute* in_pAttribute){
    in_pAttribute->m_sIdentifierName = in_pNode->m_sIdentifierName;
    in_pAttribute->m_iType = in_pNode->getTerminalType();
    in_pAttribute->m_iVariableSize = in_pNode->m_iVariableSize;
    in_pAttribute->m_bIsConst = in_pNode->m_bIsConst;
}

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode,cpSymbolTableNode* table){
    
    if(in_pNode->getNodeType() == ecpBaseNodeType_Leaf){
        return NULL;
    }
    else{
        
        //create a scope node whenver enters to a new scope
        if (in_pNode->getNodeKind()==SCOPE_NODE)
        {
            cpSymbolTableNode* node= new cpSymbolTableNode();
            node->m_pParentScope=table;
            table=node;
        }
        cp_ST_NodeTable[in_pNode]=table;   
        //insert into symbol table if current node is declaration node
        if (in_pNode->getNodeKind()==DECLARATION_NODE)
        {
            //check if there are duplicate declarations in current scope
            if(lookupSymbolTable(((cpIdentifierNode*)in_pNode)->m_value,in_pNode)!=NULL){
                return NULL;
            }
            else{
                cpSymbolAttribute* new_attribute = new cpSymbolAttribute();
                initSymbolAttributeFromDeclarationNode((cpDeclarationNode*)in_pNode,new_attribute);
                table->m_vSymbolTable.push_back(new_attribute);
            }
        }    

        if (in_pNode->getNodeKind() == IDENT_NODE)
        {
            //check if id is already defined, if null means did not find assignment in scope
            cpSymbolAttribute* attr = lookupSymbolTable(((cpIdentifierNode*)in_pNode)->m_value,in_pNode);
            if ( attr == NULL){
                return NULL;
            }
            else{
                // Node exists, marking the terminal type of this node
                in_pNode->setTerminalType(attr->m_iType);
            } 
        }
        
        //traverse tree, connect child scope table to current scope table
        int num_of_child_nodes = ((cpNormalNode*)in_pNode)->getNumOfChildNodes();
        for (int i=0;i<num_of_child_nodes;i++)
        {
            cpSymbolTableNode* temp = constructSymbolTable(((cpNormalNode*)in_pNode)->getChildNode(i),table);
            if (temp!=table) table->m_pChildScopes.push_back(temp);
        } 
        return table;  
    }    
}

cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpBaseNode* in_pNode)
{
    cpSymbolLookUpTableItor current = cp_ST_NodeTable.find(in_pNode);
    if(current!=cp_ST_NodeTable.end()){
        // Current node exist, try search throught the scope
        return SearchInScope(in_sIdentifier,current->second);
    }
    else{
        return NULL;
    }
}

cpSymbolAttribute* SearchInTable(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode){
    cpSymbolTableNode* current_node = in_pTableNode;
    cpSymbolAttribute* ret = NULL;
    while(current_node != NULL){
        ret = SearchInTable(in_sIdentifier, in_pTableNode);
        if(ret!=NULL){
            break;
        }
        else{
            current_node = current_node->m_pParentScope;
        }
    }
    return ret;
} 
cpSymbolAttribute* SearchInScope(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode){
    int vector_size = in_pTableNode->m_vSymbolTable.size();
    for(int i = 0; i < vector_size; i++){
        if(in_pTableNode->m_vSymbolTable[i]->m_sIdentifierName == in_sIdentifier){
            return in_pTableNode->m_vSymbolTable[i];
        }
    }
    return NULL;
}