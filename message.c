#include "message.h"

#if 1
MessageHandle *message_create(const char *name,
							uint16_t msg_size,
							uint32_t msgs_num_max)
{
	MessageHandle *mq = (MessageHandle *)malloc(sizeof(MessageHandle));
	MessageNode *head;
	if (mq == NULL)
	{
		return mq;
	}

	mq->msg_pool = malloc((msg_size + sizeof(MessageNode)) * msgs_num_max);
	if (mq->msg_pool == NULL)
	{
		return NULL;
	}
	mq->head = NULL;
	mq->tail = NULL;
	mq->queue_free = NULL;
	mq->msg_size = msg_size;
	mq->msgs_num_max = msgs_num_max;
	mq->name = name;
	mq->entry = 0;
	/* initialize message empty list */
	for (uint32_t i = 0; i < mq->msgs_num_max; i++)
	{
		head = (MessageNode *)((uint8_t *)mq->msg_pool +
                                        i * (mq->msg_size + sizeof(MessageNode)));
        head->next = (MessageNode *)mq->queue_free;
        mq->queue_free = head;
	}
	
	return mq;
}

int message_send(MessageHandle *mq, const void *buffer, uint16_t size)
{
	if ((mq == NULL) || (buffer == NULL))
	{
		return -1;
	}
	if (size > mq->msg_size)
	{
		return -2;
	}
	//检测消息队列是否已满
	if(mq->queue_free == NULL)
	{
		return -3;
	}
	MessageNode *msg = (MessageNode *)mq->queue_free;
	mq->queue_free = msg->next;
	/* the msg is the new tailer of list, the next shall be NULL */
	msg->next = NULL;
	//消息块头部为指针，从数据部分开始复制
	memcpy(msg + 1, buffer, size);
	if (mq->tail != NULL)
	{
		((MessageNode *)mq->tail)->next = msg;
	}
	/* set new tail */
    mq->tail = msg;
	if (mq->head == NULL)
        mq->head = msg;
	if (mq->entry < MSG_NUM_MAX)
	{
		mq->entry++;
	}
	
	return 0;
}

int message_receive(MessageHandle *mq, const void *buffer, uint16_t size)
{
	if ((mq == NULL) || (buffer == NULL))
	{
		return -1;
	}
	if (size > mq->msg_size)
	{
		return -2;
	}
	//检测消息队列是否为空
	if(mq->entry == 0)
	{
		return -3;
	}
	MessageNode *msg = (MessageNode *)mq->head;
	mq->head = msg->next;
	if (mq->tail == msg)
        mq->tail = NULL;

	//消息块头部为指针，从数据部分开始复制
	memcpy(buffer, msg + 1, size);
	/* put message to free list */
    msg->next = (MessageNode *)mq->queue_free;
    mq->queue_free = msg;
	if (mq->entry > 0)
	{
		mq->entry--;
	}
	return 0;
}

#endif
