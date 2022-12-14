/* 接收来源于特定ip的、特定协议的数据包 */
#include "esp.h"
#include "../ip/ip_packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */

#define BUFFER_SIZE 65535

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

int main()
{
    const char *expect_src_ip = "192.168.206.131";

    /* 只要是IPv4数据包（AF_INET），并且IP头中协议字段是ICMP，则都能接收到 */
    int recv_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ESP);
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

        ip_packet_t *ip = ip_packet_create_from_bytes(buffer, n);

        printf("packet size: %d\n", n);
        printf("source ip: %s\n", ip_packet_get_saddr(ip));
        printf("destination ip: %s\n", ip_packet_get_daddr(ip));
        printf("protocol: %d\n", ip_packet_get_protocol(ip));
        printf("\n");

        size_t esp_len = ip_packet_get_data_len(ip);
        uint8_t esp_buf[esp_len];
        ip_packet_get_data(ip, esp_buf);


        esp_t *esp = esp_create_from_bytes(esp_buf, esp_len,16);
        printf("esp header\n");
        printf("spi: %08x\n", esp_get_spi(esp));
        printf("seq_no: %08x\n", esp_get_seq_no(esp));
        printf("payload:\n");
        size_t payload_len = esp_get_payload_len(esp);
        uint8_t payload[payload_len];
        esp_get_payload(esp, payload);
        memory_dump(payload, payload_len);
        
        printf("padding:\n");
        size_t pad_len = esp_get_pad_len(esp);
        uint8_t padding[pad_len];
        esp_get_padding(esp, padding);
        memory_dump(padding, pad_len);

        printf("pad_len: %d\n", pad_len);

        printf("nexthdr: %d\n", esp_get_nexthdr(esp));

        printf("icv:\n");
        size_t icv_len = esp_get_icv_len(esp);
        uint8_t icv[icv_len];
        esp_get_icv(esp, icv);
        memory_dump(icv, icv_len);

        ++i;
    }
    return 0;
}