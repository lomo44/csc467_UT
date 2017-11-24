#ifndef _cpIR_h
#define _cpIR_h
#include <string>
#include <vector>
#include <sstream>
// Define some of the opcode of the ir
typedef int cpIRID;


enum ecpIROpcode{
    ecpIR_CONST_F,
    ecpIR_CONST_B,
    ecpIR_CONST_I,
    ecpIR_MOVE,
    ecpIR_ADD,
    ecpIR_SUB,
    ecpIR_MUL,
    ecpIR_DIV,
    ecpIR_CMP,
    ecpIR_GT,
    ecpIR_LT,
    ecpIR_GEQ,
    ecpIR_LEQ,
    ecpIR_AND,
    ecpIR_OR,
    ecpIR_NEG,
    ecpIR_NOT,
    ecpIR_POW,       
    ecpIR_FMOVE, // dst srcA srcB: srcA==false?dst=srcB.
    ecpIR_TMOVE, // dst srcA srcB: srcA==true?dst=srcB.
    ecpIR_Count
};
// Class for IR
/**
 * Use ptr for src ID so that when we change the dstID of the depended IR the src id will change according ly 
 * */
std::string toString(ecpIROpcode in_eIROpcode);
class cpIR{
public:
    cpIR();
    cpIR(ecpIROpcode in_eIROpcode, cpIRID* in_iSrcAID, cpIRID* in_iSrcBID): m_pSrcAID(in_iSrcBID),
                                                                            m_pSrcBID(in_iSrcAID),
                                                                            m_eOpcode(in_eIROpcode){};
    virtual ~cpIR();
    virtual std::string toIRString(){
        return toString(m_eOpcode)+" "+std::to_string(m_iDstID)+
                                   " "+std::to_string(*m_pSrcAID)+
                                   " "+std::to_string(*m_pSrcBID);
    };
    cpIRID  getSrcAID(){return *m_pSrcAID;}
    cpIRID  getSrcBID(){return *m_pSrcBID;}
    void    setDstID(int in_iID){m_iDstID=in_iID;}
    cpIRID* getDstIDPtr(){return &m_iDstID;}
protected:
    cpIRID* m_pSrcAID;
    cpIRID* m_pSrcBID;
    cpIRID m_iDstID;
    ecpIROpcode m_eOpcode;
};

class cpIR_CONST : public cpIR{
public:
    cpIR_CONST(ecpIROpcode in_eIROpcode) : cpIR(in_eIROpcode,NULL,NULL){};
    virtual ~cpIR_CONST(){};
};

class cpIR_CONST_F : public cpIR_CONST{
public:
    cpIR_CONST_F() : cpIR_CONST(ecpIR_CONST_F){};
    virtual ~cpIR_CONST_F(){};
    virtual std::string toIRString(){
        return toString(m_eOpcode)+"("+std::to_string(m_fw)+","+
                                       std::to_string(m_fx)+","+
                                       std::to_string(m_fy)+","+
                                       std::to_string(m_fz)+")";
    }
    float m_fw;
    float m_fx;
    float m_fy;
    float m_fz;
};

class cpIR_CONST_I : public cpIR_CONST{
public:
    cpIR_CONST_I() : cpIR_CONST(ecpIR_CONST_I){};
    virtual ~cpIR_CONST_I(){};
    virtual std::string toIRString(){
        return toString(m_eOpcode)+"("+std::to_string(m_iw)+","+
                                       std::to_string(m_ix)+","+
                                       std::to_string(m_iy)+","+
                                       std::to_string(m_iz)+")";
    }
    int m_iw;
    int m_ix;
    int m_iy;
    int m_iz;
};

class cpIR_CONST_B : public cpIR_CONST{
public:
    cpIR_CONST_B() : cpIR_CONST(ecpIR_CONST_B){};
    virtual ~cpIR_CONST_B(){};
    virtual std::string toIRString(){
        return toString(m_eOpcode)+"("+std::to_string(m_bw)+","+
                                       std::to_string(m_bx)+","+
                                       std::to_string(m_by)+","+
                                       std::to_string(m_bz)+")";
    }
    bool m_bw;
    bool m_bx;
    bool m_by;
    bool m_bz;
};


typedef std::vector<cpIR*> cpIRList;

void cpPrintIR(cpIRList& in_vlist);

#endif