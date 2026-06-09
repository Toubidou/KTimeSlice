#include "ktime_slice.h"

static KTaskQueueHandle tq = {0, NULL, NULL};

static inline uint32_t critical_enter(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}
static inline void critical_exit(uint32_t primask)
{
    __set_PRIMASK(primask);
}

/* ====== 静态内存池 (KTASK_USE_STATIC_ALLOC == 1) ====== */
#if KTASK_USE_STATIC_ALLOC

static KTaskNode task_pool[KTASK_NUM_MAX];
static KTaskNode *task_free_list = NULL;
static bool pool_initialized = false;

static void task_pool_init(void)
{
    if (pool_initialized) return;
    uint32_t i;
    for (i = 0; i < KTASK_NUM_MAX - 1; i++)
    {
        task_pool[i].next = &task_pool[i + 1];
    }
    task_pool[KTASK_NUM_MAX - 1].next = NULL;
    task_free_list = &task_pool[0];
    pool_initialized = true;
}

static KTaskNode *task_pool_alloc(void)
{
    if (!pool_initialized)
    {
        task_pool_init();
    }
    if (task_free_list == NULL)
    {
        return NULL;   /* 池已耗尽 */
    }
    KTaskNode *node = task_free_list;
    task_free_list = task_free_list->next;
    return node;
}

static void task_pool_free(KTaskNode *node)
{
    if (node == NULL) return;
    node->next = task_free_list;   /* 复用 next 指针链入空闲链表 */
    task_free_list = node;
}

#endif /* KTASK_USE_STATIC_ALLOC */

void ktask_inctick(void)           //���¸����������ʱ�䣬�����������1ms�ж���
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
			temp->time++;             //��ʱ
			temp->duration++;
			if(temp->time >= temp->period_ms)
			{
				temp->task_state = READY;       //����̬תΪ����̬
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
/// @param wait_time ����ʱ��
/// @param repeat_count 1: ����ִ�У� 0���ظ�ִ��
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
	
#if KTASK_USE_STATIC_ALLOC
	KTaskNode *new_ktask = task_pool_alloc();
#else
	KTaskNode *new_ktask = (KTaskNode *)malloc(sizeof(KTaskNode));
#endif
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

	uint32_t primask = critical_enter();  // �� �޸Ĺ�������ǰ���ж�

	tq.task_num++;               //������+1
	if(tq.head == NULL)
	{
		tq.head = new_ktask;         //�󶨵�һ��������
		tq.rear = new_ktask;
	}
	else
	{
		tq.rear->next = new_ktask;		//������ڵ�������		        
	}
	tq.rear = new_ktask;           //β���

	critical_exit(primask);              // �� �ָ�

	return new_ktask;
}

void ktask_run(void)
{
	uint32_t primask = critical_enter();

	KTaskNode *temp_task_node = tq.head;
	KTaskNode *pre = tq.head;
	KTaskNode *temp = NULL;
	while(temp_task_node != NULL)
	{
		if(temp_task_node->task_state == READY)   //����߳��ӳٽ�������ʼ����
		{
			temp_task_node->task_state = RUN;

			critical_exit(primask);  // �� ִ���û��ص�ǰһ��Ҫ**���ж�**��
									//��������ϵͳ��ʵʱ�ԣ�systick�������жϵȣ����ᱻ������
			temp_task_node->task(temp_task_node->param);

			primask = critical_enter();  // �� �ص����غ����¹��ж�

			if(temp_task_node->repeat_count == 1)  //����ִ��
			{
				if(temp_task_node == tq.head)
				{
					tq.head = temp_task_node->next;         //ָ����һ�����
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
#if KTASK_USE_STATIC_ALLOC
				task_pool_free(temp);       /* 归还节点到静态池 */
#else
				free(temp);                 //删除节点
#endif

				tq.task_num--;
				continue;
			}
			else{                       //�ظ�ִ��
				temp_task_node->task_state = WAIT;		
				temp_task_node->time = 0;            //���¼�ʱ							
			}			
		}	
		pre = temp_task_node;
	  	temp_task_node = temp_task_node->next;      
	}

	critical_exit(primask);
}

int ktask_del(KTaskNode *kn)
{
	KTaskNode *temp_task_node = tq.head;
	KTaskNode *prev_task_node = NULL;

	if (tq.head == NULL || tq.task_num == 0)
	{
		return -1;
	}
	
	uint32_t primask = critical_enter();

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

#if KTASK_USE_STATIC_ALLOC
			task_pool_free(temp_task_node);     /* 归还节点到静态池 */
#else
			free(temp_task_node);               //删除节点
#endif
			tq.task_num--;

			if (tq.task_num == 0)
			{
				tq.head = NULL;
                tq.rear = NULL;
			}
			critical_exit(primask);
			return 0;
		}	
		prev_task_node = temp_task_node;
	  	temp_task_node = temp_task_node->next;      
	}
	critical_exit(primask);
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

