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

cpSymbolLookUpTable gSymbolLookUpTable;
cpSymbolIRLookUpTable gSymbolIRLookUpTable;
cpSymbolTableNode* gSymbolTable = NULL;

void cpSymbolAttribute::print(){
    printf("[Symbol] Name: %s,Type: %s, Qualifier: %s\n",
        m_sIdentifierName.c_str(),
        toString(m_iType).c_str(),
        toString(m_eQualifier).c_str()
    );
}

cpSymbolTableNode::~cpSymbolTableNode(){
    int num_of_child_nodes = m_pChildScopes.size();
    for(int i = 0; i < num_of_child_nodes;i++){
        delete m_pChildScopes[i];
    }
}

void cpSymbolTableNode::print(){
    printf("SymbolTableNode: \n");
    int attribute_count = m_vSymbolTable.size();
    for(int i = 0; i < attribute_count;i++){
        m_vSymbolTable[i]->print();
    }
    int num_of_child_nodes = m_pChildScopes.size();
    for(int i = 0; i < num_of_child_nodes;i++){
        m_pChildScopes[i]->print();
    }
}

void initSymbolAttributeFromDeclarationNode(cpDeclarationNode* in_pNode, cpSymbolAttribute* in_pAttribute,int i){
    in_pAttribute->m_sIdentifierName = in_pNode->m_sIdentifierName;
    in_pAttribute->m_iType = in_pNode->m_eTargetType;
    in_pAttribute->m_iVariableSize = in_pNode->m_iVariableSize;
    in_pAttribute->m_eQualifier = in_pNode->m_eQualifier;
    in_pAttribute->m_isWrite = i;
}

cpSymbolTableNode* constructSymbolTable(cpBaseNode* in_pNode,cpSymbolTableNode* table){
    if(gSymbolTable == NULL){
        // pre-defined symbol table is not there, create one
        cpSymbolTableNode* node = new cpSymbolTableNode();
        node->m_pParentScope = NULL;
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_FragColor",ecpTerminalType_float4,ecpFunctionQualifier_Result));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_FragDepth",ecpTerminalType_bool1,ecpFunctionQualifier_Result));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_FragCoord",ecpTerminalType_float4,ecpFunctionQualifier_Result));

        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_TexCoord",ecpTerminalType_float4,ecpFunctionQualifier_Attribute));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_Color",ecpTerminalType_float4,ecpFunctionQualifier_Attribute));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_Secondary",ecpTerminalType_float4,ecpFunctionQualifier_Attribute));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_FogFragCoord",ecpTerminalType_float4,ecpFunctionQualifier_Attribute));

        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_Light_Half",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_Light_Ambient",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("gl_Material_Shininess",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("env1",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("env2",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        node->m_vSymbolTable.push_back(new cpSymbolAttribute("env3",ecpTerminalType_float4,ecpFunctionQualifier_Uniform));
        gSymbolTable = node;
        table = node;
    }
    if(in_pNode==NULL){
        return NULL;
    }
    if(in_pNode->getNodeType() == ecpBaseNodeType_Leaf && in_pNode->getNodeKind()!=IDENT_NODE){
        return NULL;
    }
    else{
        //create a scope node whenver enters to a new scope
        gSymbolLookUpTable[in_pNode]=table;
        cpSymbolTableNode* ret = NULL;
        if (in_pNode->getNodeKind() == IDENT_NODE)
        {
            //check if id is already defined, if null means did not find assignment in scope
            cpSymbolAttribute* attr = lookupSymbolTable(((cpIdentifierNode*)in_pNode)->m_value,in_pNode);
            if (attr == NULL){
                in_pNode->setTerminalType(ecpTerminalType_Invalid);
            }
            else{
                // Node exists, marking the terminal type of this node
                in_pNode->setTerminalType(attr->m_iType);
            } 
        }
        else{
            if (in_pNode->getNodeKind()==SCOPE_NODE)
            {
                cpSymbolTableNode* node= new cpSymbolTableNode();
                node->m_pParentScope=table;
                table->m_pChildScopes.push_back(node);
                table=node;
                ret = node;
            }        
            //printf("Size: %d\n",gSymbolLookUpTable.size());   
            //insert into symbol table if current node is declaration node
            if (in_pNode->getNodeKind()==DECLARATION_NODE)
            {
                //check if there are duplicate declarations in current scope
                if(SearchInScope(((cpDeclarationNode*)in_pNode)->m_sIdentifierName,table)==NULL){
                    cpSymbolAttribute* new_attribute = new cpSymbolAttribute();
                    if(((cpDeclarationNode*)in_pNode)->getChildNode(0)!=NULL) 
                        initSymbolAttributeFromDeclarationNode((cpDeclarationNode*)in_pNode,new_attribute,1);
                    else
                        initSymbolAttributeFromDeclarationNode((cpDeclarationNode*)in_pNode,new_attribute,0);
                    table->m_vSymbolTable.push_back(new_attribute);
                }
                else{
                    in_pNode->setTerminalType(ecpTerminalType_Invalid);
                }
            }    
            //traverse tree, connect child scope table to current scope table
            int num_of_child_nodes = ((cpNormalNode*)in_pNode)->getNumOfChildNodes();
            for (int i=0;i<num_of_child_nodes;i++)
            {
                constructSymbolTable(((cpNormalNode*)in_pNode)->getChildNode(i),table);
            } 
        }    
        return ret;  
    }    
}

cpSymbolAttribute* lookupSymbolTable(std::string in_sIdentifier, cpBaseNode* in_pNode)
{
    cpSymbolLookUpTableItor current = gSymbolLookUpTable.find(in_pNode);
    if(current!=gSymbolLookUpTable.end()){
        // Current node exist, try search throught the scope
        return SearchInTable(in_sIdentifier,current->second);
    }
    else{
        return NULL;
    }
}

cpSymbolAttribute* SearchInTable(const std::string& in_sIdentifier, cpSymbolTableNode* in_pTableNode){
    cpSymbolTableNode* current_node = in_pTableNode;
    cpSymbolAttribute* ret = NULL;
    while(current_node != NULL){
        ret = SearchInScope(in_sIdentifier, current_node);
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