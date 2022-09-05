#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */

const int BUFFER_SIZE = 65535;

int main()
{
    int send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_socket == -1)
    {
        perror("socket");
        fprintf(stderr, "send socket error: %s\n", strerror(errno));
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    int i = 1;
    while (true)
    {
        printf("i:%d\n", i);
        int n = recvfrom(send_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        struct iphdr *ip_header = (struct iphdr *)buffer;
        printf("packet size:%d\n", n);
        struct in_addr ip_src, ip_dst; /* source and dest address */
        ip_src.s_addr = ip_header->saddr;
        ip_dst.s_addr = ip_header->daddr;
        printf("source ip:%s\n", inet_ntoa(ip_src));
        printf("destination ip:%s\n", inet_ntoa(ip_dst));
        printf("protocal:%d\n", ip_header->protocol);
        printf("\n");
        ++i;
    }
}