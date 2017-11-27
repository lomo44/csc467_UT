#ifndef _cpIR_h
#define _cpIR_h
#include <string>
#include <vector>
#include <sstream>
#include <stack>
// Define some of the opcode of the ir
typedef int cpIRID;

enum ecpIROpcode
{
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
    ecpIR_SLT,
    ecpIR_SGE,
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
    ecpIR_LRP,
    ecpIR_SCOPE_START,
    ecpIR_SCOPE_END,
    ecpIR_Count
};

enum ecpRegisterScalar
{
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

class cpIRRegister
{
  public:
    cpIRRegister();
    cpIRRegister(cpIRID in_iID, bool in_bX, bool in_bY, bool in_bZ, bool in_bW)
    {
        m_iIRID = in_iID;
        m_bMasks[0] = in_bX;
        m_bMasks[1] = in_bY;
        m_bMasks[2] = in_bZ;
        m_bMasks[3] = in_bW;
    };
    cpIRRegister(cpIRID in_iID)
    {
        m_iIRID = in_iID;
        m_bMasks[0] = false;
        m_bMasks[1] = false;
        m_bMasks[2] = false;
        m_bMasks[3] = false;
    }
    cpIRRegister(const cpIRRegister& rhs){
        m_iIRID = rhs.m_iIRID;
        m_bMasks[0] = rhs.m_bMasks[0];
        m_bMasks[1] = rhs.m_bMasks[1];
        m_bMasks[2] = rhs.m_bMasks[2];
        m_bMasks[3] = rhs.m_bMasks[3];
    }

    void negate(){
        m_iIRID = -m_iIRID;
        m_bMasks[0]= - m_bMasks[0];
        m_bMasks[1]= - m_bMasks[1];
        m_bMasks[2]= - m_bMasks[2];
        m_bMasks[3]= - m_bMasks[3];
    }

     void not_(){
        m_iIRID = m_iIRID;
        m_bMasks[0]= ! m_bMasks[0];
        m_bMasks[1]= ! m_bMasks[1];
        m_bMasks[2]= ! m_bMasks[2];
        m_bMasks[3]= ! m_bMasks[3];
    }

    std::string toString()
    {
        std::string ret = std::to_string(m_iIRID);
        // if(m_bMasks[0]||m_bMasks[1]||m_bMasks[2]||m_bMasks[3]){
        //     ret+=".";
        //     if(m_bMasks[0]){
        //         ret+="x";
        //     }
        //     if(m_bMasks[1]){
        //         ret+="y";
        //     }
        //     if(m_bMasks[2]){
        //         ret+="z";
        //     }
        //     if(m_bMasks[3]){
        //         ret+="w";
        //     }
        // }
        return ret;
    }
    bool hasMasks()
    {
        return m_bMasks[0] || m_bMasks[1] || m_bMasks[2] || m_bMasks[3];
    }
    void copyMasks(cpIRRegister *in_pRegister)
    {
        m_bMasks[0] = in_pRegister->m_bMasks[0];
        m_bMasks[1] = in_pRegister->m_bMasks[1];
        m_bMasks[2] = in_pRegister->m_bMasks[2];
        m_bMasks[3] = in_pRegister->m_bMasks[3];
    }
    cpIRID m_iIRID;
    bool m_bMasks[ecpRegister_Count];
};
typedef std::vector<cpIRRegister *> cpIRRegisterList;


class cpIR
{
  public:
    cpIR();
    cpIR(ecpIROpcode in_eIROpcode, cpIRRegister *in_SrcA, cpIRRegister *in_SrcB) : m_SrcA(in_SrcA),
                                                                                   m_SrcB(in_SrcB),
                                                                                   m_SrcC(NULL),
                                                                                   m_Dst(NULL),
                                                                                   m_eOpcode(in_eIROpcode){};
    cpIR(ecpIROpcode in_eIROpcode, cpIRRegister *in_SrcA, cpIRRegister *in_SrcB, cpIRRegister *in_SrcC) : m_SrcA(in_SrcA),
                                                                                                          m_SrcB(in_SrcB),
                                                                                                          m_SrcC(in_SrcC),
                                                                                                          m_Dst(NULL),
                                                                                                          m_eOpcode(in_eIROpcode){};
    
    virtual ~cpIR()
    {
        if (m_Dst != NULL)
        {
            delete m_Dst;
            m_Dst = NULL;
        }
    };
    virtual std::string toIRString()
    {
        std::string ret = "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode);
        if (m_Dst != NULL)
        {
            ret += " ";
            ret += m_Dst->toString();
        }
        if (m_SrcA != NULL)
        {
            ret += " ";
            ret += m_SrcA->toString();
        }
        if (m_SrcB != NULL)
        {
            ret += " ";
            ret += m_SrcB->toString();
        }
        if (m_SrcC != NULL)
        {
            ret += " ";
            ret += m_SrcC->toString();
        }
        return ret;
    };
    cpIRRegister *getSrcA() { return m_SrcA; }
    cpIRRegister *getSrcB() { return m_SrcB; }
    void setDst(cpIRRegister *in_Dst) { m_Dst = in_Dst; }
    cpIRRegister *getDst() { return m_Dst; }
    ecpIROpcode getOpCode() { return m_eOpcode; }
    void setOpCode(ecpIROpcode in_eIROpcode) { m_eOpcode = in_eIROpcode; }

