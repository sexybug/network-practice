/* 解析主机名或域名 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> /* inet_addr,inet_aton */
#include <netdb.h>

/**
 * @brief 把主机名、域名解析成ip
 *
 * @param host 主机名
 * @param ip_addr 包含了ip的结构体
 * @return int
 */
int getSockAddr(const char *host, struct sockaddr *ip_addr)
{
    struct addrinfo *result;
    int s = getaddrinfo(host, NULL, NULL, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }
    memcpy(ip_addr, result->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(result); /* No longer needed */
    return 0;
}

int main()
{
    /* 解析目的主机名 */
    char *host = "baidu.com";
    struct sockaddr ip_addr;
    getSockAddr(host, &ip_addr);
    struct sockaddr_in *daddr = (struct sockaddr_in *)(&ip_addr);
    printf("ip: %s\n", inet_ntoa(daddr->sin_addr));

    return 0;
}
