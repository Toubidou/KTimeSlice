#ifndef		MESSAGE_H
#define		MESSAGE_H

#include "main.h"
#include <stdlib.h>
#include <string.h>

#define		MSG_NUM_MAX		256

typedef struct _MessageNode
{
	struct _MessageNode *next;
}MessageNode;

typedef struct _MessageHandle
{
	const char *name;
	void *msg_pool;
	uint16_t msg_size;
	uint32_t msgs_num_max;
	void *head;
	void *tail;
	void *queue_free;
	uint32_t entry;
}MessageHandle;

MessageHandle *message_create(const char *name,
							uint16_t msg_size,
							uint32_t msgs_num_max);
int message_send(MessageHandle *mq, const void *buffer, uint16_t size);
int message_receive(MessageHandle *mq, const void *buffer, uint16_t size);

#endif

