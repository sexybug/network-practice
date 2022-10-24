#include <stdio.h>

int main()
{
    int *a=NULL;
    a=(int *)malloc(2);
    int b[]={0x01,0x02};
    memcpy(a,b,2);
    free(a);
    printf("%d\n",a==NULL);

    return 0;
}