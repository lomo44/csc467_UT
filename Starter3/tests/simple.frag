{
    ivec4 c_iv4 = ivec4(1,2,3,4);
    vec4 c_fv4 = vec4(4.0,3.0,2.0,1.0);
    float c;
    float a = 0.0;
    if(!(c_fv4 == vec4(4.0,3.0,2.0,1.0))){
        c = dp3(c_fv4,c_fv4);
        a = c+c;
    }
    else{
        if(c_fv4 != vec4(7.0,6.0,5.0,4.0)){
            int b = 2;
            b= -b;
        }
        else{
            c_fv4 = lit(env1);
        }
    }
}