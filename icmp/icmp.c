#include "icmp.h"
#include <stdlib.h>
#include <string.h>
#include <linux/icmp.h>
#include <arpa/inet.h>

typedef struct icmphdr icmphdr;

struct icmp_t
{
    icmphdr *hdr;
    uint8_t *data;
    uint16_t data_len;
};

icmp_t *icmp_create()
{
    icmp_t *icmp = (icmp_t *)malloc(sizeof(icmp_t));
    icmp->hdr = (icmphdr *)malloc(sizeof(icmphdr));
    memset(icmp->hdr, 0, sizeof(icmphdr));
    icmp->data = NULL;
    icmp->data_len = 0;

    return icmp;
}

icmp_t *icmp_create_from_bytes(const uint8_t *packet_bytes, uint16_t total_len)
{
    icmp_t *icmp=(icmp_t *)malloc(sizeof(icmp_t));
    icmp->hdr=(icmphdr *)malloc(sizeof(icmphdr));
    memcpy(icmp->hdr,packet_bytes,sizeof(icmphdr));

    uint16_t data_len=total_len-sizeof(icmphdr);
    icmp->data=(uint8_t *)malloc(data_len);
    memcpy(icmp->data,packet_bytes+sizeof(icmphdr),data_len);
    icmp->data_len=data_len;
}

void icmp_set_type(icmp_t *icmp, uint8_t type)
{
    icmp->hdr->type = type;
}

void icmp_set_code(icmp_t *icmp, uint8_t code)
{
    icmp->hdr->code = code;
}

void icmp_set_id(icmp_t *icmp, uint16_t id)
{
    icmp->hdr->un.echo.id = htons(id);
}

void icmp_set_seqence(icmp_t *icmp, uint16_t seqence)
{
    icmp->hdr->un.echo.sequence = htons(seqence);
}

void icmp_set_data(icmp_t *icmp, const uint8_t *data, uint16_t data_len)
{
    if (icmp->data != NULL)
    {
        free(icmp->data);
        icmp->data = NULL;
    }
    icmp->data = (uint8_t *)malloc(data_len);
    memcpy(icmp->data, data, data_len);
    icmp->data_len = data_len;
}

/**
 * @brief 计算校验和
 * 
 * @param buffer 需校验数据
 * @param size 总字节数
 * @return uint16_t 
 */
uint16_t checksum(const uint16_t *buffer, int size)
{
    uint32_t sum = 0;
    while (size > 1)
    {
        sum += htons(*buffer);
        buffer++;
        size -= 2;
    }
    if (size == 1)
    {
        sum += *((uint8_t *)buffer);
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

/* 计算并填充首部校验和(16位) */
void icmp_set_checksum(icmp_t *icmp)
{
    uint16_t packet_size=icmp_get_total_len(icmp);
    uint8_t buf[packet_size];
    icmp_get_packet_bytes(icmp,buf);
    uint16_t sum=checksum((uint16_t *)buf,packet_size);
    icmp->hdr->checksum=htons(sum);
}

/* 校验首部校验和,成功返回true(即1) */
bool icmp_check(icmp_t *icmp)
{
    uint16_t packet_size=icmp_get_total_len(icmp);
    uint8_t buf[packet_size];
    icmp_get_packet_bytes(icmp,buf);
    return checksum((uint16_t *)buf,packet_size)==0;
}

uint8_t icmp_get_type(icmp_t *icmp)
{
    return icmp->hdr->type;
}

uint16_t icmp_get_total_len(icmp_t *icmp)
{
    return sizeof(icmphdr) + icmp->data_len;
}

void icmp_get_packet_bytes(icmp_t *icmp, uint8_t *buf)
{
    memcpy(buf, icmp->hdr, sizeof(icmphdr));
    memcpy(buf + sizeof(icmphdr), icmp->data, icmp->data_len);
}

void icmp_destory(icmp_t *icmp)
{
    if (icmp->data != NULL)
    {
        free(icmp->data);
        icmp->data = NULL;
    }
    free(icmp->hdr);
    icmp->hdr = NULL;
    free(icmp);
    icmp = NULL;
}
