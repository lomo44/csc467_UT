#ifndef _cpIR_h
#define _cpIR_h
#include <string>
#include <vector>
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
class cpIR{
public:
    cpIR();
    cpIR(ecpIROpcode in_eIROpcode, cpIRID* in_iSrcAID, cpIRID* in_iSrcBID): m_iSrcAID(in_iSrcBID),
                                                                            m_iSrcBID(in_iSrcAID),
                                                                            m_eOpcode(in_eIROpcode){};
    virtual ~cpIR();
    virtual std::string toIRString();
    virtual std::string toRIString();
    cpIRID  getSrcAID(){return *m_iSrcAID;}
    cpIRID  getSrcBID(){return *m_iSrcBID;}
    void    setDstID(int in_iID){m_iDstID=in_iID;}
    cpIRID* getDstIDPtr(){return &m_iDstID;}
private:
    cpIRID* m_iSrcAID;
    cpIRID* m_iSrcBID;
    cpIRID m_iDstID;
    ecpIROpcode m_eOpcode;
};

typedef std::vector<cpIR*> cpIRList;

void cpPrintIR(cpIRList& in_vlist);
void cpPrintRI(cpIRList& in_vlist);

#endif