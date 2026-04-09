#ifndef __LIST_H__
#define __LIST_H__

#include "main.h"

typedef struct _ListNode
{
    struct _ListNode *next;
}ListNode;

void list_append(ListNode *l, ListNode *n);
unsigned int list_get_len(const ListNode *l);
ListNode *list_remove(ListNode *l, ListNode *n);

#endif /* __LIST_H__ */