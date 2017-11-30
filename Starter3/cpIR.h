#ifndef _cpIR_h
#define _cpIR_h
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <algorithm>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
// Define some of the opcode of the ir
typedef int cpIRID;

enum ecpIROpcode
{
    ecpIR_CONST_F,              // 0
    ecpIR_CONST_B,              // 1
    ecpIR_CONST_I,              // 2 
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
    ecpIR_CMP,
    ecpIR_ReserveRegister,
    ecpIR_Count
    };

enum ecpRegisterMask
{
    ecpRegister_X = 0,
    ecpRegister_Y = 1,
    ecpRegister_Z = 2,
    ecpRegister_W = 3,
    ecpRegister_Count,
    ecpRegister_None
};
// Class for IR
/**
 * Use ptr for src ID so that when we change the dstID of the depended IR the src id will change according ly 
 * */
extern int* reg_neighbouring;

class cpIRRegister;

class cpRegisterSet : public std::tr1::unordered_set<cpIRRegister*>{
public:
    std::string toString();
};


typedef cpRegisterSet::iterator cpRegisterSetItor;

std::string toString(ecpIROpcode in_eIROpcode);
std::string toString(ecpRegisterMask in_eRegisterMask);
class cpIRRegister
{
  public:
    cpIRRegister();
    cpIRRegister(cpIRID in_iID)
    {
        m_iIRID = in_iID;
        m_iColor = -1;
        m_bIsConstant = false;
    };
    cpIRRegister(const cpIRRegister& rhs){
        m_iIRID = rhs.m_iIRID;
        m_iColor = rhs.m_iColor;
        m_bIsConstant = false;
    };
    std::string toString()
    {
        if(m_bIsConstant){
            std::string ret = "const_" + std::to_string(m_iIRID);
            return ret; 
        }
        else{
            return std::to_string(m_iIRID);
        }
    }
    bool isPredifined(){return m_iIRID < 0;}
    void updateInterferenceSet(cpRegisterSet& in_rLiveSet);
    std::string getInterferenceSet(){return m_InterferenceSet.toString();}
    cpIRID m_iIRID;
    cpRegisterSet m_InterferenceSet;
    int m_iColor;
    bool m_bIsConstant;
};
typedef std::vector<cpIRRegister *> cpIRRegisterList;


