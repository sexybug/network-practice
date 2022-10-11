/* 接收来源于特定ip的、特定协议的数据包 */
#include "ah_packet.h"
#include "../ip/ip_packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  /* inet_addr,inet_aton */

#define BUFFER_SIZE 65535

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

int main()
{
    const char *expect_src_ip = "192.168.206.131";

    /* 只要是IPv4数据包（AF_INET），并且IP头中协议字段是ICMP，则都能接收到 */
    int recv_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recv_socket == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(struct sockaddr_in));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(expect_src_ip);
    int recv_addr_len = sizeof(struct sockaddr_in);

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i: %d\n", i);

        int n = recvfrom(recv_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&recv_addr, &recv_addr_len);
        if (n < 0)
        {
            perror("recvfrom");
        }

        ip_packet_t *ip=ip_packet_create_from_bytes(buffer,n);

        printf("packet size: %d\n", n);
        printf("source ip: %s\n", ip_packet_get_saddr(ip));
        printf("destination ip: %s\n", ip_packet_get_daddr(ip));
        printf("protocol: %d\n", ip_packet_get_protocol(ip));
        printf("\n");

        unsigned char ah_buf[BUFFER_SIZE];
        size_t ah_len=0;
        ip_packet_get_data(ip,ah_buf,&ah_len);

        ah_packet_t *ah_packet=ah_packet_create_from_bytes(ah_buf,ah_len);
        printf("ah header\n");
        printf("Next header: %d\n", ah_packet_get_nexthdr(ah_packet));
        printf("Length: %d\n", ah_packet_get_auth_hdr_len(ah_packet));
        printf("AH SPI: 0x%08x\n", ah_packet_get_spi(ah_packet));
        printf("AH Sequence: %d\n", ah_packet_get_spi(ah_packet));
        printf("AH ICV:\n");
        unsigned char auth_data[BUFFER_SIZE];
        size_t auth_data_len = 0;
        ah_packet_get_auth_data(ah_packet,auth_data,&auth_data_len);
        memory_dump(auth_data,auth_data_len);
        printf("\n");

        ++i;
    }
    return 0;
}