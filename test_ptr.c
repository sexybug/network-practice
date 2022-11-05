#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct B B;
struct B
{
    int b1;
    char *b2;
};

typedef struct A A;
struct A
{
    int a1;
    char *a2;
    B *b;
};

int main()
{
    //必须先手动分配内存
    A *p = (A *)malloc(sizeof(A));
    //然后才能为指针赋值
    *p = (A){1,"abc",NULL};  
    *p=(A){
        .a1=2,
        .a2="bb",
        //无法使用.(*b)的形式赋值。如下是为b赋值的一种方法。
        .b=(B*)&((B){.b1=1})
    };
    //经调试发现，指针p指向的地址未发生改变。说明以上都是为分配的内存赋值。
    free(p);

    int *a0=(int *)malloc(0);
    printf("%d\n",malloc_usable_size(a0));  //output 24
    printf("%d\n",a0==NULL);
    free(a0);
    return 0;
}