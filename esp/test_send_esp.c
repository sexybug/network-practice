#include "esp.h"
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
    for (int i = 0; i < 1; ++i)
    {

        const char *src_ip = "192.168.40.128";
        const char *dst_ip = "192.168.40.129";

        esp_t *esp = esp_create();
        esp_set_spi(esp, 0x01020304);
        esp_set_seq_no(esp, 0x05060708);
        esp_set_payload(esp, "payload1payload2payload3payload4", 32);
        esp_set_padding(esp, "0000000000000000", 16);
        esp_set_nexthdr(esp, IPPROTO_ICMP);
        esp_set_icv(esp, "abc1abc2abc3abc4", 16);

        size_t esp_len = esp_get_packet_len(esp);
        uint8_t esp_buffer[esp_len];
        esp_get_packet_bytes(esp, esp_buffer);

        ip_packet_t *ip_packet = ip_packet_create();
        ip_packet_set_saddr(ip_packet, src_ip);
        ip_packet_set_daddr(ip_packet, dst_ip);
        ip_packet_set_protocol(ip_packet, IPPROTO_ESP);
        ip_packet_set_data(ip_packet, esp_buffer, esp_len);

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