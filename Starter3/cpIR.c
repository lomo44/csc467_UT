#include "cpIR.h"
#include <cstdio>

std::string gIROpcodeToStringMap[ecpIR_Count] = {
    "CONST_FLOAT",
    "CONST_BOOL",
    "CONST_INT",
    "MOVE",
    "ADD",
    "MUL",
    "DIV",
    "CMP",
    "GT",
    "LT",
    "GEQ",
    "LEQ",
    "AND",
    "OR",
    "NEG",
    "NOT",
    "POW",
    "FMOVE",
    "TMOVE"
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
void cpPrintRI(cpIRList& in_vlist){
    int size = in_vlist.size();
    for(int i = 0 ; i < size; i++){
        printf("%s\n",in_vlist[i]->toRIString().c_str());
    }
}