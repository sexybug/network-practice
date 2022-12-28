#include "ip_packet.h"

#include <stdlib.h>
#include <string.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>

void memory_dump(void *ptr, int len)
{
    for (int i = 0; i < len; ++i)
    {
        if (i % 8 == 0 && i != 0)
            printf(" ");
        if (i % 16 == 0 && i != 0)
            printf("\n");
        printf("%02x ", *((uint8_t *)ptr + i));
    }
    printf("\n");
}

typedef struct iphdr iphdr;
struct ip_packet_t
{
    /* iphdr中的数据均为网络字节序 */
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

ip_packet_t *ip_packet_create_from_bytes(const uint8_t *packet_bytes, uint16_t total_len)
{
    iphdr *iph = (iphdr *)packet_bytes;
    uint16_t iph_len = iph->ihl * 4;

    ip_packet_t *ip_packet = (ip_packet_t *)malloc(sizeof(ip_packet_t));
    ip_packet->iph = (iphdr *)malloc(iph_len);
    memcpy(ip_packet->iph, iph, iph_len);

    ip_packet->data_len = ntohs(ip_packet->iph->tot_len) - iph_len;
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

uint16_t ip_checksum(const iphdr *iph)
{
    uint16_t *buffer = (uint16_t *)iph;
    uint8_t buf_len = iph->ihl * 4 / 2;

    uint32_t sum = 0;
    for (int i = 0; i < buf_len; i++)
    {
        sum += ntohs(*(buffer + i));
    }
    sum = (sum >> 16) + (sum & 0xffff);
    return (uint16_t)(~sum);
}

void ip_packet_set_check(ip_packet_t *ip_packet)
{
    ip_packet->iph->check = 0;
    ip_packet->iph->check = htons(ip_checksum(ip_packet->iph));
}

void ip_packet_set_check_bezero(ip_packet_t *ip_packet)
{
    ip_packet->iph->check = 0;
}

bool ip_packet_check(ip_packet_t *ip_packet)
{
    return ip_checksum(ip_packet->iph) == 0;
}

void ip_packet_set_saddr(ip_packet_t *ip_packet, const char *saddr)
{
    ip_packet->iph->saddr = inet_addr(saddr);
}

void ip_packet_set_daddr(ip_packet_t *ip_packet, const char *daddr)
{
    ip_packet->iph->daddr = inet_addr(daddr);
}

void ip_packet_set_data(ip_packet_t *ip_packet, const uint8_t *data, uint16_t data_len)
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

uint16_t ip_packet_get_id(ip_packet_t *ip_packet)
{
    return ntohs(ip_packet->iph->id);
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

int ip_packet_send(ip_packet_t *ip_packet)
{
    int send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_socket == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in send_addr;
    memset(&send_addr, 0, sizeof(struct sockaddr_in));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = ip_packet->iph->daddr;

    const int mtu = 1500;
    int iph_len = ip_packet_get_iph_len(ip_packet);
    int data_window = mtu - iph_len;
    uint8_t buffer[mtu];
    // 已发送数据部分总长度
    int send_data_len = 0;
    // 分段数据长度
    int n;
    // 分段头部
    iphdr hdr;
    while (send_data_len < ip_packet->data_len)
    {
        // 初始化分段头部
        memcpy(&hdr, ip_packet->iph, iph_len);

        if ((ip_packet->data_len - send_data_len) >= data_window)
        {
            n = data_window;
            hdr.frag_off = htons(0x2000 | (send_data_len >> 3));
        }
        else
        {
            n = ip_packet->data_len - send_data_len;
            hdr.frag_off = htons(send_data_len >> 3);
        }
        // 修改分段头部
        hdr.tot_len = htons(iph_len + n);
        hdr.check = htons(ip_checksum(&hdr));

        memcpy(buffer, &hdr, iph_len);
        memcpy(buffer + iph_len, ip_packet->data + send_data_len, n);

        memory_dump(buffer, iph_len + n);

        int r = sendto(send_socket, buffer, iph_len + n, 0, (struct sockaddr *)&send_addr, sizeof(struct sockaddr_in));
        if (r < 0)
        {
            fprintf(stderr, "ip_packet_send error: %s\n", strerror(errno));
            return -1;
        }
        printf("send %d bytes data\n", r);
        send_data_len += n;
    }
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