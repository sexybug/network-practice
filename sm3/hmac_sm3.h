#ifndef HMAC_SM3_H
#define HMAC_SM3_H

#include "sm3.h"

#define SM3_HMAC_SIZE 32
#define IPAD 0x36
#define OPAD 0x5C

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        sm3_ctx_t sm3_ctx;
        unsigned char key[64];
    } sm3_hmac_ctx_t;

    int sm3_hmac_init(sm3_hmac_ctx_t *ctx, const unsigned char *key, size_t key_len);
    int sm3_hmac_update(sm3_hmac_ctx_t *ctx, const unsigned char *data, size_t data_len);
    int sm3_hmac_final(sm3_hmac_ctx_t *ctx, unsigned char *mac);
    int sm3_hmac(const unsigned char *data, int data_len,
                 const unsigned char *key, int key_len,
                 unsigned char *mac);

#ifdef __cplusplus
}
#endif
#endif
