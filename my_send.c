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
#include <arpa/inet.h>  /* inet_addr,inet_aton */
#include <netdb.h>

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;

/**
 * @brief 把主机名、域名解析成ip
 *
 * @param host 主机名
 * @param ip_addr 包含了ip的结构体
 * @return int
 */
int getSockAddr(const char *host, struct sockaddr *ip_addr)
{
    struct addrinfo *result;
    int s = getaddrinfo(host, NULL, NULL, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }
    memcpy(ip_addr, result->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(result); /* No longer needed */
    return 0;
}

int main()
{
    /**
     * @brief 创建IPv4套接字
     * IPPROTO_RAW：可发送任意类型数据包。并设置了IP_HDRINCL，因此需手动创建IP头。
     */
    int send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_socket == -1)
    {
        perror("socket");
        fprintf(stderr, "send socket error: %s\n", strerror(errno));
        return -1;
    }

    /* 解析目的主机名 */
    char *host = "ubuntu2";
    struct sockaddr ip_addr;
    getSockAddr(host, &ip_addr);
    struct sockaddr_in *daddr = (struct sockaddr_in *)(&ip_addr);
    printf("dst ip: %s\n", inet_ntoa(daddr->sin_addr));

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i:%d\n", i);
        /* <netinet/ip.h> 中ip头结构体 */
        struct iphdr ip;
        bzero(&ip, sizeof(struct iphdr));
        /* <netinet/ip.h> 中定义的IPv4版本号：4 */
        ip.version = IPVERSION;
        /* ip头长度，单位：4字节。一般为5*4=20字节 */
        ip.ihl = 5;
        /* 服务类型。<netinet/ip.h>中定义了服务类型宏，为0表示一般类型 */
        ip.tos = 0x00;
        /* ip数据包总长度，最大为IP_MAXPACKET = 65535 */
        ip.tot_len = BUFFER_SIZE;
        /* 标识。如果IP标识为0，内核将设置该字段 */
        ip.id = htonl(i % 65535);
        /* IP_HDRINCL情况下，IP数据包不会自动分片，超过网卡接口MTU将被丢弃。这里表示不分片。 */
        ip.frag_off = htons(0x0000);
        /* <netinet/ip.h> 中定义了各种TTL，这里使用默认TTL */
        ip.ttl = IPDEFTTL;
        /* <netinet/in.h> 中定义了网络层协议号，此处为ICMP协议 */
        ip.protocol = IPPROTO_ICMP;
        /* 校验和由内核计算并添加 */
        /* ip.check */
        /* 源ip。如果不设置，内核将把它设置为外出接口的主IP地址 */
        /* 如果此处源ip与网卡接口ip不一样，也可发送出去，但将无法收到响应（因为响应被发送到错误的ip） */
        ip.saddr = inet_addr("192.168.1.2");
        /* 目的ip。 */
        // ip.daddr = daddr->sin_addr.s_addr;
        ip.daddr = inet_addr("192.168.1.3");

        memcpy(&buffer, &ip, sizeof(struct iphdr));
        /* 设置数据部分 */
        for (int k = 20; k < BUFFER_SIZE; ++k)
        {
            buffer[k] = 'a';
        }

        /* 发送 */
        /* 如果此处目的ip与包的目的ip不一样，也可发送出去，且目的ip仍为包的目的ip。此处的目的ip没有作用，但不可缺少 */
        daddr->sin_addr.s_addr = inet_addr("192.168.1.4");
        int n = sendto(send_socket, buffer, BUFFER_SIZE, 0, &ip_addr, sizeof(struct sockaddr));
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