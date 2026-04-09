#include "ktime_slice.h"

static KTaskQueueHandle tq = {0, NULL, NULL};

void ktask_inctick(void)           //更新各任务的阻塞时间，这个函数放在1ms中断中
{
	uint8_t i;
	KTaskNode *temp = tq.head;
	if ((tq.head == NULL) || (tq.task_num == 0))
	{
		return;
	}
	
	for(i = 0; i < tq.task_num; i++)
	{
		if(temp->task_state == WAIT)
		{
			temp->time++;             //计时
			temp->duration++;
			if(temp->time >= temp->period_ms)
			{
				temp->task_state = READY;       //阻塞态转为就绪态
			}
		}
	
		if(temp->next != NULL)
		{
			temp = temp->next;  
		}		          
	}
}
/// @brief 
/// @param task 
/// @param wait_time 挂起时间
/// @param repeat_count 1: 单次执行， 0：重复执行
/// @return 
KTaskNode *ktask_create(void (*task)(void *parm), void *parm, uint32_t wait_time, uint8_t repeat_count)
{
	if (task == NULL)
	{
		return NULL;
	}
	if (tq.task_num >= KTASK_NUM_MAX)
	{
		return NULL;
	}
	
	KTaskNode *new_ktask  = (KTaskNode *)malloc(sizeof(KTaskNode));
	if (new_ktask == NULL)
	{
		return NULL;
	}
	
	new_ktask->task = task;
	new_ktask->time = 0;
	new_ktask->period_ms = wait_time;
	new_ktask->task_state = WAIT;
	new_ktask->repeat_count = repeat_count;
	new_ktask->next = NULL;
	new_ktask->param = parm;
	new_ktask->duration = 0;
	tq.task_num++;               //任务数+1
	if(tq.head == NULL)
	{
		tq.head = new_ktask;         //绑定第一个任务结点
		tq.rear = new_ktask;
	}
	else
	{
		tq.rear->next = new_ktask;		//将任务节点插入队列		        
	}
	tq.rear = new_ktask;           //尾结点
	return new_ktask;
}

void ktask_run(void)
{
	KTaskNode *temp_task_node = tq.head;
	KTaskNode *pre = tq.head;
	KTaskNode *temp = NULL;
	while(temp_task_node != NULL)
	{
		if(temp_task_node->task_state == READY)   //这个线程延迟结束，开始调度
		{
			temp_task_node->task_state = RUN;
			temp_task_node->task(temp_task_node->param);
			if(temp_task_node->repeat_count == 1)  //单次执行
			{
				if(temp_task_node == tq.head)
				{
					tq.head = temp_task_node->next;         //指向下一个结点
				}
				else if(temp_task_node == tq.rear)
				{
					tq.rear = pre;
				}
				else{
					pre->next = temp_task_node->next;
				}
				temp = temp_task_node;
				temp_task_node = temp_task_node->next;
				free(temp);             //删除结点

				tq.task_num--;
				continue;
			}
			else{                       //重复执行
				temp_task_node->task_state = WAIT;		
				temp_task_node->time = 0;            //重新计时							
			}			
		}	
		pre = temp_task_node;
	  	temp_task_node = temp_task_node->next;      
	}
}

int ktask_del(KTaskNode *kn)
{
	KTaskNode *temp_task_node = tq.head;
	KTaskNode *prev_task_node = NULL;

	if (tq.head == NULL || tq.task_num == 0)
	{
		return -1;
	}
	
	while(temp_task_node != NULL)
	{
		if(temp_task_node == kn)   
		{
			if ((prev_task_node == NULL) && (kn == tq.head))
			{
				tq.head = temp_task_node->next;
			}
			else
			{
				prev_task_node->next = temp_task_node->next;
			}
			if (temp_task_node == tq.rear)
			{
				tq.rear = prev_task_node;
			}
			
			free(temp_task_node);             //删除结点
			tq.task_num--;

			if (tq.task_num == 0)
			{
				tq.head = NULL;
                tq.rear = NULL;
			}
			return 0;
		}	
		prev_task_node = temp_task_node;
	  	temp_task_node = temp_task_node->next;      
	}
	return -1;
}

uint32_t ktask_get_duration(KTaskNode *ktn)
{
	return ktn->duration;
}

void ktask_clear_duration(KTaskNode *ktn)
{
	ktn->duration = 0;
}

