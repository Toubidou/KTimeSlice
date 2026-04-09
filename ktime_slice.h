#ifndef		TIME_SLICE_H
#define		TIME_SLICE_H

#include "main.h"
#include <stdlib.h>
#include <stdbool.h>

#define		KTASK_NUM_MAX		128

typedef	struct _KTaskNode
{
	uint32_t time;
	uint32_t period_ms;
	uint8_t task_state;         //run: 0; ready: 1; wait: 2
	void (*task)(void *parm);
	void *param;
	uint8_t repeat_count;      //单次执行还是重复执行， 1：单次；  0：循环
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

#endif 
