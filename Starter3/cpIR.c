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
    "SCOPE_END"
};



std::string toString(ecpIROpcode in_eOpcode){
    return gIROpcodeToStringMap[in_eOpcode];
}

void cpIRList::print(){
    int size = m_vIRList.size();
    for(int i = 0 ; i < size; i++){
        printf("%s",m_vIRList[i]->toIRString().c_str());
        std::string ret = " {";
        for (int j = 0; j < m_vIRList[i]->getDlist().size(); j++){
                ret+= " ";
                ret += std::to_string(m_vIRList[m_vIRList[i]->getDlist()[j]]->getDst()->m_realID);
        }
        ret +=" }";
        printf("%s\n", ret.c_str());
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


void cpIRList::genDependency(){
    int size = m_vIRList.size()-1;
    reg_neighbouring = new int[size+1]();
    cpDependencylist live;
    live.push_back(m_vIRList[size]->getDst()->m_iIRID);
    for(int i = size ; i >= 0; i--){
        if (m_vIRList[i]->getOpCode() == ecpIR_MOVE) 
        {
            reg_neighbouring[i]=-1;
        }
        live = m_vIRList[i]->generateLiveset(live);
        for(int j = 0; j<live.size();j++)
        {
            cpIRRegister* curr_v = m_vIRList[live[j]]->getDst();
            for(int k = j+1; k < live.size();k++)
            {
                cpIRRegister* neighbour_v = m_vIRList[live[k]]->getDst();
                neighbour_v->m_neighbours.push_back(live[j]);
                curr_v->m_neighbours.push_back(live[k]);
            }
        }
        
    }
    for(int i = size ; i >= 0; i--){
        if (reg_neighbouring[i]!=-1){
            cpDependencylist* temp = &(m_vIRList[i]->getDst()->m_neighbours);
            std::sort(temp->begin(),temp->end());
            temp->erase(std::unique(temp->begin(),temp->end()),temp->end());
            reg_neighbouring[i]+=temp->size();
        }
    }
    /*****************some test******************************
    for(int i =0;i<=size;i++) {
        printf("neigbour num of %d: %d\n",i,reg_neighbouring[i]);}
    *********************************************************/
}

void cpIRList::color_ordering(){
    int size = m_vIRList.size();
    cpIRID min_v;
    cpDependencylist adj_list;
    int min_edge; std::numeric_limits<int>::max();
    int curr_edge;
    bool find_min; 
    while (true){
        min_edge = std::numeric_limits<int>::max();
        find_min = false;
        for (int i = 0; i<size; i++)
        {
            curr_edge = reg_neighbouring[i];
            if((curr_edge > 0) && (curr_edge < min_edge)){
                min_v = i;
                min_edge = curr_edge;
                find_min = true;
            }
        }
        if(find_min){
            m_coloringOrder.push_back(min_v);
            m_vIRList[min_v]->getDst()->m_realID = -1;
            reg_neighbouring[min_v]=-1;
            adj_list = m_vIRList[min_v]->getDst()->m_neighbours;
            for (int j = 0; j<adj_list.size(); j++){
                cpIRID adj_v = adj_list[j];
                if (reg_neighbouring[adj_v] != -1 )
                    reg_neighbouring[adj_v] -= 1;
                if (reg_neighbouring[adj_v] == 0) {
                    m_coloringOrder.push_back(adj_v);
                    m_vIRList[adj_v]->getDst()->m_realID = -1;
                    reg_neighbouring[adj_v]=-1;
                }
                else if (reg_neighbouring[adj_v] < -1) printf("Error on color-ordering");
            }
        }
        else break;
    }
    /*************************some test**************************
    printf("order of coloring:");
    for (int i = 0; i<m_coloringOrder.size(); i++)
        printf(" %d:%d",m_coloringOrder[i],m_vIRList[m_coloringOrder[i]]->getDst()->m_realID);
    printf("\n");
    *************************************************************/
}

void cpIRList::regRename(){
    cpIRID cur_vid;
    cpIRID neighbour_vid;
    cpIRRegister* cur_v;
    cpIRRegister* neighbour_v;
    cpIRID maxid; 
    for (int i = m_coloringOrder.size()-1; i>=0; i--){
        cur_vid = m_coloringOrder[i];
        maxid = -1;
        cur_v = m_vIRList[cur_vid]->getDst();
        for(int j = 0; j<cur_v->m_neighbours.size(); j++)
        {
            neighbour_vid = cur_v->m_neighbours[j];
            neighbour_v = m_vIRList[neighbour_vid]->getDst();
            if (neighbour_v->m_realID > maxid) maxid = neighbour_v->m_realID;
        }
        //we can refine it not to assign max reg name +1 everytime by using reg mapping instead
        maxid += 1;
        cur_v->m_realID = maxid;
    }
    printf("after coloring:");
    for (int i = 0; i<m_coloringOrder.size(); i++)
        printf(" %d:%d",m_coloringOrder[i],m_vIRList[m_coloringOrder[i]]->getDst()->m_realID);
    printf("\n");

}


cpDependencylist cpIR::generateLiveset(cpDependencylist liveset){
    int def = m_Dst->m_iIRID;
    liveset.erase(std::remove(liveset.begin(), liveset.end(), def), liveset.end());
    if (m_SrcA != NULL) {m_Dependencylist.push_back(m_SrcA->m_iIRID);}
    if (m_SrcB != NULL) {m_Dependencylist.push_back(m_SrcB->m_iIRID);}
    if (m_SrcC != NULL) {m_Dependencylist.push_back(m_SrcC->m_iIRID);}
    if(!liveset.empty()){
        for (int i = 0 ; i < liveset.size(); i++) 
        {
            m_Dependencylist.push_back(liveset[i]);
        }
    }
    std::sort(m_Dependencylist.begin(),m_Dependencylist.end());
    m_Dependencylist.erase(std::unique(m_Dependencylist.begin(),m_Dependencylist.end()),m_Dependencylist.end());
    return m_Dependencylist;
}