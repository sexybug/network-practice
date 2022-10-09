#ifndef SM3_H
#define SM3_H

#define SM3_DIGEST_LENGTH 32
#define SM3_BLOCK_SIZE 64

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		uint32_t digest[8];
		int nblocks;
		unsigned char block[64];
		int num;
	} sm3_ctx_t;

	int sm3_init(sm3_ctx_t *ctx);
	int sm3_update(sm3_ctx_t *ctx, const unsigned char *data, size_t data_len);
	int sm3_final(sm3_ctx_t *ctx, unsigned char digest[SM3_DIGEST_LENGTH]);
	int sm3_compress(uint32_t digest[8], const unsigned char block[SM3_BLOCK_SIZE]);
	int sm3(const unsigned char *data, size_t datalen, unsigned char digest[SM3_DIGEST_LENGTH]);

#ifdef __cplusplus
}
#endif
#endif
