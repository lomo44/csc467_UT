{
    ivec4 c_iv4 = ivec4(1,2,3,4);
    vec4 c_fv4 = vec4(4.0,3.0,2.0,1.0);
    float c;
    float a = 0.0;
    if(c_fv4 == vec4(4.0,3.0,2.0,1.0)){
        c = dp3(c_fv4,c_fv4);
        a = c+c;
    }
    else{
        int b = 2;
    }
}