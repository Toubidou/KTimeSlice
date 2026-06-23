#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include "async_event.h"

#define ENCODER_SIG_NUM_CW      0
#define ENCODER_SIG_NUM_CCW     1

void encoder_create(uint16_t (*get_counter)(void), void (*cw_cb)(void), void (*ccw_cb)(void));

#endif /* __ENCODER_H__ */