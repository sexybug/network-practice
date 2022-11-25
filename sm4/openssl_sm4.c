#include "openssl_sm4.h"
#include <openssl/evp.h>

int sm4_cbc(const uint8_t *key, const uint8_t *iv, const uint8_t *in, int in_len, uint8_t *out, int *out_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        return -1;
    }
    EVP_EncryptInit_ex2(ctx, EVP_sm4_cbc(), key, iv, NULL);
    int ret = EVP_EncryptUpdate(ctx, out, out_len, in, in_len);
    if (ret == 0)
    {
        return -1;
    }
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}