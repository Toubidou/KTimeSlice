#ifndef		TIME_SLICE_H
#define		TIME_SLICE_H

#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include "async_event.h"

#define		USE_ENCODER			1
#define		USE_BUTTON			0

#define		KTASK_NUM_MAX		128

/* 任务内存分配策略
 * 0: 动态分配 (malloc/free)，灵活但可能产生堆碎片
 * 1: 静态内存池，无碎片，内存占用固定 (sizeof(KTaskNode) * KTASK_NUM_MAX)
 */
#define		KTASK_USE_STATIC_ALLOC	1

typedef	struct _KTaskNode
{
	uint32_t time;
	uint32_t period_ms;
	uint8_t task_state;         //run: 0; ready: 1; wait: 2
	void (*task)(void *parm);
	void *param;
	uint8_t repeat_count;      //����ִ�л����ظ�ִ�У� 1�����Σ�  0��ѭ��
	struct _KTaskNode *next;
	uint32_t duration;
}KTaskNode;

enum
{
	RUN, READY, WAIT
};

enum
{
	INFINITE, SINGLE
};

typedef struct
{
	uint8_t task_num;
	KTaskNode *head;
	KTaskNode *rear;
}KTaskQueueHandle;

void ktask_inctick(void);
KTaskNode *ktask_create(void (*task)(void *parm), void *parm, uint32_t time, uint8_t repeat_count);
void ktask_run(void);
uint32_t ktask_get_duration(KTaskNode *ktn);
int ktask_del(KTaskNode *kn);
void ktask_clear_duration(KTaskNode *ktn);
void ktask_init(void);
#endif 
