#include "ah_packet.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

ah_packet_t *ah_packet_create(size_t auth_data_len)
{
    size_t auth_hdr_len=sizeof(ip_auth_hdr) + auth_data_len;
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
ah_packet_t *ah_packet_create_from_bytes(uint8_t *packet_bytes, size_t total_len)
{
    ip_auth_hdr *auth_hdr = (ip_auth_hdr *)packet_bytes;
    /* 认证头（包括鉴别数据）总长度 */
    size_t auth_hdr_len = (auth_hdr->hdrlen + 2) * 4;
    size_t auth_data_len = auth_hdr_len - sizeof(ip_auth_hdr);

    ah_packet_t *ah_packet = (ah_packet_t *)malloc(sizeof(ah_packet_t));
    ah_packet->auth_hdr = (ip_auth_hdr *)malloc(auth_hdr_len);
    *(ah_packet->auth_hdr) = *(auth_hdr);
    ah_packet->auth_data_len = auth_data_len;
    ah_packet->data = packet_bytes + auth_hdr_len;
    ah_packet->data_len = total_len - auth_hdr_len;

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

void ah_packet_set_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data)
{
    memcpy(ah_packet->auth_hdr->auth_data, auth_data, ah_packet->auth_data_len);
}

void ah_packet_set_data(ah_packet_t *ah_packet, uint8_t *data, size_t data_len)
{
    ah_packet->data = data;
    ah_packet->data_len = data_len;
}

uint8_t ah_packet_get_nexthdr(ah_packet_t *ah_packet)
{
    return ah_packet->auth_hdr->nexthdr;
}

int ah_packet_get_auth_hdr_len(ah_packet_t *ah_packet)
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

size_t ah_packet_get_auth_data_len(ah_packet_t *ah_packet)
{
    return ah_packet->auth_data_len;
}

void ah_packet_get_auth_data(ah_packet_t *ah_packet, uint8_t *auth_data, size_t *auth_data_len)
{
    memcpy(auth_data, ah_packet->auth_hdr->auth_data,ah_packet->auth_data_len);
    *auth_data_len=ah_packet->auth_data_len;
}

void ah_packet_get_packet_bytes(ah_packet_t *ah_packet, uint8_t *packet_bytes, size_t *total_len)
{
    int auth_hdr_len=ah_packet_get_auth_hdr_len(ah_packet);
    memcpy(packet_bytes,ah_packet->auth_hdr,auth_hdr_len);
    memcpy(packet_bytes+auth_hdr_len,ah_packet->data,ah_packet->data_len);
    *(total_len)=auth_hdr_len+ah_packet->auth_data_len;
}

void ah_packet_destory(ah_packet_t *ah_packet)
{
    free(ah_packet->auth_hdr);
    free(ah_packet);
}