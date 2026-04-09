#ifndef _ASYNC_EVENT_H_
#define _ASYNC_EVENT_H_

#include "main.h"
#include <stdlib.h>
#include "list.h"

#define     SIG_NUM_MAX     256

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)
#define list_entry(node, type, member) \
    ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

typedef struct _asyncevent_attr
{
    void (*evt_cb)(void);
    uint32_t sig;
}ASY_EVT_ATTR;

typedef struct _ASY_EVT_HANDLE
{   
    ListNode list_head;
    ASY_EVT_ATTR map[SIG_NUM_MAX];
    uint32_t no_evt_time;
}ASY_EVT_HANDLE;

typedef struct _ASY_EVT
{
    uint32_t sig;
    ListNode next;
}ASY_EVT;

uint8_t asy_evt_register(uint32_t sig, void (*evt_cb)(void));
uint8_t asy_evt_emit(uint32_t sig);
void task_asy_evt_process(void *p);
unsigned int asy_get_no_evt_time(void);

#endif