class cpIR
{
  public:
    cpIR(){};
    cpIR(ecpIROpcode in_eIROpcode, cpIRRegister *in_SrcA, cpIRRegister *in_SrcB) : m_SrcA(in_SrcA),
                                                                                   m_SrcB(in_SrcB),
                                                                                   m_SrcC(NULL),
                                                                                   m_Dst(NULL),
                                                                                   m_eSrcAMask(ecpRegister_None),
                                                                                   m_eSrcBMask(ecpRegister_None),
                                                                                   m_eSrcCMask(ecpRegister_None),
                                                                                   m_eOpcode(in_eIROpcode){};
    cpIR(ecpIROpcode in_eIROpcode, cpIRRegister *in_SrcA, cpIRRegister *in_SrcB, cpIRRegister *in_SrcC) : m_SrcA(in_SrcA),
                                                                                                          m_SrcB(in_SrcB),
                                                                                                          m_SrcC(in_SrcC),
                                                                                                          m_Dst(NULL),
                                                                                                          m_eSrcAMask(ecpRegister_None),
                                                                                                          m_eSrcBMask(ecpRegister_None),
                                                                                                          m_eSrcCMask(ecpRegister_None),
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
        std::string ret = "[" + std::to_string(m_Dst->m_iIRID) + "->" + std::to_string(m_Dst->m_iColor)+"] " + toString(m_eOpcode);
        if (m_Dst != NULL)
        {
            ret += " ";
            ret += m_Dst->toString();
        }
        if (m_SrcA != NULL)
        {
            ret += " ";
            ret += m_SrcA->toString();
            if(m_eSrcAMask!=ecpRegister_None){
                ret += toString(m_eSrcAMask);
            }
        }
        if (m_SrcB != NULL)
        {
            ret += " ";
            ret += m_SrcB->toString();
            if(m_eSrcBMask!=ecpRegister_None){
                ret += toString(m_eSrcBMask);
            }
        }
        if (m_SrcC != NULL)
        {
            ret += " ";
            ret += m_SrcC->toString();
            if(m_eSrcCMask!=ecpRegister_None){
                ret += toString(m_eSrcCMask);
            }
        }
        ret += " L:";
        ret += m_LiveSet.toString();
        ret += " I:";
        ret += m_Dst->getInterferenceSet();
        return ret;
    };
    virtual std::string     toRIString(std::vector<std::string>& in_vRegisterMap);
    cpIRRegister *          getSrcA() { return m_SrcA; }
    cpIRRegister *          getSrcB() { return m_SrcB; }
    void                    setDst(cpIRRegister *in_Dst) { m_Dst = in_Dst; }
    cpIRRegister *          getDst() { return m_Dst; }
    ecpIROpcode             getOpCode() { return m_eOpcode; }
    void                    setOpCode(ecpIROpcode in_eIROpcode) { m_eOpcode = in_eIROpcode; }
    cpRegisterSet&          getLiveSet(){return m_LiveSet;}
    void                    generateLiveSet(cpIR* in_pPreviousIR);
    void                    setSrcAMask(ecpRegisterMask in_eRegisterMask){m_eSrcAMask = in_eRegisterMask;}
    void                    setSrcBMask(ecpRegisterMask in_eRegisterMask){m_eSrcBMask = in_eRegisterMask;}
    void                    setSrcCMask(ecpRegisterMask in_eRegisterMask){m_eSrcCMask = in_eRegisterMask;}

  protected:
    cpIRRegister *m_SrcA;
    cpIRRegister *m_SrcB;
    cpIRRegister *m_SrcC;
    cpIRRegister *m_Dst;
    ecpRegisterMask m_eSrcAMask;
    ecpRegisterMask m_eSrcBMask;
    ecpRegisterMask m_eSrcCMask;
    ecpIROpcode m_eOpcode;
    cpRegisterSet m_LiveSet;
};



class cpIR_CONST : public cpIR
{
  public:
    cpIR_CONST(ecpIROpcode in_eIROpcode) : cpIR(in_eIROpcode, NULL, NULL){};
    virtual ~cpIR_CONST(){};
    virtual std::string toRIString(std::vector<std::string>& in_vRegisterMap){return "";}
};

