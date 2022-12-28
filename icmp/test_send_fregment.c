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
#include <linux/icmp.h>

int main(int argc, char **argv)
{
    for (int i = 0; i < 1; ++i)
    {
        const char *src_ip = "192.168.40.128";
        const char *dst_ip = "192.168.40.129";

        icmp_t *icmp = icmp_create();
        icmp_set_type(icmp, ICMP_ECHO);
        icmp_set_code(icmp, 0);
        icmp_set_id(icmp, 0x1234);
        icmp_set_seqence(icmp, 0x5678);
        uint8_t icmp_data[1500];
        for(int i=0; i<1500; i++)
        {
            icmp_data[i] = i%256;
        }
        icmp_set_data(icmp, icmp_data, 1500);
        icmp_set_checksum(icmp);

        uint16_t icmp_len = icmp_get_total_len(icmp);
        uint8_t icmp_buf[icmp_len];
        icmp_get_packet_bytes(icmp, icmp_buf);

        ip_packet_t *ip_packet = ip_packet_create();
        ip_packet_set_saddr(ip_packet, src_ip);
        ip_packet_set_daddr(ip_packet, dst_ip);
        ip_packet_set_id(ip_packet, 0x0304);
        ip_packet_set_protocol(ip_packet, IPPROTO_ICMP);
        ip_packet_set_data(ip_packet, icmp_buf, icmp_len);
        ip_packet_send(ip_packet);
        
        sleep(1);
    }
    return 0;
}