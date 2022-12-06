#ifndef _ICMP_H
#define _ICMP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct icmp_t icmp_t;

icmp_t *icmp_create();
icmp_t *icmp_create_from_bytes(const uint8_t *packet_bytes, uint16_t total_len);

void icmp_set_type(icmp_t *icmp,uint8_t type);
void icmp_set_code(icmp_t *icmp,uint8_t code);
void icmp_set_id(icmp_t *icmp,uint16_t id);
void icmp_set_seqence(icmp_t *icmp,uint16_t seqence);
void icmp_set_data(icmp_t *icmp,const uint8_t *data,uint16_t data_len);
/* 计算并填充ICMP包校验和(16位) */
void icmp_set_checksum(icmp_t *icmp);

/* 校验首部校验和,成功返回true(即1) */
bool icmp_check(icmp_t *icmp);
uint8_t icmp_get_type(icmp_t *icmp);
uint16_t icmp_get_total_len(icmp_t *icmp);
void icmp_get_packet_bytes(icmp_t *icmp, uint8_t *buf);


void icmp_destory(icmp_t *icmp);

#endif /* _ICMP_H */