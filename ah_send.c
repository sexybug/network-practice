/* 发送AH数据包 */
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
#include <linux/types.h>

struct ip_auth_hdr {
    /* 下一个头 */
	__u8  nexthdr;

    /* 载荷长度 */
	__u8  hdrlen;		/* This one is measured in 32 bit units! */

    /* 保留 */
	__be16 reserved;

    /* 安全参数索引 */
	__be32 spi;

    /* 序列号 */
	__be32 seq_no;		/* Sequence number */
    
    /*鉴别数据（变长的） */
    /* modified by sexybug to be compatible with c99 */
	__u8  auth_data[];	/* Variable len but >=4. Mind the 64 bit alignment! */
};

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;
/* 无可选字段的IP头长度为20byte */
const int IP_HDR_LEN = 20;

int main()
{
    const char *src_ip = "192.168.206.131";
    const char *dst_ip = "192.168.206.132";
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
        /* <netinet/ip.h> 中ip头结构体 */
        struct iphdr ip;
        memset(&ip, 0, sizeof(struct iphdr));
        /* <netinet/ip.h> 中定义的IPv4版本号：4 */
        ip.version = IPVERSION;
        /* ip头长度，单位：4字节。一般为5*4=20字节 */
        ip.ihl = IP_HDR_LEN / 4;
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
        ip.protocol = IPPROTO_AH;
        /* 校验和由内核计算并添加 */
        /* ip.check */
        /* 源ip。如果不设置，内核将把它设置为外出接口的主IP地址 */
        /* 如果此处源ip与网卡接口ip不一样，也可发送出去，但将无法收到响应（因为响应被发送到错误的ip） */
        ip.saddr = inet_addr(src_ip);
        /* 目的ip。 */
        // ip.daddr = daddr->sin_addr.s_addr;
        ip.daddr = inet_addr(dst_ip);
        memcpy(buffer, &ip, IP_HDR_LEN);

        /* 设置ah头 */
        struct ip_auth_hdr *ah;
        /* 假设使用hmac(sm3),摘要长度256bit=32byte */
        int auth_data_len = 32;
        int auth_hdr_len = sizeof(struct ip_auth_hdr) + auth_data_len;
        ah = (struct ip_auth_hdr *)malloc(auth_hdr_len);
        if(ah == NULL)
        {
            return -1;
        }
        memset(ah, 0, auth_hdr_len);
        /* 假设数据部分使用ICMP协议 */
        ah->nexthdr = IPPROTO_ICMP;
        /* ah头长度 */
        ah->hdrlen = auth_hdr_len / 4 - 2;
        /* ah->spi */
        /* 序列号，从0开始 */
        ah->seq_no = 0;
        /* 鉴别数据 */
        strcpy(ah->auth_data, "authdat1authdat2authdat3authdat4");
        memcpy(buffer + IP_HDR_LEN, ah, auth_hdr_len);

        /* 设置数据部分 */
        for (int k = IP_HDR_LEN + auth_hdr_len; k < BUFFER_SIZE; ++k)
        {
            buffer[k] = k % 256;
        }

        /* 发送 */
        struct sockaddr_in send_addr;
        memset(&send_addr, 0, sizeof(struct sockaddr_in));
        send_addr.sin_family = AF_INET;
        send_addr.sin_addr.s_addr = inet_addr(dst_ip);
        /* 如果此处目的ip与包的目的ip不一样，也可发送出去，且目的ip仍为包中设定的目的ip。此处的目的ip没有作用，但不可缺少 */
        int n = sendto(send_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&send_addr, sizeof(struct sockaddr_in));
        if (n < 0)
        {
            fprintf(stderr, "sendto error: %s\n", strerror(errno));
            return -1;
        }
        printf("send %d bytes\n", n);
        
        free(ah);
        ++i;
        /* 睡眠一秒 */
        sleep(1);
    }
}