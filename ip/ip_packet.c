#include "ip_packet.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


static iphdr *ip_packet_create_iph()
{
    /* 为ip头分配空间 */
    iphdr *iph = (iphdr *)malloc(sizeof(iphdr));
    *iph = (iphdr){
        .version = IPVERSION,
        .ihl = sizeof(iphdr) / 4,
        .tos = 0,
        .tot_len = htons(sizeof(iphdr)),
        .id = 0,
        .frag_off = 0,
        .ttl = IPDEFTTL,
        .protocol = 0,
        .check = 0,
        .saddr = 0,
        .daddr = 0};
    return iph;
}

ip_packet_t *ip_packet_create()
{
    /* 为结构体中的指针分配空间 */
    ip_packet_t *ip_packet = (ip_packet_t *)malloc(sizeof(ip_packet_t));

    *ip_packet = (ip_packet_t){
        .iph = ip_packet_create_iph(),
        .data = NULL,
        .data_len = 0};

    return ip_packet;
}

ip_packet_t *ip_packet_create_from_bytes(uint8_t *packet_bytes, size_t total_len)
{
    iphdr *iph = (iphdr *)packet_bytes;
    int iph_len = iph->ihl * 4;

    ip_packet_t *ip_packet = (ip_packet_t *)malloc(sizeof(ip_packet_t));
    ip_packet->iph = (iphdr *)malloc(iph_len);
    memcpy(ip_packet->iph, iph, iph_len);
    ip_packet->data = packet_bytes + iph_len;
    ip_packet->data_len = total_len - iph_len;

    return ip_packet;
}

void ip_packet_set_id(ip_packet_t *ip_packet, uint16_t id)
{
    ip_packet->iph->id = id;
}

void ip_packet_set_protocol(ip_packet_t *ip_packet, uint8_t protocol)
{
    ip_packet->iph->protocol = protocol;
}

void ip_packet_set_saddr(ip_packet_t *ip_packet, const char *saddr)
{
    ip_packet->iph->saddr = inet_addr(saddr);
}

void ip_packet_set_daddr(ip_packet_t *ip_packet, const char *daddr)
{
    ip_packet->iph->daddr = inet_addr(daddr);
}

void ip_packet_set_data(ip_packet_t *ip_packet, uint8_t *data, size_t data_len)
{
    ip_packet->data = data;
    ip_packet->data_len = data_len;
    ip_packet->iph->tot_len = htons(ip_packet_get_iph_len(ip_packet) + data_len);
}

int ip_packet_get_iph_len(ip_packet_t *ip_packet)
{
    return ip_packet->iph->ihl * 4;
}

int ip_packet_get_protocol(ip_packet_t *ip_packet)
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

void ip_packet_get_packet_bytes(ip_packet_t *ip_packet, uint8_t *packet_bytes, size_t *total_len)
{
    int iph_len = ip_packet_get_iph_len(ip_packet);
    memcpy(packet_bytes, ip_packet->iph, iph_len);
    memcpy(packet_bytes + iph_len, ip_packet->data, ip_packet->data_len);
    *total_len = ip_packet_get_iph_len(ip_packet) + ip_packet->data_len;
}

void ip_packet_destory(ip_packet_t *ip_packet)
{
    free(ip_packet->iph);
    free(ip_packet);
}