  protected:
    cpIRRegister *m_SrcA;
    cpIRRegister *m_SrcB;
    cpIRRegister *m_SrcC;
    cpIRRegister *m_Dst;
    ecpIROpcode m_eOpcode;
};



class cpIR_CONST : public cpIR
{
  public:
    cpIR_CONST(ecpIROpcode in_eIROpcode) : cpIR(in_eIROpcode, NULL, NULL){};
    virtual ~cpIR_CONST(){};
};

class cpIR_CONST_F : public cpIR_CONST
{
  public:
    cpIR_CONST_F() : cpIR_CONST(ecpIR_CONST_F){};
    virtual ~cpIR_CONST_F(){};
    virtual std::string toIRString()
    {
        return "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_fx) + "," +
               std::to_string(m_fy) + "," +
               std::to_string(m_fz) + "," +
               std::to_string(m_fw) + ")";
    }
    virtual void setScalar(float in_fValue)
    {
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

class cpIR_CONST_I : public cpIR_CONST
{
  public:
    cpIR_CONST_I() : cpIR_CONST(ecpIR_CONST_I){};
    virtual ~cpIR_CONST_I(){};
    virtual std::string toIRString()
    {
        return "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_ix) + "," +
               std::to_string(m_iy) + "," +
               std::to_string(m_iz) + "," +
               std::to_string(m_iw) + ")";
    }
    virtual void setScalar(int in_iValue)
    {
        m_iw = in_iValue;
        m_ix = in_iValue;
        m_iy = in_iValue;
        m_iz = in_iValue;
    }
    static cpIR_CONST_I *generateMaskIR(cpIRRegister* in_pIn)
    {
        cpIR_CONST_I *newIR = new cpIR_CONST_I();
        newIR->m_ix = (in_pIn->m_bMasks[0] == true);
        newIR->m_iy = (in_pIn->m_bMasks[1] == true);
        newIR->m_iz = (in_pIn->m_bMasks[2] == true);
        newIR->m_iw = (in_pIn->m_bMasks[3] == true);
        return newIR;
    }
    static cpIR_CONST_I *generateInvMaskIR(cpIRRegister* in_pIn)
    {
        cpIR_CONST_I *newIR = new cpIR_CONST_I();
        newIR->m_ix = (in_pIn->m_bMasks[0] == false);
        newIR->m_iy = (in_pIn->m_bMasks[1] == false);
        newIR->m_iz = (in_pIn->m_bMasks[2] == false);
        newIR->m_iw = (in_pIn->m_bMasks[3] == false);
        return newIR;
    }
    int m_iw;
    int m_ix;
    int m_iy;
    int m_iz;
};

class cpIR_CONST_B : public cpIR_CONST
{
  public:
    cpIR_CONST_B() : cpIR_CONST(ecpIR_CONST_B){};
    virtual ~cpIR_CONST_B(){};
    virtual std::string toIRString()
    {
        return "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_bx) + "," +
               std::to_string(m_by) + "," +
               std::to_string(m_bz) + "," +
               std::to_string(m_bw) + ")";
    }
    virtual void setScalar(bool in_bValue)
    {
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

class cpIR_Br : public cpIR
{
  public:
    cpIR_Br(int in_iOffset) : cpIR(ecpIR_BR, NULL, NULL)
    {
        m_iOffset = in_iOffset;
    };
    virtual ~cpIR_Br(){};
    virtual std::string toIRString()
    {
        return "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode) + " " + std::to_string(m_iOffset);
    }
    virtual void setOffset(int in_iOffset) { m_iOffset = in_iOffset; }
    virtual int getOffset() { return m_iOffset; }

  protected:
    int m_iOffset;
};

class cpIR_Brz : public cpIR_Br
{
  public:
    cpIR_Brz(int in_iOffset, cpIRRegister *in_pSrcA) : cpIR_Br(in_iOffset)
    {
        m_SrcA = in_pSrcA;
        m_eOpcode = ecpIR_BRZ;
    }
    virtual ~cpIR_Brz(){};
    virtual std::string toIRString()
    {
        return "[" + std::to_string(m_Dst->m_iIRID) + "] " + toString(m_eOpcode) + " " + m_SrcA->toString() + " " + std::to_string(m_iOffset);
    }
};

class cpIRList
{
  public:
    cpIRList(){};
    virtual ~cpIRList();
    int size() { return m_vIRList.size(); }
    cpIR *&operator[](int in_iIndex) { return m_vIRList[in_iIndex]; }
    void print();
    cpIRRegister *insert(cpIR *in_pIR);
    void pushIfCondition(cpIRRegister *in_pCondition);
    void popIfCondition();

  private:
    std::vector<cpIR *> m_vIRList;
    std::stack<cpIRRegister *> m_IfConditionStack;
};


#endif