#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */
#include <netdb.h>

void print_family(struct addrinfo *aip)
{
    printf(" family ");
    switch (aip->ai_family)
    {
    case AF_INET:
        printf("inet");
        break;
    case AF_INET6:
        printf("inet6");
        break;
    case AF_UNIX:
        printf("unix");
        break;
    case AF_UNSPEC:
        printf("unspecified");
        break;
    default:
        printf("unknown");
    }
}
void print_type(struct addrinfo *aip)
{
    printf(" type ");
    switch (aip->ai_socktype)
    {
    case SOCK_STREAM:
        printf("stream");
        break;
    case SOCK_DGRAM:
        printf("datagram");
        break;
    case SOCK_SEQPACKET:
        printf("seqpacket");
        break;
    case SOCK_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)", aip->ai_socktype);
    }
}
void print_protocol(struct addrinfo *aip)
{
    printf(" protocol ");
    switch (aip->ai_protocol)
    {
    case 0:
        printf("default");
        break;
    case IPPROTO_TCP:
        printf("TCP");
        break;
    case IPPROTO_UDP:
        printf("UDP");
        break;
    case IPPROTO_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)", aip->ai_protocol);
    }
}
void print_flags(struct addrinfo *aip)
{
    printf("flags");
    if (aip->ai_flags == 0)
    {
        printf(" 0");
    }
    else
    {
        if (aip->ai_flags & AI_PASSIVE)
            printf(" passive");
        if (aip->ai_flags & AI_CANONNAME)
            printf(" canon");
        if (aip->ai_flags & AI_NUMERICHOST)
            printf(" numhost");
#if defined(AI_NUMERICSERV)
        if (aip->ai_flags & AI_NUMERICSERV)
            printf(" numserv");
#endif
#if defined(AI_V4MAPPED)
        if (aip->ai_flags & AI_V4MAPPED)
            printf(" v4mapped");
#endif
#if defined(AI_ALL)
        if (aip->ai_flags & AI_ALL)
            printf(" all");
#endif
    }
}

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    //这些设置等于hints为NULL时的设置
    /* Obtain address(es) matching host/port. */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_IP; /* Any protocol */

    // int s = getaddrinfo("ubuntu", "ssh", &hints, &result);
    int s = getaddrinfo("192.168.206.132", NULL, NULL, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    /* getaddrinfo() returns a list of address structures.
              Try each address until we successfully bind(2).
              If socket(2) (or bind(2)) fails, we (close the socket
              and) try the next address. */

    printf("begin\n");
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        print_flags(rp);
        print_family(rp);
        print_type(rp);
        print_protocol(rp);

        if (ntohs(rp->ai_addr->sa_family) == AF_INET)
        {
            printf("ipv4\n");
        }
        if (ntohs(rp->ai_addr->sa_family) == AF_INET6)
        {
            printf("ipv6\n");
        }
        printf("\naf_net %d\n", rp->ai_addr->sa_family);
        struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
        printf("address: %s\n", inet_ntoa(addr->sin_addr));
        printf("port: %d\n", ntohs(addr->sin_port));
    }
    freeaddrinfo(result); /* No longer needed */

    return 0;
}