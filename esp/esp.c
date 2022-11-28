#include "esp.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

struct esp_t
{
    uint32_t spi;
    uint32_t seq_no; /* Sequence number */
    uint8_t *payload;
    uint8_t *padding;
    uint8_t pad_len;
    uint8_t nexthdr;
    uint8_t *icv;

    uint16_t payload_len; /* not included in packet */
    uint16_t icv_len;     /* not included in packet */
};

esp_t *esp_create()
{
    esp_t *esp = (esp_t *)malloc(sizeof(esp_t));
    esp->spi = 0;
    esp->seq_no = 0;
    esp->payload = NULL;
    esp->padding = NULL;
    esp->pad_len = 0;
    esp->nexthdr = 0;
    esp->icv = NULL;
    esp->payload_len = 0;
    esp->icv_len = 0;

    return esp;
}

esp_t *esp_create_from_bytes(const uint8_t *bytes, uint16_t total_len, uint16_t icv_len)
{
    esp_t *esp = (esp_t *)malloc(sizeof(esp_t));
    uint32_t *spi = (uint32_t *)bytes;
    esp->spi = ntohl(*spi);
    uint32_t *seq_no = (uint32_t *)(bytes + 4);
    esp->seq_no = ntohl(*seq_no);

    if (icv_len == 0)
    {
        esp->icv = NULL;
    }
    else
    {
        esp->icv = (uint8_t *)malloc(icv_len);
        memcpy(esp->icv, bytes + total_len - icv_len, icv_len);
    }
    esp->icv_len = icv_len;

    esp->nexthdr = *(bytes + total_len - icv_len - 1);

    esp->pad_len = *(bytes + total_len - icv_len - 2);
    if (esp->pad_len == 0)
    {
        esp->padding = NULL;
    }
    else
    {
        esp->padding = (uint8_t *)malloc(esp->pad_len);
        memcpy(esp->padding, bytes + total_len - icv_len - 2 - esp->pad_len, esp->pad_len);
    }

    esp->payload_len = total_len - 4 - 4 - esp->pad_len - 2 - icv_len;
    esp->payload = (uint8_t *)malloc(esp->payload_len);
    memcpy(esp->payload, bytes + 4 + 4, esp->payload_len);

    return esp;
}

void esp_set_spi(esp_t *esp, uint32_t spi)
{
    esp->spi = spi;
}

void esp_set_seq_no(esp_t *esp, uint32_t seq_no)
{
    esp->seq_no = seq_no;
}

void esp_set_payload(esp_t *esp, const uint8_t *payload, uint16_t payload_len)
{
    if (esp->payload != NULL)
    {
        free(esp->payload);
        esp->payload = NULL;
    }
    esp->payload = (uint8_t *)malloc(payload_len);
    memcpy(esp->payload, payload, payload_len);

    esp->payload_len = payload_len;
}

void esp_set_padding(esp_t *esp, const uint8_t *padding, uint8_t pad_len)
{
    if (esp->padding != NULL)
    {
        free(esp->padding);
        esp->padding = NULL;
    }
    esp->padding = (uint8_t *)malloc(pad_len);
    memcpy(esp->padding, padding, pad_len);

    esp->pad_len = pad_len;
}

void esp_set_nexthdr(esp_t *esp, uint8_t nexthdr)
{
    esp->nexthdr = nexthdr;
}

void esp_set_icv(esp_t *esp, const uint8_t *icv, uint16_t icv_len)
{
    if (esp->icv != NULL)
    {
        free(esp->icv);
        esp->icv = NULL;
    }
    esp->icv = (uint8_t *)malloc(icv_len);
    memcpy(esp->icv, icv, icv_len);

    esp->icv_len = icv_len;
}

uint32_t esp_get_spi(esp_t *esp)
{
    return esp->spi;
}

uint32_t esp_get_seq_no(esp_t *esp)
{
    return esp->seq_no;
}

uint16_t esp_get_payload_len(esp_t *esp)
{
    return esp->payload_len;
}

void esp_get_payload(esp_t *esp, uint8_t *payload)
{
    memcpy(payload, esp->payload, esp->payload_len);
}

uint8_t esp_get_pad_len(esp_t *esp)
{
    return esp->pad_len;
}

void esp_get_padding(esp_t *esp, uint8_t *padding)
{
    if (esp->padding != NULL)
    {
        memcpy(padding, esp->padding, esp->pad_len);
    }
}

uint8_t esp_get_nexthdr(esp_t *esp)
{
    return esp->nexthdr;
}

uint16_t esp_get_icv_len(esp_t *esp)
{
    return esp->icv_len;
}

void esp_get_icv(esp_t *esp, uint8_t *icv)
{
    if (esp->icv != NULL)
    {
        memcpy(icv, esp->icv, esp->icv_len);
    }
}

uint16_t esp_get_packet_len(esp_t *esp)
{
    return 4 + 4 + esp->payload_len + esp->pad_len + 2 + esp->icv_len;
}

void esp_get_packet_bytes(esp_t *esp, uint8_t *packet_bytes)
{
    *((uint32_t *)packet_bytes) = htonl(esp->spi);
    *((uint32_t *)(packet_bytes + 4)) = htonl(esp->seq_no);
    memcpy(packet_bytes + 8, esp->payload, esp->payload_len);
    if (esp->padding != NULL)
    {
        memcpy(packet_bytes + 8 + esp->payload_len, esp->padding, esp->pad_len);
    }
    *(packet_bytes + 8 + esp->payload_len + esp->pad_len) = esp->pad_len;
    *(packet_bytes + 8 + esp->payload_len + esp->pad_len + 1) = esp->nexthdr;
    if (esp->icv != NULL)
    {
        memcpy(packet_bytes + 8 + esp->payload_len + esp->pad_len + 2, esp->icv, esp->icv_len);
    }
}
/**
 * @brief 获取需加密部分的数据长度
 *
 * @param esp
 * @return uint16_t
 */
uint16_t esp_get_enc_data_len(esp_t *esp)
{
    return esp->payload_len + esp->pad_len + 2;
}
/**
 * @brief 获取需加密部分的数据
 *
 * @param esp
 * @return uint16_t
 */
void esp_get_enc_data(esp_t *esp, uint8_t *enc_data)
{
    memcpy(enc_data, esp->payload, esp->payload_len);
    if (esp->padding != NULL)
    {
        memcpy(enc_data + esp->payload_len, esp->padding, esp->pad_len);
    }
    *(enc_data + esp->payload_len + esp->pad_len) = esp->pad_len;
    *(enc_data + esp->payload_len + esp->pad_len + 1) = esp->nexthdr;
}

esp_t *esp_clone(esp_t *esp)
{
    esp_t *clone = (esp_t *)malloc(sizeof(esp_t));
    clone->spi = esp->spi;
    clone->seq_no = esp->seq_no;
    memcpy(clone->payload, esp->payload, esp->payload_len);
    memcpy(clone->padding, esp->padding, esp->pad_len);
    clone->pad_len = esp->pad_len;
    clone->nexthdr = esp->nexthdr;
    memcpy(clone->icv, esp->icv, esp->icv_len);

    clone->icv_len = esp->icv_len;
    clone->payload_len = esp->payload_len;
    return clone;
}

void esp_destroy(esp_t *esp)
{
    if (esp->payload != NULL)
    {
        free(esp->payload);
        esp->payload = NULL;
    }
    if (esp->padding != NULL)
    {
        free(esp->padding);
        esp->padding = NULL;
    }
    if (esp->icv != NULL)
    {
        free(esp->icv);
        esp->icv = NULL;
    }
    free(esp);
    esp = NULL;
}