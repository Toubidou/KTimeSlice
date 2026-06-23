#include "async_event.h"

static ASY_EVT_HANDLE AsyEvtHandle = {.no_evt_time = 0, .map = {0}};

/* ====== 静态事件节点池 ====== */
static ASY_EVT  event_pool[ASY_EVT_QUEUE_MAX];
static ListNode *event_free_head = NULL;   /* 指向空闲节点的 next 字段 */
static bool pool_initialized = false;

static void event_pool_init(void)
{
    if (pool_initialized) return;
    uint32_t i;
    for (i = 0; i < ASY_EVT_QUEUE_MAX - 1; i++)
    {
        event_pool[i].next.next = &event_pool[i + 1].next;
    }
    event_pool[ASY_EVT_QUEUE_MAX - 1].next.next = NULL;
    event_free_head = &event_pool[0].next;
    pool_initialized = true;
}

static ASY_EVT *event_pool_alloc(void)
{
    if (!pool_initialized) event_pool_init();
    if (event_free_head == NULL) return NULL;   /* 池已耗尽 */

    ASY_EVT *evt = list_entry(event_free_head, ASY_EVT, next);
    event_free_head = event_free_head->next;
    return evt;
}

static void event_pool_free(ASY_EVT *evt)
{
    if (evt == NULL) return;
    evt->next.next = event_free_head;
    event_free_head = &evt->next;
}

/// @brief 
/// @param sig  encoder sig: 0, 1;  button: 5~40 
/// @param evt_cb 
/// @return 
uint8_t asy_evt_register(uint32_t sig, void (*evt_cb)(void))
{
    if((sig > SIG_NUM_MAX))
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
    AsyEvtHandle.map[sig].sig = 0;
}

uint8_t asy_evt_emit(uint32_t sig)
{
    if((sig > SIG_NUM_MAX))
        return 1;
    if(AsyEvtHandle.map[sig].evt_cb == NULL)
        return 2;
    ASY_EVT *node = event_pool_alloc();
    if (node == NULL)
    {
        return 3;
    }

    node->sig = sig;
    list_append(&(AsyEvtHandle.list_head), &(node->next));
    return 0;
}

uint8_t asy_evt_process(void)
{
    if (AsyEvtHandle.list_head.next == NULL)
    {
        AsyEvtHandle.no_evt_time += 5;
        return 1;
    }

    /* 逐个从头部弹出，O(n)，比原 list_for_each + list_remove 的 O(n²) 高效 */
    ListNode *node = AsyEvtHandle.list_head.next;
    while (node != NULL)
    {
        ASY_EVT *evt = list_entry(node, ASY_EVT, next);
        ListNode *next_node = node->next;           /* 先保存后继，防止 use-after-free */

        /* 从链表头部弹出 (O(1)) */
        AsyEvtHandle.list_head.next = next_node;

        if (AsyEvtHandle.map[evt->sig].evt_cb != NULL)
        {
            AsyEvtHandle.map[evt->sig].evt_cb();    /* 执行回调 */
        }

        event_pool_free(evt);                        /* 归还到静态池 */
        node = next_node;
    }

    AsyEvtHandle.no_evt_time = 0;
    return 0;
}

//5ms调度
void asy_evt_task_process(void *p)
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