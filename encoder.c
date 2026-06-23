#include "encoder.h"
#include "ktime_slice.h"

uint16_t (*get_counter)(void) = NULL;
static uint16_t prev_cnt = 0;

void encoder_get_dir(void *parm)
{
    uint16_t cur_cnt;
    int32_t diff;

    cur_cnt = get_counter();
    diff = (int32_t)(cur_cnt - prev_cnt);

    // 处理16位计数器回绕 (0 ↔ 65535)
    if (diff > 32767)
    {
        diff -= 65536;       // 向下计数回绕: 0→65535 实际是往下走
    } 
    else if (diff < -32768)
    {
        diff += 65536;       // 向上计数回绕: 65535→0 实际是往上走
    }

    if (diff > 0) 
    {
        asy_evt_emit(ENCODER_SIG_NUM_CW);
    }
    else if (diff < 0) 
    {
        asy_evt_emit(ENCODER_SIG_NUM_CCW);
    }
    prev_cnt = cur_cnt;
}

void encoder_create(uint16_t (*enc_get_counter)(void), void (*cw_cb)(void), void (*ccw_cb)(void))
{
    get_counter = enc_get_counter;
    prev_cnt = get_counter();
    asy_evt_register(ENCODER_SIG_NUM_CW, cw_cb);
    asy_evt_register(ENCODER_SIG_NUM_CCW, ccw_cb);
    ktask_create(encoder_get_dir, NULL, 100, 0);
}
