#include <linux/types.h>

struct ip_auth_hdr
{
    /* 下一个头 */
    __u8 nexthdr;

    /* 载荷长度 */
    __u8 hdrlen; /* This one is measured in 32 bit units! */

    /* 保留 */
    __be16 reserved;

    /* 安全参数索引 */
    __be32 spi;

    /* 序列号 */
    __be32 seq_no; /* Sequence number */

    /*鉴别数据（变长的） */
    /* modified by sexybug to be compatible with c99 */
    __u8 auth_data[]; /* Variable len but >=4. Mind the 64 bit alignment! */
};

/* 网卡接口默认MTU=1500 */
const int BUFFER_SIZE = 1500;
/* 无可选字段的IP头长度为20byte */
const int IP_HDR_LEN = 20;