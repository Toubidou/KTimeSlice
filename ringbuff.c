#include "ringbuff.h"

RingBuff *ringbuffer_create(uint32_t length)
{
    
    RingBuff *rb = (RingBuff *)malloc(sizeof(RingBuff));
    if (rb == NULL)
        return rb;
    rb->buffer = (uint8_t *)malloc(length);
    if (rb->buffer == NULL)
    {
        free(rb);
        rb = NULL;
        return rb;
    }
    rb->head = 0;
    rb->tail = 0;
    rb->length = length;
    return rb;
}
uint32_t ringbuffer_get_stored_length(RingBuff *rb)
{
    if (rb->tail >= rb->head)
    {
        return rb->tail - rb->head;
    }
    else
    {
        return rb->length - (rb->head - rb->tail);
    }
}

bool ringbuffer_is_empty(RingBuff *rb)
{
    if (rb == NULL)
    {
        return false;
    }
    return rb->head == rb->tail;
}
bool ringbuffer_is_full(RingBuff *rb)
{
    if (rb == NULL)
    {
        return false;
    }
    return ((rb->tail + 1) % rb->length) == rb->head;
}

int ringbuffer_put(RingBuff *rb, uint8_t *src_data, uint32_t num)
{
    if (ringbuffer_is_full(rb))
    {
        return -1;
    }
    if (num == 0)
    {
        return -2;
    }
    uint32_t space_size = rb->length - ringbuffer_get_stored_length(rb);
    uint8_t status = 0;
    if ((rb->length - ringbuffer_get_stored_length(rb) < num))
    {
        num = space_size;
        status = -3;
    }
    if (rb->length - rb->tail > num)
    {
        memcpy(rb->buffer + rb->tail, src_data, num);
        rb->tail += num;
        return status;
    }
    //到缓冲区末端不足存储全部数据，拆成两段
    memcpy(rb->buffer + rb->tail, src_data, rb->length - rb->tail);
    memcpy(rb->buffer, src_data + (rb->length - rb->tail), num - (rb->length - rb->tail));
    rb->tail = num - (rb->length - rb->tail);
    return status;
}
int ringbuffer_get(RingBuff *rb, uint8_t *dst_data, uint32_t num)
{
    if (ringbuffer_is_empty(rb))
    {
        return -1;
    }
    if (num == 0)
    {
        return -2;
    }

    uint32_t stored_length = ringbuffer_get_stored_length(rb);
    /* less data */
    if (stored_length < num)
        num = stored_length;
    if (rb->length - rb->head > num)
    {
        memcpy(dst_data, rb->buffer + rb->head, num);
        rb->head += num;
        return 0;
    }
    //拆成两段
    memcpy(dst_data, rb->buffer + rb->head, rb->length - rb->head);
    memcpy(dst_data + (rb->length - rb->head), rb->buffer, num - (rb->length - rb->head));
    rb->head = num - (rb->length - rb->head);
    return 0;
}
uint8_t ringbuffer_getchar(RingBuff *rb)
{
    uint8_t c = NULL;
    if (rb == NULL)
    {
        return c;
    }
    if (ringbuffer_is_empty(rb))
    {
        return c;
    }
    
    c = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % rb->length;
    return c;
}