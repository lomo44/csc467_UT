{
    const int a = 2;
    const float x =2.3;
    const float h=x; /*Should we allow const to const assignment*/
    int b=1;
    int c=2;
    const int d =b+c;/* now showing invalid assignment, should be invalid initializing const with expression*/
    const int f =  gl_Light_Half;
    const vec4 e = gl_Light_Half;
    const vec4 g =gl_FogFragCoord;/* now showing invalid assignment, should be invalid initializing const with non-uniform type*/
 }