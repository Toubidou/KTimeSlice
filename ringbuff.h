#ifndef __RINGBUFF_H__
#define __RINGBUFF_H__

#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct _RingBuff
{
    uint32_t head;
    uint32_t tail;
    uint8_t *buffer;
    uint32_t length;
}RingBuff;

RingBuff *ringbuffer_create(uint32_t length);
uint32_t ringbuffer_get_stored_length(RingBuff *rb);
int ringbuffer_put(RingBuff *rb, uint8_t *src_data, uint32_t num);
int ringbuffer_get(RingBuff *rb, uint8_t *dst_data, uint32_t num);
uint8_t ringbuffer_getchar(RingBuff *rb);

#endif /* __RINGBUFF_H__ */