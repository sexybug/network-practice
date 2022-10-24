#include "ip_packet.h"

#include <stdlib.h>
#include <string.h>
#include <linux/ip.h>
#include <arpa/inet.h>

typedef struct iphdr iphdr;
struct ip_packet_t
{
    iphdr *iph;
    uint8_t *data;
    uint16_t data_len;
};

ip_packet_t *ip_packet_create()
{
    /* 为结构体中的指针分配空间 */
    ip_packet_t *ip_packet = (ip_packet_t *)malloc(sizeof(ip_packet_t));
    ip_packet->iph = (iphdr *)malloc(sizeof(iphdr));
    *(ip_packet->iph) = (iphdr){
        .version = IPVERSION,
        .ihl = sizeof(iphdr) / 4,
        .tos = 0,
        /* IP头中的字段要注意大小端问题 */
        .tot_len = htons(sizeof(iphdr)),
        .id = 0,
        .frag_off = 0,
        .ttl = IPDEFTTL,
        .protocol = 0,
        .check = 0,
        .saddr = 0,
        .daddr = 0};
    ip_packet->data = NULL;
    ip_packet->data_len = 0;

    return ip_packet;
}

ip_packet_t *ip_packet_create_from_bytes(uint8_t *packet_bytes, uint16_t total_len)
{
    iphdr *iph = (iphdr *)packet_bytes;
    uint16_t iph_len = iph->ihl * 4;

    ip_packet_t *ip_packet = (ip_packet_t *)malloc(sizeof(ip_packet_t));
    ip_packet->iph = (iphdr *)malloc(iph_len);
    memcpy(ip_packet->iph, iph, iph_len);

    ip_packet->data_len = total_len - iph_len;
    ip_packet->data = (uint8_t *)malloc(ip_packet->data_len);
    memcpy(ip_packet->data, packet_bytes + iph_len, ip_packet->data_len);

    return ip_packet;
}

void ip_packet_set_tos(ip_packet_t *ip_packet, uint8_t tos)
{
    ip_packet->iph->tos = tos;
}

void ip_packet_set_id(ip_packet_t *ip_packet, uint16_t id)
{
    ip_packet->iph->id = htons(id);
}

void ip_packet_set_frag_off(ip_packet_t *ip_packet, uint16_t frag_off)
{
    ip_packet->iph->frag_off = htons(frag_off);
}

void ip_packet_set_ttl(ip_packet_t *ip_packet, uint8_t ttl)
{
    ip_packet->iph->ttl = ttl;
}

void ip_packet_set_protocol(ip_packet_t *ip_packet, uint8_t protocol)
{
    ip_packet->iph->protocol = protocol;
}

void ip_packet_set_check(ip_packet_t *ip_packet, uint16_t check)
{
    ip_packet->iph->check = htons(check);
}

void ip_packet_set_saddr(ip_packet_t *ip_packet, const char *saddr)
{
    ip_packet->iph->saddr = inet_addr(saddr);
}

void ip_packet_set_daddr(ip_packet_t *ip_packet, const char *daddr)
{
    ip_packet->iph->daddr = inet_addr(daddr);
}

void ip_packet_set_data(ip_packet_t *ip_packet, uint8_t *data, uint16_t data_len)
{
    if (ip_packet->data != NULL)
    {
        free(ip_packet->data);
        ip_packet->data = NULL;
    }
    ip_packet->data = (uint8_t *)malloc(data_len);
    memcpy(ip_packet->data, data, data_len);
    ip_packet->data_len = data_len;
    ip_packet->iph->tot_len = htons(ip_packet->iph->ihl * 4 + data_len);
}

uint16_t ip_packet_get_iph_len(ip_packet_t *ip_packet)
{
    return ip_packet->iph->ihl * 4;
}

uint8_t ip_packet_get_protocol(ip_packet_t *ip_packet)
{
    return ip_packet->iph->protocol;
}

char *ip_packet_get_saddr(ip_packet_t *ip_packet)
{
    return inet_ntoa((struct in_addr){ip_packet->iph->saddr});
}

char *ip_packet_get_daddr(ip_packet_t *ip_packet)
{
    return inet_ntoa((struct in_addr){ip_packet->iph->daddr});
}

uint16_t ip_packet_get_data_len(ip_packet_t *ip_packet)
{
    return ip_packet->data_len;
}

void ip_packet_get_data(ip_packet_t *ip_packet, uint8_t *data)
{
    memcpy(data, ip_packet->data, ip_packet->data_len);
}

uint16_t ip_packet_get_packet_len(ip_packet_t *ip_packet)
{
    return ntohs(ip_packet->iph->tot_len);
}

void ip_packet_get_packet_bytes(ip_packet_t *ip_packet, uint8_t *packet_bytes)
{
    uint16_t iph_len = ip_packet_get_iph_len(ip_packet);
    memcpy(packet_bytes, ip_packet->iph, iph_len);
    memcpy(packet_bytes + iph_len, ip_packet->data, ip_packet->data_len);
}

ip_packet_t *ip_packet_clone(ip_packet_t *ip_packet)
{
    ip_packet_t *clone = (ip_packet_t *)malloc(sizeof(ip_packet_t));
    clone->iph = (iphdr *)malloc(ip_packet_get_iph_len(ip_packet));
    clone->data = (uint8_t *)malloc(ip_packet->data_len);
    memcpy(clone->iph, ip_packet->iph, ip_packet_get_iph_len(ip_packet));
    memcpy(clone->data, ip_packet->data, ip_packet->data_len);
    clone->data_len = ip_packet->data_len;
    return clone;
}

void ip_packet_destory(ip_packet_t *ip_packet)
{
    if (ip_packet->data != NULL)
    {
        free(ip_packet->data);
        ip_packet->data = NULL;
    }
    free(ip_packet->iph);
    ip_packet->iph = NULL;
    free(ip_packet);
    ip_packet = NULL;
}