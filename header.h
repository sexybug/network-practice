#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>  /* inet_addr,inet_aton */

#include <linux/ip.h>

#include <stdint.h>

#include <netinet/ip_icmp.h>

#include <linux/icmp.h>