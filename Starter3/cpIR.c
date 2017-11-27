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
    "SLT",
    "SGE",
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
    "LRP",
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

cpIRRegister* cpIRList::insert(cpIR* in_pIR){
    // Check if this is within a loop
    if(in_pIR->getOpCode() == ecpIR_MOVE){
        if(!m_IfConditionStack.empty()){
            cpIRRegister* currentCondition = m_IfConditionStack.top();
            cpIRRegister* dst = in_pIR->getSrcA();
            cpIRRegister* src = in_pIR->getSrcB();
            /***********replace not with LRT***************/
            cpIRRegister * condition =  new cpIRRegister(*currentCondition);
            condition->not_();
            cpIR_CONST_B* bool0 = new cpIR_CONST_B();
            bool0->setScalar(0);
            cpIR_CONST_B* bool1 = new cpIR_CONST_B();
            bool1->setScalar(1);
            cpIR* notCondition = new cpIR(ecpIR_LRP,condition,insert(bool0),insert(bool1));
            cpIRRegister* negCon = insert(notCondition);
            /***********************************************/
            cpIR* src_sum = new cpIR(ecpIR_MUL,src,currentCondition);
            cpIR* dst_sum = new cpIR(ecpIR_MUL,dst,negCon);
            cpIR* total_sum = new cpIR(ecpIR_ADD, insert(src_sum),insert(dst_sum));
            cpIRRegister* sum = insert(total_sum);
            cpIR* output = new cpIR(ecpIR_MOVE,dst,sum);
            output->setDst(new cpIRRegister(m_vIRList.size()));
            m_vIRList.push_back(output);
            delete in_pIR;
            return output->getDst();
        }
    }
    if (in_pIR->getOpCode() == ecpIR_NEG) 
        in_pIR->setOpCode(ecpIR_MOVE);
    in_pIR->setDst(new cpIRRegister(m_vIRList.size()));
    m_vIRList.push_back(in_pIR);
    return in_pIR->getDst();
}

cpIRList::~cpIRList(){
    int size = m_vIRList.size();
    for(int i = 0; i < size; i++){
        delete m_vIRList[i];
    }
}

void cpIRList::pushIfCondition(cpIRRegister* in_pCondition){
    if(!m_IfConditionStack.empty()){
        // And with current condition and push it in
        cpIRRegister* currentCondition = m_IfConditionStack.top();
        cpIR* newIR = new cpIR(ecpIR_AND,currentCondition,in_pCondition);
        insert(newIR);
        m_IfConditionStack.push(newIR->getDst());
    }
    else{
        m_IfConditionStack.push(in_pCondition);
    }
}

void cpIRList::popIfCondition(){
    m_IfConditionStack.pop();
}