class cpIR_CONST_F : public cpIR_CONST
{
  public:
    cpIR_CONST_F() : cpIR_CONST(ecpIR_CONST_F){
        m_fx = 0.0;
        m_fy = 0.0;
        m_fz = 0.0;
        m_fw = 0.0;
    };
    virtual ~cpIR_CONST_F(){};
    virtual std::string toIRString()
    {
        std::string ret= "[" + std::to_string(m_Dst->m_iIRID)+ "->" + std::to_string(m_Dst->m_iColor) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_fx) + "," +
                          std::to_string(m_fy) + "," +
                          std::to_string(m_fz) + "," +
                          std::to_string(m_fw) + ")" +
                          " L:"+m_LiveSet.toString() +
                          " I:"+m_Dst->getInterferenceSet();
        return ret;
    }
    virtual std::string toRIString(std::vector<std::string>& in_vRegisterMap){
        std::string ret = "PARAM "+m_Dst->toString()+" = "+"{"+
                                                    std::to_string(m_fx)+","+
                                                    std::to_string(m_fy)+","+
                                                    std::to_string(m_fz)+","+
                                                    std::to_string(m_fw)+"}";
        return ret;
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
    cpIR_CONST_I() : cpIR_CONST(ecpIR_CONST_I){
        m_ix = 0;
        m_iy = 0;
        m_iz = 0;
        m_iw = 0;
    };
    virtual ~cpIR_CONST_I(){};
    virtual std::string toIRString()
    {
        std::string ret = "[" + std::to_string(m_Dst->m_iIRID)+ "->" + std::to_string(m_Dst->m_iColor) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_ix) + "," +
                            std::to_string(m_iy) + "," +
                            std::to_string(m_iz) + "," +
                            std::to_string(m_iw) + ")" + 
                            " L:"+m_LiveSet.toString() +
                            " I:"+m_Dst->getInterferenceSet();
        return ret;
    }
    virtual std::string toRIString(std::vector<std::string>& in_vRegisterMap){
        std::string ret = "PARAM "+m_Dst->toString()+" = "+"{"+
                                                    std::to_string(m_ix)+","+
                                                    std::to_string(m_iy)+","+
                                                    std::to_string(m_iz)+","+
                                                    std::to_string(m_iw)+"}";
        return ret;
    }
    virtual void setScalar(int in_iValue)
    {
        m_iw = in_iValue;
        m_ix = in_iValue;
        m_iy = in_iValue;
        m_iz = in_iValue;
    }
    static cpIR_CONST_I *generateMaskIR(ecpRegisterMask in_eMask)
    {
        cpIR_CONST_I *newIR = new cpIR_CONST_I();
        newIR->m_ix = (in_eMask == ecpRegister_X);
        newIR->m_iy = (in_eMask == ecpRegister_Y);
        newIR->m_iz = (in_eMask == ecpRegister_Z);
        newIR->m_iw = (in_eMask == ecpRegister_W);
        return newIR;
    }
    static cpIR_CONST_I *generateInvMaskIR(ecpRegisterMask in_eMask)
    {
        cpIR_CONST_I *newIR = new cpIR_CONST_I();
        newIR->m_ix = (in_eMask != ecpRegister_X);
        newIR->m_iy = (in_eMask != ecpRegister_Y);
        newIR->m_iz = (in_eMask != ecpRegister_Z);
        newIR->m_iw = (in_eMask != ecpRegister_W);
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
    cpIR_CONST_B() : cpIR_CONST(ecpIR_CONST_B){
        m_bx = 0;
        m_by = 0;
        m_bz = 0;
        m_bw = 0;
    };
    virtual ~cpIR_CONST_B(){};
    virtual std::string toIRString()
    {
        std::string ret = "[" + std::to_string(m_Dst->m_iIRID)+ "->" + std::to_string(m_Dst->m_iColor) + "] " + toString(m_eOpcode) + "(" + std::to_string(m_bx) + "," +
                            std::to_string(m_by) + "," +
                            std::to_string(m_bz) + "," +
                            std::to_string(m_bw) + ")" + 
                            " L:"+m_LiveSet.toString() +
                            " I:"+m_Dst->getInterferenceSet();
        //  if (!m_Dependencylist.empty()){
        //     ret += " {";
        //     for (int i = 0; i < m_Dependencylist.size(); i++){
        //         ret+= " ";
        //         ret += std::to_string(m_Dependencylist[i]);
        //     }
        //     ret +=" }";
        // }
        return ret;
    }
    virtual std::string toRIString(std::vector<std::string>& in_vRegisterMap){
        std::string ret = "PARAM "+m_Dst->toString()+" = "+"{"+
                                                    std::to_string(m_bx)+","+
                                                    std::to_string(m_by)+","+
                                                    std::to_string(m_bz)+","+
                                                    std::to_string(m_bw)+"}";
        return ret;
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

typedef std::tr1::unordered_map<int,cpIRRegister*> cpConstIntMap;
typedef cpConstIntMap::iterator cpConstIntMapItor;
typedef std::tr1::unordered_map<float,cpIRRegister*> cpConstFloatMap;
typedef cpConstFloatMap::iterator cpConstFloatMapItor;


class cpIRList
{
  public:
    cpIRList(){
        m_iChromaticNumber = 0;
    };
    virtual ~cpIRList();
    int size() { return m_vIRList.size(); }
    cpIR *&operator[](int in_iIndex) { return m_vIRList[in_iIndex]; }
    void printIR();
    void printRI(FILE* in_pOutput);
    cpIRRegister *insert(cpIR *in_pIR);
    void pushIfCondition(cpIRRegister *in_pCondition);
    void popIfCondition();
    void generateDependency();
    void registerMapping();
    cpIRRegister* getConst(float in_fIn);
    cpIRRegister* getConst(int in_iIn);
  private:
    std::vector<cpIR *> m_vIRList;
    std::stack<cpIRRegister *> m_IfConditionStack;
    cpConstIntMap m_ConstIntMap;
    cpConstFloatMap m_ConstFloatMap;
    int m_iChromaticNumber;
};


#endif