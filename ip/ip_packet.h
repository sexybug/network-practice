#ifndef IP_PACKET_H
#define IP_PACKET_H

#include <stdint.h>
#include <stdbool.h>

typedef struct ip_packet_t ip_packet_t;

ip_packet_t *ip_packet_create();
ip_packet_t *ip_packet_create_from_bytes(const uint8_t *packet_bytes, uint16_t total_len);

/* 服务类型字段(8位) */
void ip_packet_set_tos(ip_packet_t *ip_packet, uint8_t tos);
/* 标识字段(16位)*/
void ip_packet_set_id(ip_packet_t *ip_packet, uint16_t id);
/* 片偏移(16位)，3位分段标志+13位分段偏移数 */
void ip_packet_set_frag_off(ip_packet_t *ip_packet, uint16_t frag_off);
/* 数据报生存时间(8位) */
void ip_packet_set_ttl(ip_packet_t *ip_packet, uint8_t ttl);
/* 协议字段(8位) */
void ip_packet_set_protocol(ip_packet_t *ip_packet, uint8_t protocol);
/* 计算并填充首部校验和(16位) */
void ip_packet_set_check(ip_packet_t *ip_packet);
/* 首部校验和置0 */
void ip_packet_set_check_bezero(ip_packet_t *ip_packet);
/* 校验首部校验和,成功返回true(即1) */
bool ip_packet_check(ip_packet_t *ip_packet);
/* 源IP地址 */
void ip_packet_set_saddr(ip_packet_t *ip_packet, const char *saddr);
/* 目的IP地址 */
void ip_packet_set_daddr(ip_packet_t *ip_packet, const char *daddr);
/* 数据部分 */
void ip_packet_set_data(ip_packet_t *ip_packet, const uint8_t *data, uint16_t data_len);

/* 获取IP头长度，单位：字节 */
uint16_t ip_packet_get_iph_len(ip_packet_t *ip_packet);
uint8_t ip_packet_get_protocol(ip_packet_t *ip_packet);
uint16_t ip_packet_get_id(ip_packet_t *ip_packet);
char *ip_packet_get_saddr(ip_packet_t *ip_packet);
char *ip_packet_get_daddr(ip_packet_t *ip_packet);
/* 获取数据部分长度 */
uint16_t ip_packet_get_data_len(ip_packet_t *ip_packet);
/* 获取数据部分 */
void ip_packet_get_data(ip_packet_t *ip_packet, uint8_t *data);
/* 获取整个IP数据包总长度 */
uint16_t ip_packet_get_packet_len(ip_packet_t *ip_packet);
/* 获取整个数据包（包括IP头部和数据部分）所有字节的数据 */
void ip_packet_get_packet_bytes(ip_packet_t *ip_packet, uint8_t *packet_bytes);

/* clone整个IP包，包括IP头和数据部分 */
ip_packet_t *ip_packet_clone(ip_packet_t *ip_packet);

/* 调用create,create_from,clone方法创建的对象，必须用destory销毁 */
void ip_packet_destory(ip_packet_t *ip_packet);

#endif /* IP_PACKET_H */