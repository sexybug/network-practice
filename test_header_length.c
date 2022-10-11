/* 发送ICMP数据包 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h> /* superset of previous */
#include <netinet/ip_icmp.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */
#include <netdb.h>

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;

int main()
{
    struct ether_header eth_hdr;
    sizeof(struct ether_header);

    struct iphdr ip;
    sizeof(struct iphdr);

    struct icmphdr icmp;
    sizeof(struct icmphdr);
    return 0;
}