#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

void printHex(uint8_t *pBuf, int nLen)
{
    for (int i = 0; i < nLen; ++i)
    {
        printf("%02x", pBuf[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    const uint8_t msg[] = "123456";
    const int msgLen = 6;
    const uint8_t key[] = "1234567890";
    const int keyLen = 10;
    uint8_t md[32] = {0};
    int mdLen = 0;

    unsigned char *p = HMAC(EVP_sm3(),key,keyLen,msg,msgLen,md,&mdLen);

    printf("%d\n",mdLen);
    printHex(md,mdLen);
    printHex(p,mdLen);

    return 0;
}