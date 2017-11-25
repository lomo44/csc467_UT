#include "cpIR.h"
#include <cstdio>

std::string gIROpcodeToStringMap[ecpIR_Count] = {
    "CONST_FLOAT",
    "CONST_BOOL",
    "CONST_INT",
    "MOVE",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "EQ",
    "NEQ",
    "GT",
    "LT",
    "GEQ",
    "LEQ",
    "AND",
    "OR",
    "NEG",
    "NOT",
    "POW",
    "BRZ",
    "BR",
    "LIT",
    "RSQ",
    "DP3",
    "SCOPE_START",
    "SCOPE_END"
};


std::string toString(ecpIROpcode in_eOpcode){
    return gIROpcodeToStringMap[in_eOpcode];
}

void cpIRList::print(){
    int size = m_vIRList.size();
    for(int i = 0 ; i < size; i++){
        printf("%s\n",m_vIRList[i]->toIRString().c_str());
    }
}

void cpIRList::insert(cpIR* in_pIR){
    in_pIR->setDst(new cpIRRegister(m_vIRList.size()));
    m_vIRList.push_back(in_pIR);
}

cpIRList::~cpIRList(){
    int size = m_vIRList.size();
    for(int i = 0; i < size; i++){
        delete m_vIRList[i];
    }
}
