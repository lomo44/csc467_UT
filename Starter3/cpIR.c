#include "cpIR.h"
#include <cstdio>
#include <limits>

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
    "SCOPE_END",
    "CMP"
};

std::string gRegisterMaskToString[ecpRegister_Count] = {
    ".x",
    ".y",
    ".z",
    ".w"
};

std::string toString(ecpIROpcode in_eOpcode){
    return gIROpcodeToStringMap[in_eOpcode];
}

std::string toString(ecpRegisterMask in_eMask){
    return gRegisterMaskToString[in_eMask];
}


void cpIRRegister::updateInterferenceSet(cpRegisterSet& in_rLiveSet){
    /** Check the live set, and move any unique register into the live set
     **/
    cpRegisterSetItor itor = in_rLiveSet.begin();
    while(itor != in_rLiveSet.end()){
        if(*itor != this && m_InterferenceSet.find(*itor)==m_InterferenceSet.end()){
            m_InterferenceSet.insert(*itor);
        }
        ++itor;
    }
}

std::string cpRegisterSet::toString(){
    std::string ret = "{";
    if(!empty()){
        auto itor = begin();
        ret += std::to_string((*itor)->m_iIRID);
        ++itor;
        while(itor!=end()){
            ret += ",";
            ret += std::to_string((*itor)->m_iIRID);
            ++itor;    
        }
    }
    ret+="}";
    return ret;
}

void cpIRList::print(){
    int size = m_vIRList.size();
    for(int i = 0 ; i < size; i++){
        printf("%s\n",m_vIRList[i]->toIRString().c_str());
        // std::string ret = " {";
        // for (int j = 0; j < m_vIRList[i]->getDlist().size(); j++){
        //         ret+= " ";
        //         ret += std::to_string(m_vIRList[m_vIRList[i]->getDlist()[j]]->getDst()->m_realID);
        // }
        // ret +=" }";
        // printf("%s\n", ret.c_str());
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
            cpIR* notCondition = new cpIR(ecpIR_LRP,currentCondition,getConst(0),getConst(1));
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


void cpIRList::generateDependency(){
    int start_index = m_vIRList.size()-1;
    cpIR* previous = NULL;

    while(start_index>=0){
        m_vIRList[start_index]->generateLiveSet(previous);
        cpRegisterSetItor itor = m_vIRList[start_index]->getLiveSet().begin();
        cpRegisterSetItor end = m_vIRList[start_index]->getLiveSet().end();
        while(itor!=end){
            (*itor)->updateInterferenceSet(m_vIRList[start_index]->getLiveSet());
            ++itor;
        }
        previous = m_vIRList[start_index];
        start_index--;
    }
    /*****************some test******************************
    for(int i =0;i<=size;i++) {
        printf("neigbour num of %d: %d\n",i,reg_neighbouring[i]);}
    *********************************************************/
}

void cpIRList::registerMapping(){
    /**
     * Since we do not do any re ordering, we'll to the register remapping in the 
     * most minimal listic way
     */ 

    // Finding the chromatic number by iterate through every instruction's output register
    int chromatic_number = -1;
    int IR_Count = m_vIRList.size();
    int current_number = 0;
    for(int i = 0; i < IR_Count; i++){
        current_number = m_vIRList[i]->getDst()->m_InterferenceSet.size(); 
        if(current_number > chromatic_number){
            chromatic_number = current_number;
        }
    }

    // Work our way up from here
    std::tr1::unordered_set<int> current_set;
    cpIRRegister* current_r;
    for(int i = 0; i < IR_Count;i++){
        current_r = m_vIRList[i]->getDst();
        cpRegisterSetItor start = current_r->m_InterferenceSet.begin();
        cpRegisterSetItor end = current_r->m_InterferenceSet.end();
        // Iterate through the entire inteference set and find all the interferences
        while(start!=end){
            int color = (*start)->m_iColor;
            if(color >= 0 && current_set.find(color) == current_set.end()){
                // new interference color, insert
                current_set.insert(color);
            }
            ++start;
        }
        int assigned_color = -1;
        for(int j = 0; j < chromatic_number; j++){
            if(current_set.find(j)==current_set.end()){
                assigned_color = j;
                break;
            }
        }
        if(assigned_color == -1){
            // we ran out of chromatic number, need to assign one
            assigned_color = chromatic_number;
            chromatic_number++;
        }
        current_r->m_iColor = assigned_color;
        current_set.clear();
    }
    /*************************some test**************************
    printf("order of coloring:");
    for (int i = 0; i<m_coloringOrder.size(); i++)
        printf(" %d:%d",m_coloringOrder[i],m_vIRList[m_coloringOrder[i]]->getDst()->m_realID);
    printf("\n");
    *************************************************************/
}

void cpIR::generateLiveSet(cpIR* in_pPreviousIR){
    if(in_pPreviousIR!=NULL){
        /**
         * If previous IR has an live set, then propagate the live set
         * */
        cpRegisterSetItor itor = in_pPreviousIR->getLiveSet().begin();
        cpRegisterSetItor end = in_pPreviousIR->getLiveSet().end();
        while(itor != end){
            if(m_LiveSet.find(*itor)==m_LiveSet.end()){
                /**
                 * Item is not in the current live set, insert it.
                 **/
                m_LiveSet.insert(*itor);
            }
            ++itor;
        }
    }
    // remove genset
    if(m_LiveSet.find(m_Dst)!=m_LiveSet.end() && !m_Dst->isPredifined()){
        /** output is in the current live set, remove it
         **/
        m_LiveSet.erase(m_Dst);
    }
    // Add sources to live set
    if(m_SrcA!=NULL && m_LiveSet.find(m_SrcA)== m_LiveSet.end() && !m_SrcA->isPredifined()){
        m_LiveSet.insert(m_SrcA);
    }
    if(m_SrcB!=NULL && m_LiveSet.find(m_SrcB)== m_LiveSet.end() && !m_SrcB->isPredifined()){
        m_LiveSet.insert(m_SrcB);
    }
    if(m_SrcC!=NULL && m_LiveSet.find(m_SrcC)== m_LiveSet.end() && !m_SrcC->isPredifined()){
        m_LiveSet.insert(m_SrcC);
    }
}

cpIRRegister* cpIRList::getConst(float in_fIn){
    if(m_ConstFloatMap.find(in_fIn)==m_ConstFloatMap.end()){
        cpIR_CONST_F* in_const = new cpIR_CONST_F();
        in_const->setScalar(in_fIn);
        insert(in_const);
        m_ConstFloatMap[in_fIn] = in_const->getDst();
    }
    return m_ConstFloatMap[in_fIn];
}

cpIRRegister* cpIRList::getConst(int in_iIn){
    if(m_ConstIntMap.find(in_iIn)==m_ConstIntMap.end()){
        cpIR_CONST_I* in_const = new cpIR_CONST_I();
        in_const->setScalar(in_iIn);
        insert(in_const);
        m_ConstIntMap[in_iIn] = in_const->getDst();
    }
    return m_ConstIntMap[in_iIn];
}