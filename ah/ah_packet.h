#ifndef AH_PACKET_H
#define AH_PACKET_H

#include <linux/ip.h>
#include <stdint.h>
#include <stddef.h>

typedef struct ip_auth_hdr ip_auth_hdr;
typedef struct ah_packet_t ah_packet_t;

struct ah_packet_t
{
    ip_auth_hdr *auth_hdr;
    /* AH头 鉴别数据 字段长度。单位：字节 */
    size_t auth_data_len;
    uint8_t *data;
    size_t data_len;
};

ah_packet_t *ah_packet_create(size_t auth_data_len);
ah_packet_t *ah_packet_create_from_bytes(uint8_t *packet_bytes, size_t total_len);

void ah_packet_set_nexthdr(ah_packet_t *ah_packet, uint8_t nexthdr);
void ah_packet_set_spi(ah_packet_t *ah_packet, uint32_t spi);
void ah_packet_set_seq_no(ah_packet_t *ah_packet, uint32_t seq_no);
void ah_packet_set_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data);
void ah_packet_set_data(ah_packet_t *ah_packet, uint8_t *data, size_t data_len);

uint8_t ah_packet_get_nexthdr(ah_packet_t *ah_packet);
/* 获取AH头长度，单位：字节 */
int ah_packet_get_auth_hdr_len(ah_packet_t *ah_packet);
uint32_t ah_packet_get_spi(ah_packet_t *ah_packet);
uint32_t ah_packet_get_seq_no(ah_packet_t *ah_packet);
size_t ah_packet_get_auth_data_len(ah_packet_t *ah_packet);
/* 获取鉴别数据 */
void ah_packet_get_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data, size_t *auth_data_len);
/* 获取整个数据包（包括AH头和数据部分）所有字节的数据 */
void ah_packet_get_packet_bytes(ah_packet_t *ah_packet, uint8_t *packet_bytes, size_t *total_len);

void ah_packet_destory(ah_packet_t *ah_packet);

#endif /* AH_PACKET_H */