#ifndef _ESP_PACKET_H
#define _ESP_PACKET_H

#include <stdint.h>

typedef struct esp_packet_t esp_packet_t;

esp_packet_t *esp_packet_create();

esp_packet_t *esp_packet_create_from_bytes(const uint8_t *bytes, uint16_t total_len, uint16_t icv_len);

void esp_packet_set_spi(esp_packet_t *esp_packet, uint32_t spi);

void esp_packet_set_seq_no(esp_packet_t *esp_packet, uint32_t seq_no);

void esp_packet_set_data(esp_packet_t *esp_packet, const uint8_t *data, uint16_t data_len);

void esp_packet_set_icv(esp_packet_t *esp_packet, const uint8_t *icv, uint8_t icv_len);

uint32_t esp_packet_get_spi(esp_packet_t *esp_packet);

uint32_t esp_packet_get_seq_no(esp_packet_t *esp_packet);

uint16_t esp_packet_get_data_len(esp_packet_t *esp_packet);

void esp_packet_get_data(esp_packet_t *esp_packet, uint8_t *data);

uint8_t esp_packet_get_icv_len(esp_packet_t *esp_packet);

void esp_packet_get_icv(esp_packet_t *esp_packet, uint8_t *icv);

uint16_t esp_packet_get_packet_len(esp_packet_t *esp_packet);

void esp_packet_get_packet_bytes(esp_packet_t *esp_packet, uint8_t *packet_bytes);

//获取包含在完整性校验部分的数据长度
uint16_t esp_packet_get_auth_data_len(esp_packet_t *esp_packet);
//获取包含在完整性校验部分的数据
void esp_packet_get_auth_data(esp_packet_t *esp_packet, uint8_t *auth_data);

esp_packet_t *esp_packet_clone(esp_packet_t *esp_packet);

void esp_packet_destroy(esp_packet_t *esp_packet);

#endif /* _ESP_PACKET_H */