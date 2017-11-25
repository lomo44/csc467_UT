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

void cpPrintIR(cpIRList& in_vlist){
    int size = in_vlist.size();
    for(int i = 0 ; i < size; i++){
        printf("%s\n",in_vlist[i]->toIRString().c_str());
    }
}

void cpInsertInList(cpIR* in_pIR, cpIRList& in_List){
    in_pIR->setDst(new cpIRRegister(in_List.size()));
    in_List.push_back(in_pIR);
}

void cpFreeIRList(cpIRList& in_List){
    int size = in_List.size();
    for(int i = 0; i < size; i++){
        delete in_List[i];
    }
}