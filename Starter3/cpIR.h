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
    ecpIR_EQ,
    ecpIR_NEQ,
    ecpIR_GT,
    ecpIR_LT,
    ecpIR_GEQ,
    ecpIR_LEQ,
    ecpIR_AND,
    ecpIR_OR,
    ecpIR_NEG,
    ecpIR_NOT,
    ecpIR_POW,       
    ecpIR_BRZ,
    ecpIR_BR,
    ecpIR_LIT,
    ecpIR_RSQ,
    ecpIR_DP3,
    ecpIR_Count
};

enum ecpRegisterScalar{
    ecpRegister_W,
    ecpRegister_X,
    ecpRegister_Y,
    ecpRegister_Z,
    ecpRegister_Count
};
// Class for IR
/**
 * Use ptr for src ID so that when we change the dstID of the depended IR the src id will change according ly 
 * */

std::string toString(ecpIROpcode in_eIROpcode);
class cpIRRegister{
public:
    cpIRRegister();
    cpIRRegister(cpIRID in_iID, bool in_bX,bool in_bY,bool in_bZ,bool in_bW){
        m_iIRID = in_iID;
        m_bMasks[0] = in_bX;
        m_bMasks[1] = in_bY;
        m_bMasks[2] = in_bZ;
        m_bMasks[3] = in_bW;
    };
    cpIRRegister(cpIRID in_iID){
         m_iIRID = in_iID;
        m_bMasks[0] = false;
        m_bMasks[1] = false;
        m_bMasks[2] = false;
        m_bMasks[3] = false;
    }
    std::string toString(){
        std::string ret = std::to_string(m_iIRID);
        if(m_bMasks[0]||m_bMasks[1]||m_bMasks[2]||m_bMasks[3]){
            ret+=".";
            if(m_bMasks[0]){
                ret+="x";
            }
            if(m_bMasks[1]){
                ret+="y";
            }
            if(m_bMasks[2]){
                ret+="z";
            }
            if(m_bMasks[3]){
                ret+="w";
            }
        }
        return ret;
    }
    cpIRID m_iIRID;
    bool m_bMasks[ecpRegister_Count];
};

typedef std::vector<cpIRRegister*> cpIRRegisterList;

class cpIR{
public:
    cpIR();
    cpIR(ecpIROpcode in_eIROpcode, cpIRRegister* in_SrcA, cpIRRegister* in_SrcB): m_SrcA(in_SrcA),
                                                                            m_SrcB(in_SrcB),
                                                                            m_eOpcode(in_eIROpcode){};
    virtual ~cpIR(){};
    virtual std::string toIRString(){
        return toString(m_eOpcode)+" "+m_Dst->toString()+
                                   " "+m_SrcA->toString()+
                                   " "+m_SrcB->toString();
    };
    cpIRRegister*  getSrcA(){return m_SrcA;}
    cpIRRegister*  getSrcB(){return m_SrcB;}
    void           setDst(cpIRRegister* in_Dst){m_Dst=in_Dst;}
    cpIRRegister*  getDst(){return m_Dst;}
protected:
    cpIRRegister* m_SrcA;
    cpIRRegister* m_SrcB;
    cpIRRegister* m_Dst;
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
    virtual void setScalar(float in_fValue){
        m_fw = in_fValue;
        m_fx = in_fValue;
        m_fy = in_fValue;
        m_fz = in_fValue;
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
    virtual void setScalar(int in_iValue){
        m_iw = in_iValue;
        m_ix = in_iValue;
        m_iy = in_iValue;
        m_iz = in_iValue;
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
    virtual void setScalar(bool in_bValue){
        m_bw = in_bValue;
        m_bx = in_bValue;
        m_by = in_bValue;
        m_bz = in_bValue;
    }
    bool m_bw;
    bool m_bx;
    bool m_by;
    bool m_bz;
};

/** cpIRContext
 * Control context that used when traversing ast node and generate IR
 * **/
struct cpIRContext{
    cpIR* m_pPreviousIfCondition;
};

typedef std::vector<cpIR*> cpIRList;

void cpPrintIR(cpIRList& in_vlist);
void cpInsertInList(cpIR* in_pIR, cpIRList& in_List);
#endif