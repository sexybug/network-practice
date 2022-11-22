#ifndef _ESP_H
#define _ESP_H

#include <stdint.h>

typedef struct esp_t esp_t;

esp_t *esp_create();

esp_t *esp_create_from_bytes(const uint8_t *bytes, uint16_t total_len, uint16_t icv_len);

void esp_set_spi(esp_t *esp, uint32_t spi);

void esp_set_seq_no(esp_t *esp, uint32_t seq_no);
void esp_set_payload(esp_t *esp, const uint8_t *payload, uint16_t payload_len);

void esp_set_padding(esp_t *esp, const uint8_t *padding, uint8_t pad_len);

void esp_set_nexthdr(esp_t *esp, uint8_t nexthdr);

void esp_set_icv(esp_t *esp, const uint8_t *icv, uint16_t icv_len);

uint32_t esp_get_spi(esp_t *esp);

uint32_t esp_get_seq_no(esp_t *esp);

uint16_t esp_get_payload_len(esp_t *esp);

void esp_get_payload(esp_t *esp, uint8_t *payload);

uint8_t esp_get_pad_len(esp_t *esp);

void esp_get_padding(esp_t *esp, uint8_t *padding);

uint8_t esp_get_nexthdr(esp_t *esp);

uint16_t esp_get_icv_len(esp_t *esp);

void esp_get_icv(esp_t *esp, uint8_t *icv);

uint16_t esp_get_packet_len(esp_t *esp);

void esp_get_packet_bytes(esp_t *esp, uint8_t *packet_bytes);

esp_t *esp_clone(esp_t *esp);

void esp_destroy(esp_t *esp);

#endif /* _ESP_H */