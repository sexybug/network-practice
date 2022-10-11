#include <stdio.h>
#include "ip_packet.h"

int main(int argc, char **argv)
{
    ip_packet_t *ip=ip_packet_create();
    int len=ip_packet_get_iph_len(ip);
    return 0;
}