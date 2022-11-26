#include "esp_packet.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

struct esp_packet_t
{
    uint32_t spi;
    uint32_t seq_no;
    uint16_t data_len;
    uint8_t *data;
    uint16_t icv_len;
    uint8_t *icv;
};

esp_packet_t *esp_packet_create()
{
    esp_packet_t *esp_packet = (esp_packet_t *)malloc(sizeof(esp_packet_t));
    esp_packet->spi = 0;
    esp_packet->seq_no = 0;
    esp_packet->data_len = 0;
    esp_packet->data = NULL;
    esp_packet->icv_len = 0;
    esp_packet->icv = NULL;
    return esp_packet;
}

esp_packet_t *esp_packet_create_from_bytes(const uint8_t *bytes, uint16_t total_len, uint16_t icv_len)
{
    esp_packet_t *esp_packet = (esp_packet_t *)malloc(sizeof(esp_packet_t));
    esp_packet->spi = ntohl(*((uint32_t *)bytes));
    esp_packet->seq_no = ntohl(*((uint32_t *)(bytes + 4)));

    esp_packet->data_len = total_len - 8 - icv_len;
    esp_packet->data = (uint8_t *)malloc(esp_packet->data_len);
    memcpy(esp_packet->data, bytes + 8, esp_packet->data_len);

    if (icv_len > 0)
    {
        esp_packet->icv_len = icv_len;
        esp_packet->icv = (uint8_t *)malloc(esp_packet->icv_len);
        memcpy(esp_packet->icv, bytes + 8 + esp_packet->data_len, esp_packet->icv_len);
    }
    return esp_packet;
}

void esp_packet_set_spi(esp_packet_t *esp_packet, uint32_t spi)
{
    esp_packet->spi = spi;
}

void esp_packet_set_seq_no(esp_packet_t *esp_packet, uint32_t seq_no)
{
    esp_packet->seq_no = seq_no;
}

void esp_packet_set_data(esp_packet_t *esp_packet, const uint8_t *data, uint16_t data_len)
{
    if (esp_packet->data != NULL)
    {
        free(esp_packet->data);
        esp_packet->data = NULL;
    }
    esp_packet->data = (uint8_t *)malloc(data_len);
    memcpy(esp_packet->data, data, data_len);
    esp_packet->data_len = data_len;
}

void esp_packet_set_icv(esp_packet_t *esp_packet, const uint8_t *icv, uint8_t icv_len)
{
    if (esp_packet->icv != NULL)
    {
        free(esp_packet->icv);

        esp_packet->icv = NULL;
    }
    esp_packet->icv = (uint8_t *)malloc(icv_len);
    memcpy(esp_packet->icv, icv, icv_len);
    esp_packet->icv_len = icv_len;
}

uint32_t esp_packet_get_spi(esp_packet_t *esp_packet)
{
    return esp_packet->spi;
}

uint32_t esp_packet_get_seq_no(esp_packet_t *esp_packet)
{
    return esp_packet->seq_no;
}

uint16_t esp_packet_get_data_len(esp_packet_t *esp_packet)
{
    return esp_packet->data_len;
}

void esp_packet_get_data(esp_packet_t *esp_packet, uint8_t *data)
{
    memcpy(data, esp_packet->data, esp_packet->data_len);
}

uint8_t esp_packet_get_icv_len(esp_packet_t *esp_packet)
{
    return esp_packet->icv_len;
}

void esp_packet_get_icv(esp_packet_t *esp_packet, uint8_t *icv)
{
    memcpy(icv, esp_packet->icv, esp_packet->icv_len);
}

uint16_t esp_packet_get_packet_len(esp_packet_t *esp_packet)
{
    return 8 + esp_packet->data_len + esp_packet->icv_len;
}

void esp_packet_get_packet_bytes(esp_packet_t *esp_packet, uint8_t *packet_bytes)
{
    *((uint32_t *)packet_bytes) = htonl(esp_packet->spi);
    *((uint32_t *)(packet_bytes + 4)) = htonl(esp_packet->seq_no);
    memcpy(packet_bytes + 8, esp_packet->data, esp_packet->data_len);
    if (esp_packet->icv != NULL)
    {
        memcpy(packet_bytes + 8 + esp_packet->data_len, esp_packet->icv, esp_packet->icv_len);
    }
}

esp_packet_t *esp_packet_clone(esp_packet_t *esp_packet)
{
    esp_packet_t *clone = (esp_packet_t *)malloc(sizeof(esp_packet_t));
    clone->spi = esp_packet->spi;
    clone->seq_no = esp_packet->seq_no;

    clone->data_len = esp_packet->data_len;
    clone->data = (uint8_t *)malloc(clone->data_len);
    memcpy(clone->data, esp_packet->data, clone->data_len);
    if (esp_packet->icv != NULL)
    {
        clone->icv_len = esp_packet->icv_len;
        clone->icv = (uint8_t *)malloc(clone->icv_len);
        memcpy(clone->icv, esp_packet->icv, clone->icv_len);
    }
    return clone;
}

void esp_packet_destroy(esp_packet_t *esp_packet)
{
    if (esp_packet->data != NULL)
    {
        free(esp_packet->data);
        esp_packet->data = NULL;
    }
    if (esp_packet->icv != NULL)
    {
        free(esp_packet->icv);
        esp_packet->icv = NULL;
    }
    free(esp_packet);
    esp_packet = NULL;
}
