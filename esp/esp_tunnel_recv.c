#include "esp_packet.h"
#include "../ip/ip_packet.h"
#include "../sm4/openssl_sm4.h"
#include "../sm3/hmac_sm3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

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

        esp_packet_t *esp = esp_packet_create_from_bytes(esp_buf, esp_len, 32);
        printf("esp header\n");
        printf("spi: %08x\n", esp_packet_get_spi(esp));
        printf("seq_no: %08x\n", esp_packet_get_seq_no(esp));
        printf("data:\n");
        size_t data_len = esp_packet_get_data_len(esp);
        uint8_t data[data_len];
        esp_packet_get_data(esp, data);
        memory_dump(data, data_len);

        printf("icv:\n");
        size_t icv_len = esp_packet_get_icv_len(esp);
        uint8_t icv[icv_len];
        esp_packet_get_icv(esp, icv);
        memory_dump(icv, icv_len);

        uint8_t key[] = "1234567812345678";
        int key_len = 16;
        uint8_t iv[] = "1234567812345678";
        // 计算完整性校验值
        int auth_data_len = esp_packet_get_auth_data_len(esp);
        uint8_t auth_data[auth_data_len];
        esp_packet_get_auth_data(esp, auth_data);
        uint8_t new_icv[32];
        sm3_hmac(auth_data, auth_data_len, key, key_len, new_icv);
        // 比较完整性校验值
        printf("icv check result:%d\n", memcmp(new_icv, icv, icv_len));

        // 解密
        uint8_t plain_data[data_len];
        int plain_data_len = 0;
        sm4_cbc_dec(key, iv, data, data_len, plain_data, &plain_data_len);
        printf("plain_data:\n");
        memory_dump(plain_data, plain_data_len);

        // 解析内层IP包
        ip_packet_t *inner_ip = ip_packet_create_from_bytes(plain_data, plain_data_len);
        printf("\ninner_ip_packet:\n");
        printf("source ip: %s\n", ip_packet_get_saddr(inner_ip));
        printf("destination ip: %s\n", ip_packet_get_daddr(inner_ip));
        printf("protocol: %d\n", ip_packet_get_protocol(inner_ip));
        printf("total len: %d\n", ip_packet_get_packet_len(inner_ip));
        printf("header len: %d\n", ip_packet_get_iph_len(inner_ip));

        int inner_data_len = ip_packet_get_data_len(inner_ip);
        uint8_t inner_data[inner_data_len];
        ip_packet_get_data(inner_ip, inner_data);
        printf("inner ip data:(len:%d)\n", inner_data_len);
        memory_dump(inner_data, inner_data_len);

        ++i;
    }
    return 0;
}