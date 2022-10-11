#ifndef IP_PACKET_H
#define IP_PACKET_H

#include <linux/ip.h>
#include <stdint.h>
#include <stddef.h>

typedef struct iphdr iphdr;
typedef struct ip_packet_t ip_packet_t;

struct ip_packet_t
{
    iphdr *iph;
    uint8_t *data;
    size_t data_len;
};

ip_packet_t *ip_packet_create();
ip_packet_t *ip_packet_create_from_bytes(uint8_t *packet_bytes, size_t total_len);

void ip_packet_set_id(ip_packet_t *ip_packet, uint16_t id);
void ip_packet_set_protocol(ip_packet_t *ip_packet, uint8_t protocol);
void ip_packet_set_saddr(ip_packet_t *ip_packet, const char *saddr);
void ip_packet_set_daddr(ip_packet_t *ip_packet, const char *daddr);
void ip_packet_set_data(ip_packet_t *ip_packet, uint8_t *data, size_t data_len);

/* 获取IP头长度，单位：字节 */
int ip_packet_get_iph_len(ip_packet_t *ip_packet);
int ip_packet_get_protocol(ip_packet_t *ip_packet);
char *ip_packet_get_saddr(ip_packet_t *ip_packet);
char *ip_packet_get_daddr(ip_packet_t *ip_packet);
/* 获取数据部分 */
void ip_packet_get_data(ip_packet_t *ip_packet, uint8_t *data, size_t *data_len);
/* 获取整个数据包（包括IP头部和数据部分）所有字节的数据 */
void ip_packet_get_packet_bytes(ip_packet_t *ip_packet, uint8_t *packet_bytes, size_t *total_len);

void ip_packet_destory(ip_packet_t *ip_packet);

#endif /* IP_PACKET_H */