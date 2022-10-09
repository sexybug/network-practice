/* 接收来源于特定ip的、特定协议的数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */

#define BUFFER_SIZE 65535
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

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct in_addr ip_src, ip_dst; /* source and dest address */
        ip_src.s_addr = ip_header->saddr;
        ip_dst.s_addr = ip_header->daddr;

        printf("packet size: %d\n", n);
        printf("source ip: %s\n", inet_ntoa(ip_src));
        printf("destination ip: %s\n", inet_ntoa(ip_dst));
        printf("protocol: %d\n", ip_header->protocol);
        printf("\n");
        ++i;
    }
}