#include "openssl_sm4.h"
#include <openssl/evp.h>

int sm4_cbc_enc(const uint8_t *key, const uint8_t *iv, const uint8_t *in, int in_len, uint8_t *out, int *out_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    EVP_EncryptInit_ex2(ctx, EVP_sm4_cbc(), key, iv, NULL);
    if (!EVP_EncryptUpdate(ctx, out, out_len, in, in_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int sm4_cbc_dec(const uint8_t *key, const uint8_t *iv, const uint8_t *in, int in_len, uint8_t *out, int *out_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);
    EVP_DecryptInit_ex2(ctx, EVP_sm4_cbc(), key, iv, NULL);
    if (!EVP_DecryptUpdate(ctx, out, out_len, in, in_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}