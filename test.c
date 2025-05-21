#include<stdio.h>
int fun()
{
int fun_ret;
int a,b;
if(a>0)
{
fun_ret = 1;
}
else
{
printf("%d",a);
}
return fun_ret;
}
int main()
{
printf("%d",fun());
return 0;
}