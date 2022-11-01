#ifndef AH_PACKET_H
#define AH_PACKET_H

#include <stdint.h>

typedef struct ah_packet_t ah_packet_t;

ah_packet_t *ah_packet_create(uint16_t auth_data_len);
ah_packet_t *ah_packet_create_from_bytes(uint8_t *packet_bytes, uint16_t total_len);

/* 下一个头(8位) */
void ah_packet_set_nexthdr(ah_packet_t *ah_packet, uint8_t nexthdr);
/* 安全参数索引(32位) */
void ah_packet_set_spi(ah_packet_t *ah_packet, uint32_t spi);
/* 序列号(32位) */
void ah_packet_set_seq_no(ah_packet_t *ah_packet, uint32_t seq_no);
/* 鉴别数据(变长的) */
void ah_packet_set_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data);
/* 鉴别数据(变长的) */
void ah_packet_set_auth_data_bezero(ah_packet_t *ah_packet);
/* 数据部分 */
void ah_packet_set_data(ah_packet_t *ah_packet, uint8_t *data, uint16_t data_len);

uint8_t ah_packet_get_nexthdr(ah_packet_t *ah_packet);
/* 获取AH头长度，单位：字节 */
uint16_t ah_packet_get_auth_hdr_len(ah_packet_t *ah_packet);
uint32_t ah_packet_get_spi(ah_packet_t *ah_packet);
uint32_t ah_packet_get_seq_no(ah_packet_t *ah_packet);
/* 获取鉴别数据长度 */
uint16_t ah_packet_get_auth_data_len(ah_packet_t *ah_packet);
/* 获取鉴别数据 */
void ah_packet_get_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data);
/* 获取数据部分长度 */
uint16_t ah_packet_get_data_len(ah_packet_t *ah_packet);
/* 获取数据部分 */
void ah_packet_get_data(ah_packet_t *ah_packet, uint8_t *data);
/* 获取整个AH数据包总长度 */
uint16_t ah_packet_get_packet_len(ah_packet_t *ah_packet);
/* 获取整个数据包（包括AH头和数据部分）所有字节的数据 */
void ah_packet_get_packet_bytes(ah_packet_t *ah_packet, uint8_t *packet_bytes);

/* clone整个AH包，包括AH头和数据部分 */
ah_packet_t *ah_packet_clone(ah_packet_t *ah_packet);

void ah_packet_destory(ah_packet_t *ah_packet);

#endif /* AH_PACKET_H */