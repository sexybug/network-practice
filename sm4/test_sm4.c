#include <stdio.h>
#include "openssl_sm4.h"

void memory_dump(void *ptr, int len)
{
    for (int i = 0; i < len; ++i)
    {
        if (i % 8 == 0 && i != 0)
            printf(" ");
        if (i % 16 == 0 && i != 0)
            printf("\n");
        printf("%02x ", *((uint8_t *)ptr + i));
    }
    printf("\n");
}

int main()
{
    uint8_t key[] = "1234567812345678";
    uint8_t iv[] = "1234567812345678";
    uint8_t msg[] = "abcdefghabcdefghabcdefghabcdefgh";
    int msg_len=32;

    uint8_t out[1024];
    int out_len=0;

    sm4_cbc(key,iv,msg,msg_len,out,&out_len);

    memory_dump(out,out_len);
    return 0;
}