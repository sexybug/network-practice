#ifndef _OPENSSL_SM4_H
#define _OPENSSL_SM4_H

#include <stdint.h>

/* 输入必须是整分组 */
int sm4_cbc_enc(const uint8_t *key, const uint8_t *iv, const uint8_t *in, int in_len, uint8_t *out, int *out_len);
int sm4_cbc_dec(const uint8_t *key, const uint8_t *iv, const uint8_t *in, int in_len, uint8_t *out, int *out_len);

#endif /* _OPENSSL_SM4_H */