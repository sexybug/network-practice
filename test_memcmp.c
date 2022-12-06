#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main()
{
    char * a="abc";
    char * b="abc";
    int a2=999;
    int b2=999;
    int res=memcmp(&a2,&b2,sizeof(int));
    printf("%d\n",res);
    return 0;
}