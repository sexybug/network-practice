#include "ip_packet.h"
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

        ip_packet_t *ip_packet = ip_packet_create();
        ip_packet_set_saddr(ip_packet, src_ip);
        ip_packet_set_daddr(ip_packet, dst_ip);
        ip_packet_set_protocol(ip_packet, IPPROTO_ICMP);
        // 必须指定id，用于确定分段
        ip_packet_set_id(ip_packet, 0x0102);

        uint8_t data[1500];
        for (int i = 0; i < 1500; ++i)
        {
            data[i] = i % 256;
        }
        ip_packet_set_data(ip_packet, data, 1500);

        ip_packet_send(ip_packet);
        sleep(1);
    }
    return 0;
}