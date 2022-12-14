#include "ip/ip_packet.h"
#include "ah/ah_packet.h"
#include "sm3/hmac_sm3.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;

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
    int recv_socket = socket(AF_INET, SOCK_RAW, IPPROTO_AH);
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

        ip_packet_t *ip_packet = ip_packet_create_from_bytes(buffer, n);
        printf("ip_packet:\n");
        memory_dump(buffer, n);

        printf("packet size: %d\n", n);
        printf("source ip: %s\n", ip_packet_get_saddr(ip_packet));
        printf("destination ip: %s\n", ip_packet_get_daddr(ip_packet));
        printf("protocol: %d\n", ip_packet_get_protocol(ip_packet));
        printf("\n");

        size_t ah_len = ip_packet_get_data_len(ip_packet);
        uint8_t ah_buf[ah_len];
        ip_packet_get_data(ip_packet, ah_buf);

        ah_packet_t *ah_packet = ah_packet_create_from_bytes(ah_buf, ah_len);
        printf("ah header\n");
        printf("Next header: %d\n", ah_packet_get_nexthdr(ah_packet));
        printf("Length: %d\n", ah_packet_get_auth_hdr_len(ah_packet));
        printf("AH SPI: 0x%08x\n", ah_packet_get_spi(ah_packet));
        printf("AH Sequence: %d\n", ah_packet_get_seq_no(ah_packet));

        size_t inner_ip_len = ah_packet_get_data_len(ah_packet);
        uint8_t inner_ip_buf[inner_ip_len];
        ah_packet_get_data(ah_packet, inner_ip_buf);

        ip_packet_t *inner_ip = ip_packet_create_from_bytes(inner_ip_buf, inner_ip_len);
        printf("source ip: %s\n", ip_packet_get_saddr(inner_ip));
        printf("destination ip: %s\n", ip_packet_get_daddr(inner_ip));
        printf("protocol: %d\n", ip_packet_get_protocol(inner_ip));

        ++i;
    }
    return 0;
}