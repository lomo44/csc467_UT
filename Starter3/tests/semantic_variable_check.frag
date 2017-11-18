{
    int a=1;
    int a=1;/*duplicate declaration*/
    float a=true;
    int c=3;
    const int e=123;
    
    {
        int a = 1 ;/*shadow*/
        int b = 1;
        c=a;/*scope check correct*/
        d=2;/*id not declared*/
        e=c;
    }
    {
        int a = 2 ;/*shadow*/
    }
    a=b;
}