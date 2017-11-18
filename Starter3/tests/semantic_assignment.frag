{
    float a=1; 
    int b=1.0;
    bool c = 1;
    ivec4 d= ivec4(1,2,3,4);
    a=true;
    a=b;
    a=c;
    b=true;
    b=a;
    b=c;
    c=1.02;
    c=a;
    c=b;
    d[4]=a;/*Here it should have 2 error, but only has 1 instead*/
    d[0]=1;
    d[1]=2;
    d[2]=a;
    d[3]=c;
    b=d[0];


}