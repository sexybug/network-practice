#include "ip/ip_packet.h"
#include "ah/ah_packet.h"
#include "sm3/hmac_sm3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;

void memory_dump(void *ptr, int len)
{
    for (int i = 0; i < len; ++i)
    {
        if (i % 8 == 0 && i != 0)
            printf(" ");
        if (i % 16 == 0 && i != 0)
            printf("\n");
        printf("%02x ", *((uint8_t *)ptr + i));
    }
    printf("\n");
}

void ah_transport_sm3(ip_packet_t *ip_packet, uint8_t *key, int key_len, uint8_t *auth_data)
{
    ip_packet_t *clone = ip_packet_clone(ip_packet);
    clone->iph->tos = 0;
    clone->iph->frag_off = 0;
    clone->iph->ttl = 0;
    clone->iph->check = 0;

    uint8_t buf[BUFFER_SIZE];
    size_t len = 0;
    ip_packet_get_packet_bytes(clone, buf, &len);

    printf("clone:\n");
    memory_dump(buf, len);

    sm3_hmac(buf, len, key, key_len, auth_data);
    printf("hmac-sm3:\n");
    memory_dump(auth_data, 32);
}

int main(int argc, char **argv)
{
    for (int i = 0; i < 1; ++i)
    {

        const char *src_ip = "192.168.206.131";
        const char *dst_ip = "192.168.206.132";

        ah_packet_t *ah_packet = ah_packet_create(32);
        ah_packet_set_nexthdr(ah_packet, IPPROTO_ICMP);
        ah_packet_set_seq_no(ah_packet, 0x0102);
        //还需set_id
        ah_packet_set_spi(ah_packet, 0x0304);
        ah_packet_set_data(ah_packet, "abcd1234abcd1234abcd1234abcd1234", 32);

        unsigned char ah_buffer[BUFFER_SIZE];
        size_t ah_len = 0;
        ah_packet_get_packet_bytes(ah_packet, ah_buffer, &ah_len);

        ip_packet_t *ip_packet = ip_packet_create();
        ip_packet_set_saddr(ip_packet, src_ip);
        ip_packet_set_daddr(ip_packet, dst_ip);
        ip_packet_set_protocol(ip_packet, IPPROTO_AH);
        ip_packet_set_data(ip_packet, ah_buffer, ah_len);

        /* 改成先获取长度，再获取数据指针*/
        //计算IP/AH 传输模式数据包hmac-sm3
        uint8_t auth_data[32];
        ah_transport_sm3(ip_packet, "123", 3, auth_data);
        ah_packet_set_auth_data(ah_packet, auth_data);
        ah_packet_get_packet_bytes(ah_packet, ah_buffer, &ah_len);
        ip_packet_set_data(ip_packet, ah_buffer, ah_len);

        unsigned char ip_buffer[BUFFER_SIZE];
        size_t total_len = 0;
        ip_packet_get_packet_bytes(ip_packet, ip_buffer, &total_len);
        printf("ip_packet:\n");
        memory_dump(ip_buffer, total_len);

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
        int n = sendto(send_socket, ip_buffer, total_len, 0, (struct sockaddr *)&send_addr, sizeof(struct sockaddr_in));
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