#include "async_event.h"

static ASY_EVT_HANDLE AsyEvtHandle = {.no_evt_time = 0};

uint8_t asy_evt_register(uint32_t sig, void (*evt_cb)(void))
{
    if((sig < 0) || (sig > SIG_NUM_MAX))
        return 1;
    if(AsyEvtHandle.map[sig].evt_cb != NULL)
        return 2;
    AsyEvtHandle.map[sig].evt_cb = evt_cb;
    AsyEvtHandle.map[sig].sig = sig;
    return 0;
}
void asy_evt_unregister(uint32_t sig)
{
    AsyEvtHandle.map[sig].evt_cb = NULL;
    AsyEvtHandle.map[sig].sig = NULL;
}

uint8_t asy_evt_emit(uint32_t sig)
{
    if((sig < 0) || (sig > SIG_NUM_MAX))
        return 1;
    if(AsyEvtHandle.map[sig].evt_cb == NULL)
        return 2;
    ASY_EVT *node = (ASY_EVT *)malloc(sizeof(ASY_EVT));
    if(node == NULL)
    {
        return 3;
    }
    
    node->sig = sig;
    list_append(&(AsyEvtHandle.list_head), &(node->next));
    return 0;
}

uint8_t asy_evt_process(void)
{
    if(list_get_len(&(AsyEvtHandle.list_head)) == 0)
    {
        AsyEvtHandle.no_evt_time += 5;
        return 1;
    }
    // rt_kprintf("evt num %d", rt_slist_len(&(AsyEvtHandle.list_head)));
    ListNode *node = NULL;
    ASY_EVT *temp_evt_p;
    list_for_each(node, &(AsyEvtHandle.list_head))
    {
        temp_evt_p = list_entry(node, ASY_EVT, next);
        AsyEvtHandle.map[temp_evt_p->sig].evt_cb();
        list_remove(&(AsyEvtHandle.list_head), &(temp_evt_p->next));
        free(temp_evt_p);
    }

    AsyEvtHandle.no_evt_time = 0;
    return 0;
}

//5ms调度
void task_asy_evt_process(void *p)
{
    asy_evt_process();
}

unsigned int asy_get_no_evt_time(void)
{
    return AsyEvtHandle.no_evt_time;
}

void asy_clear_evt_time(void)
{
    AsyEvtHandle.no_evt_time = 0;
}