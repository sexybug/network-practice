#include <stdio.h>
#include "hmac_sm3.h"

int main()
{
    char *key = "1234567890";
    unsigned char output[32];
    char *clear = "123456";
    unsigned int keyoutlen = 10;
    unsigned int clearoutlen = 6;

    printf("keylen:%d \n", keyoutlen);
    printf("clearlen:%d \n", clearoutlen);
    sm3_hmac(clear, clearoutlen, key, keyoutlen, output);
    int ij = 0;
    for (ij = 0; ij < 32; ij++)
    {
        printf("%02x", output[ij]);
    }

    printf("\n");

    return 0;
}