#include "icmp.h"
#include "../ip/ip_packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;

int main(int argc, char **argv)
{
    for (int i = 0; i < 2; ++i)
    {

        const char *src_ip = "192.168.206.131";
        const char *dst_ip = "192.168.206.132";

        icmp_t *icmp = icmp_create();
        icmp_set_type(icmp, 8);
        icmp_set_code(icmp, 0);
        icmp_set_id(icmp, 0x1234);
        icmp_set_seqence(icmp, 0x5678);
        icmp_set_data(icmp, "abcd1234abcd1234abcd1234abcd1234", 32);
        icmp_set_checksum(icmp);

        uint16_t icmp_len = icmp_get_total_len(icmp);
        uint8_t icmp_buf[icmp_len];
        icmp_get_packet_bytes(icmp, icmp_buf);

        ip_packet_t *ip_packet = ip_packet_create();
        ip_packet_set_saddr(ip_packet, src_ip);
        ip_packet_set_daddr(ip_packet, dst_ip);
        ip_packet_set_protocol(ip_packet, IPPROTO_ICMP);
        ip_packet_set_data(ip_packet, icmp_buf, icmp_len);

        size_t ip_len = ip_packet_get_packet_len(ip_packet);
        unsigned char ip_buffer[ip_len];
        ip_packet_get_packet_bytes(ip_packet, ip_buffer);

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

        struct sockaddr_in send_addr;
        memset(&send_addr, 0, sizeof(struct sockaddr_in));
        send_addr.sin_family = AF_INET;
        send_addr.sin_addr.s_addr = inet_addr(dst_ip);
        /* 发送 */
        /* 如果此处目的ip与包的目的ip不一样，也可发送出去，且目的ip仍为包中设定的目的ip。此处的目的ip没有作用，但不可缺少 */
        int n = sendto(send_socket, ip_buffer, ip_len, 0, (struct sockaddr *)&send_addr, sizeof(struct sockaddr_in));
        if (n < 0)
        {
            fprintf(stderr, "sendto error: %s\n", strerror(errno));
            return -1;
        }
        printf("send %d bytes\n", n);
        sleep(1);
    }
    return 0;
}