#ifndef _cpRI_h
#define _cpRI_h

enum ecpRIOpcode{
    ecpRIOpcode_ABS,// v v absolute value
    ecpRIOpcode_ADD,// v,v v add
    ecpRIOpcode_CMP,// v,v,v v compare
    ecpRIOpcode_COS,// s ssss cosine with reduction to [-PI,PI]
    ecpRIOpcode_DP3,// v,v ssss 3-component dot product
    ecpRIOpcode_DP4,// v,v ssss 4-component dot product
    ecpRIOpcode_DPH,// v,v ssss homogeneous dot product
    ecpRIOpcode_DST,// v,v v distance vector
    ecpRIOpcode_EX2,// s ssss exponential base 2
    ecpRIOpcode_FLR,// v v floor
    ecpRIOpcode_FRC,// v v fraction
    ecpRIOpcode_KIL,// v v kill fragment
    ecpRIOpcode_LG2,// s ssss logarithm base 2
    ecpRIOpcode_LIT,// v v compute light coefficients
    ecpRIOpcode_LRP,// v,v,v v linear interpolation
    ecpRIOpcode_MAD,// v,v,v v multiply and add
    ecpRIOpcode_MAX,// v,v v maximum
    ecpRIOpcode_MIN,// v,v v minimum
    ecpRIOpcode_MOV,// v v move
    ecpRIOpcode_MUL,// v,v v multiply
    ecpRIOpcode_POW,// s,s ssss exponentiate
    ecpRIOpcode_RCP,// s ssss reciprocal
    ecpRIOpcode_RSQ,// s ssss reciprocal square root
    ecpRIOpcode_SCS,// s ss-- sine/cosine without reduction
    ecpRIOpcode_SGE,// v,v v set on greater than or equal
    ecpRIOpcode_SIN,// s ssss sine with reduction to [-PI,PI]
    ecpRIOpcode_SLT,// v,v v set on less than
    ecpRIOpcode_SUB,// v,v v subtract
    ecpRIOpcode_SWZ,// v v extended swizzle
    ecpRIOpcode_TEX,// v,u,t v texture sample
    ecpRIOpcode_TXB,// v,u,t v texture sample with bias
    ecpRIOpcode_TXP,// v,u,t v texture sample with projection
    ecpRIOpcode_XPD // v,v v cross product
};
//
#endif