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

        printf("packet size: %d\n", n);
        printf("source ip: %s\n", inet_ntoa(ip_src));
        printf("destination ip: %s\n", inet_ntoa(ip_dst));
        printf("protocol: %d\n", ip_header->protocol);
        printf("\n");
        ++i;
    }
}