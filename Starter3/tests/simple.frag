{
    const vec4 c_fv4 = vec4(1.0,2.0,3.0,4.0);
    const vec3 c_fv3 = vec3(2.0,3.0,4.0);
    const vec2 c_fv2 = vec2(3.0,4.0);
    const float c_fv1 = 1.0;

    bvec4 c_fb4 = bvec4(true,false,true,true);
    bvec3 c_fb3 = bvec3(false,true,false);
    bvec2 c_fb2 = bvec2(true,false);
    bool c_fb1 = false;

    const ivec4 c_iv4 = ivec4(1,2,3,4);
    const ivec3 c_iv3 = ivec3(2,3,4);
    const ivec2 c_iv2 = ivec2(3,4);
    const int c_iv1 = 1;

    if(c_fb1==false){
        c_iv1 = 2;
        c_fv1 = rsq(c_iv1);
        c_fv1 = rsq(c_iv1/c_iv1);
        c_fv4[3] = rsq(c_iv1^c_iv1);
        c_fv1 = dp3(c_fv3,c_fv3);
        c_iv2[2] = dp3(c_fv3+c_fv3,c_fv3*c_fv3);
    }
    else{
        if(!c_fb1){
            c_iv2 = 3;
            c_fv1 = -rsq(c_fv1);
        }
        c_fv1 = dp3(c_fv4,c_fv4);
        c_iv2[0] = dp3(c_fv4-c_fv4,c_fv4);
    }
}