#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/ipsec.h>
#include <linux/xfrm.h>

int main()
{
    struct ether_header e;
    struct ether_header e2;
    struct iphdr e3;
    struct ifreq if1;

    int domain = AF_PACKET;
    int type = SOCK_RAW;
    int protocol;
    int socket_fd = socket(domain, type, protocol);

    struct addrinfo *result, *rp;
    int s = getaddrinfo(NULL, "ssh", NULL, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */
    return 0;
}