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
    uint8_t iv[] = "iv01iv01iv01iv01";
    uint8_t in[] = {0x42, 0xc1, 0x7a, 0x53, 0xe7, 0xea, 0x25, 0xb9, 0x0a, 0x18, 0x93, 0x4e, 0x6d, 0x38, 0x71, 0x97,
                        0xc7, 0x68, 0x68, 0xc7, 0x43, 0xb9, 0x9c, 0xc0, 0x3c, 0xf8, 0x9c, 0x34, 0xce, 0x28, 0xa8, 0x6f};
    int in_len = 32;

    uint8_t out[1024];
    int out_len = 0;

    sm4_cbc_dec(key, iv, in, in_len, out, &out_len);
    printf("out len: %d\n", out_len);
    memory_dump(out, out_len);
    return 0;
}