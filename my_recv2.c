#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */

#define BUFFER_SIZE 65535
int main()
{
    /**
     * @brief
     * ETH_P_IP - 只接收目的mac是本机的IP类型数据帧
     * ETH_P_ARP - 只接收目的mac是本机的ARP类型数据帧
     * ETH_P_RARP - 只接收目的mac是本机的RARP类型数据帧
     * ETH_P_PAE - 只接收目的mac是本机的802.1x类型的数据帧
     * ETH_P_ALL - 接收目的mac是本机的所有类型数据帧，同时还可以接收本机发出的所有数据帧，混杂模式打开时，还可以接收到目的mac不是本机的数据帧.
     * ETH_P_ALL可与SOCK_DGRAM一起用，但效果未知。经测试，猜测也是只接收IP包。
     * 关于SOCK_DGRAM: https://man7.org/linux/man-pages/man7/packet.7.html
     */
     int recv_socket = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    /* 这种socket无法接收不正确的ICMP包 */
    //int recv_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recv_socket == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(struct sockaddr_in));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr("192.168.1.2");
    int recv_addr_len = sizeof(recv_addr);

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        
        int n = recvfrom(recv_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&recv_addr, &recv_addr_len);
        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct in_addr ip_src, ip_dst; /* source and dest address */
        ip_src.s_addr = ip_header->saddr;
        ip_dst.s_addr = ip_header->daddr;
        if (memcmp(&ip_src, &recv_addr.sin_addr, sizeof(struct in_addr)) == 0)
        {
            printf("i:%d\n", i);
            printf("packet size:%d\n", n);
            printf("source ip:%s\n", inet_ntoa(ip_src));
            printf("destination ip:%s\n", inet_ntoa(ip_dst));
            printf("protocal:%d\n", ip_header->protocol);
            printf("\n");
        }
        ++i;
    }
}