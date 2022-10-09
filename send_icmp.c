/* 发送ICMP数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h> /* superset of previous */
#include <netinet/ip_icmp.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */
#include <netdb.h>

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;
/* 无可选字段的IP头长度为20byte */
const int IP_HDR_LEN = 20;

int main()
{
    const char *src_ip = "192.168.1.8";
    const char *dst_ip = "192.168.1.7";
    /*
    const char *src_ip = "192.168.206.131";
    const char *dst_ip = "192.168.206.132";
    */

    /**
     * @brief 创建IPv4套接字
     * IPPROTO_RAW：可发送任意类型数据包。并设置了IP_HDRINCL，因此需手动创建IP头。
     */
    int send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_socket == -1)
    {
        perror("socket");
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i:%d\n", i);
        /* 设置ip头 */
        struct iphdr ip;
        memset(&ip, 0, sizeof(struct iphdr));
        ip.version = IPVERSION;
        ip.ihl = IP_HDR_LEN/4;
        ip.tos = 0x00;
        ip.tot_len = BUFFER_SIZE;
        ip.id = htonl(i % 65535);
        ip.frag_off = htons(0x0000);
        ip.ttl = IPDEFTTL;
        ip.protocol = IPPROTO_ICMP;
        ip.saddr = inet_addr(src_ip);
        ip.daddr = inet_addr(dst_ip);

        /* 设置icmp头 */
        struct icmphdr icmp;
        memset(&icmp, 0, sizeof(struct icmphdr));
        icmp.type = ICMP_ECHOREPLY;
        icmp.code = 0;
        icmp.checksum = 0;

        memcpy(buffer, &ip, IP_HDR_LEN);
        /* 设置数据部分 */
        for (int k = IP_HDR_LEN; k < BUFFER_SIZE; ++k)
        {
            buffer[k] = 'a';
        }

        struct sockaddr_in send_addr;
        memset(&send_addr, 0, sizeof(struct sockaddr_in));
        send_addr.sin_family = AF_INET;
        send_addr.sin_addr.s_addr = inet_addr(dst_ip);

        int n = sendto(send_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&send_addr, sizeof(struct sockaddr_in));
        if (n < 0)
        {
            perror("sendto");
            return -1;
        }
        printf("send %d bytes\n", n);
        ++i;
        /* 睡眠一秒 */
        sleep(1);
    }
}