/* 接收所有数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */

#define BUFFER_SIZE 65535
int main()
{
    /**
     * @brief 创建套接字
     * ETH_P_IP - 只接收目的mac是本机的IP类型数据帧
     * ETH_P_ARP - 只接收目的mac是本机的ARP类型数据帧
     * ETH_P_RARP - 只接收目的mac是本机的RARP类型数据帧
     * ETH_P_PAE - 只接收目的mac是本机的802.1x类型的数据帧
     * ETH_P_ALL - 接收目的mac是本机的所有类型数据帧，同时还可以接收本机发出的所有数据帧，混杂模式打开时，还可以接收到目的mac不是本机的数据帧.
     * ETH_P_ALL可与SOCK_DGRAM一起用，但效果未知。经测试，猜测也是只接收IP包。
     * 关于SOCK_DGRAM: https://man7.org/linux/man-pages/man7/packet.7.html
     * 注意此种socket不支持接收指定ip地址的数据包，必须接收所有数据包
     */
    int recv_socket = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    if (recv_socket == -1)
    {
        perror("socket");
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i: %d\n", i);

        /* AF_PACKET套接字,recvfrom不可指定源ip,需置为NULL */
        /* recvfrom默认是阻塞式的，未获取到数据将一直等待，直到收到数据。可通过setsockopt修改成非阻塞式。 */
        int n = recvfrom(recv_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
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