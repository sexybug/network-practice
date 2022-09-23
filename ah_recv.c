/* 接收AH数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */
#include "ah.h"

void memory_dump(void *ptr, int len) {
    
    for (int i = 0; i < len; ++i) {
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

    /* 接收协议字段为AH的IP数据包 */
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

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct in_addr ip_src, ip_dst; /* source and dest address */
        ip_src.s_addr = ip_header->saddr;
        ip_dst.s_addr = ip_header->daddr;

        printf("ipv4 header:\n");
        printf("packet size: %d\n", n);
        printf("source ip: %s\n", inet_ntoa(ip_src));
        printf("destination ip: %s\n", inet_ntoa(ip_dst));
        printf("protocol: %d\n", ip_header->protocol);
        printf("\n");

        struct ip_auth_hdr *ah_header=(struct ip_auth_hdr *)(buffer+ IP_HDR_LEN);
        printf("ah header:\n");
        printf("Next header: %d\n", ah_header->nexthdr);
        printf("Length: %d\n", (ah_header->hdrlen+2)*4);
        printf("AH SPI:\n");
        memory_dump(ah_header->spi,4);
        printf("AH Sequence: %d\n", ntohl(ah_header->seq_no));
        printf("AH ICV:\n");
        memory_dump(ah_header->auth_data,32);
        printf("\n");

        ++i;
    }
}