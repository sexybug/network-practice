#include <stdio.h>
#include "sm3.h"

int main()
{
    char *input = "abc";
    unsigned int ilen = 3;
    unsigned char output[32];
    sm3_ctx_t ctx;

    sm3((unsigned char *)input, ilen, output);
    printf("hash:\n");
    for (int i = 0; i < 32; i++)
    {
        printf("%02x", output[i]);
    }
    printf("\n");
    return 0;
}