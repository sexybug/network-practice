#include "ah_packet.h"

#include <stdlib.h>
#include <string.h>
#include <linux/ip.h>
#include <arpa/inet.h>

typedef struct ip_auth_hdr ip_auth_hdr;
struct ah_packet_t
{
    ip_auth_hdr *auth_hdr;
    uint8_t *data;
    /* AH头"鉴别数据"字段长度。单位：字节 */
    uint16_t auth_data_len;
    uint16_t data_len;
};

ah_packet_t *ah_packet_create(uint16_t auth_data_len)
{
    uint16_t auth_hdr_len = sizeof(ip_auth_hdr) + auth_data_len;
    ah_packet_t *ah_packet = (ah_packet_t *)malloc(sizeof(ah_packet_t));

    ah_packet->auth_hdr = (ip_auth_hdr *)malloc(auth_hdr_len);
    memset(ah_packet->auth_hdr, 0, auth_hdr_len);
    *(ah_packet->auth_hdr) = (ip_auth_hdr){
        .nexthdr = 0x00,
        .hdrlen = auth_hdr_len / 4 - 2,
        .reserved = 0,
        .spi = 0,
        .seq_no = 0};
    ah_packet->auth_data_len = auth_data_len;
    ah_packet->data = NULL;
    ah_packet->data_len = 0;

    return ah_packet;
}
ah_packet_t *ah_packet_create_from_bytes(const uint8_t *packet_bytes, uint16_t total_len)
{
    ip_auth_hdr *auth_hdr = (ip_auth_hdr *)packet_bytes;
    /* 认证头（包括鉴别数据）总长度 */
    uint16_t auth_hdr_len = (auth_hdr->hdrlen + 2) * 4;
    uint16_t auth_data_len = auth_hdr_len - sizeof(ip_auth_hdr);

    ah_packet_t *ah_packet = (ah_packet_t *)malloc(sizeof(ah_packet_t));
    ah_packet->auth_hdr = (ip_auth_hdr *)malloc(auth_hdr_len);
    memcpy(ah_packet->auth_hdr, auth_hdr, auth_hdr_len);
    //不能使用直接赋值的方式，auth_data会赋值失败。
    //必须用memcpy给结构体赋值。下面的方法会导致结构体中的变长数组赋值失败。
    //*(ah_packet->auth_hdr) = *(auth_hdr);
    ah_packet->auth_data_len = auth_data_len;

    ah_packet->data_len = total_len - auth_hdr_len;
    ah_packet->data = (uint8_t *)malloc(ah_packet->data_len);
    memcpy(ah_packet->data, packet_bytes + auth_hdr_len, ah_packet->data_len);

    return ah_packet;
}

void ah_packet_set_nexthdr(ah_packet_t *ah_packet, uint8_t nexthdr)
{
    ah_packet->auth_hdr->nexthdr = nexthdr;
}

void ah_packet_set_spi(ah_packet_t *ah_packet, uint32_t spi)
{
    ah_packet->auth_hdr->spi = htonl(spi);
}

void ah_packet_set_seq_no(ah_packet_t *ah_packet, uint32_t seq_no)
{
    ah_packet->auth_hdr->seq_no = htonl(seq_no);
}

void ah_packet_set_auth_data(ah_packet_t *ah_packet, const uint8_t *auth_data)
{
    memcpy(ah_packet->auth_hdr->auth_data, auth_data, ah_packet->auth_data_len);
}

void ah_packet_set_auth_data_bezero(ah_packet_t *ah_packet)
{
    memset(ah_packet->auth_hdr->auth_data, 0, ah_packet->auth_data_len);
}

void ah_packet_set_data(ah_packet_t *ah_packet, const uint8_t *data, uint16_t data_len)
{
    if (ah_packet->data != NULL)
    {
        free(ah_packet->data);
        ah_packet->data = NULL;
    }
    ah_packet->data = (uint8_t *)malloc(data_len);
    memcpy(ah_packet->data, data, data_len);
    ah_packet->data_len = data_len;
}

uint8_t ah_packet_get_nexthdr(ah_packet_t *ah_packet)
{
    return ah_packet->auth_hdr->nexthdr;
}

uint16_t ah_packet_get_auth_hdr_len(ah_packet_t *ah_packet)
{
    return (ah_packet->auth_hdr->hdrlen + 2) * 4;
}

uint32_t ah_packet_get_spi(ah_packet_t *ah_packet)
{
    return ntohl(ah_packet->auth_hdr->spi);
}

uint32_t ah_packet_get_seq_no(ah_packet_t *ah_packet)
{
    return ntohl(ah_packet->auth_hdr->seq_no);
}

uint16_t ah_packet_get_auth_data_len(ah_packet_t *ah_packet)
{
    return ah_packet->auth_data_len;
}

void ah_packet_get_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data)
{
    memcpy(auth_data, ah_packet->auth_hdr->auth_data, ah_packet->auth_data_len);
}

uint16_t ah_packet_get_data_len(ah_packet_t *ah_packet)
{
    return ah_packet->data_len;
}

void ah_packet_get_data(ah_packet_t *ah_packet, uint8_t *data)
{
    memcpy(data, ah_packet->data, ah_packet->data_len);
}

uint16_t ah_packet_get_packet_len(ah_packet_t *ah_packet)
{
    return ah_packet_get_auth_hdr_len(ah_packet) + ah_packet->data_len;
}

void ah_packet_get_packet_bytes(ah_packet_t *ah_packet, uint8_t *packet_bytes)
{
    uint16_t auth_hdr_len = ah_packet_get_auth_hdr_len(ah_packet);
    memcpy(packet_bytes, ah_packet->auth_hdr, auth_hdr_len);
    memcpy(packet_bytes + auth_hdr_len, ah_packet->data, ah_packet->data_len);
}

ah_packet_t *ah_packet_clone(ah_packet_t *ah_packet)
{
    ah_packet_t *clone = (ah_packet_t *)malloc(sizeof(ah_packet_t));
    clone->auth_hdr = (ip_auth_hdr *)malloc(ah_packet_get_auth_hdr_len(ah_packet));
    memcpy(clone->auth_hdr, ah_packet->auth_hdr, ah_packet_get_auth_hdr_len(ah_packet));
    clone->auth_data_len = ah_packet->auth_data_len;
    memcpy(clone, ah_packet->data, ah_packet->data_len);
    clone->data_len = ah_packet->data_len;
    return clone;
}

void ah_packet_destory(ah_packet_t *ah_packet)
{
    if (ah_packet->data != NULL)
    {
        free(ah_packet->data);
        ah_packet->data = NULL;
    }
    free(ah_packet->auth_hdr);
    ah_packet->auth_hdr = NULL;
    free(ah_packet);
    ah_packet = NULL;
}