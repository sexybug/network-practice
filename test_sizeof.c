#include <stdio.h>
#include <stdlib.h>

struct student{
    int age;          
    unsigned char gender;                

    double score;               // 8 bytes
    struct student *classmate;  // 8 bytes
};

int main()
{
    struct student stu;
    struct student *stu_ptr;
    printf("struct student: %ld\n",sizeof(struct student));
    printf("stu: %ld\n",sizeof(stu));
    printf("stu_ptr: %ld\n",sizeof(stu_ptr));
    return 0;
}