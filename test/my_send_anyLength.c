/* 发送IP数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>  /* superset of previous */
#include <arpa/inet.h> /* inet_addr,inet_aton */

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 3000;
/* 无可选字段的IP头长度为20byte */
const int IP_HDR_LEN = 20;

int main()
{
    const char *src_ip = "192.168.40.128";
    const char *dst_ip = "192.168.40.129";

    int send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_socket == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in send_addr;
    memset(&send_addr, 0, sizeof(struct sockaddr_in));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr(dst_ip);

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i:%d\n", i);
        /* <netinet/ip.h> 中ip头结构体 */
        struct iphdr ip;
        memset(&ip, 0, sizeof(struct iphdr));
        /* <netinet/ip.h> 中定义的IPv4版本号：4 */
        ip.version = IPVERSION;
        /* ip头长度，单位：4字节。一般为5*4=20字节 */
        ip.ihl = IP_HDR_LEN / 4;
        /* 服务类型。<netinet/ip.h>中定义了服务类型宏，为0表示一般类型 */
        ip.tos = 0x00;
        /* ip数据包总长度，以字节为单位，最大为IP_MAXPACKET = 65535 */
        ip.tot_len = htons(BUFFER_SIZE);
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
        ip.saddr = inet_addr(src_ip);
        /* 目的ip。 */
        // ip.daddr = daddr->sin_addr.s_addr;
        ip.daddr = inet_addr(dst_ip);

        memcpy(buffer, &ip, IP_HDR_LEN);
        /* 设置数据部分 */
        for (int k = IP_HDR_LEN; k < BUFFER_SIZE; ++k)
        {
            buffer[k] = '0' + k % 10;
        }

        /* 发送 */
        int send_bytes = 0;

        ++i;
        /* 睡眠一秒 */
        sleep(1);
    }
    return 0;
}