#include "list.h"

void list_append(ListNode *l, ListNode *n)
{
    ListNode *node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}
unsigned int list_get_len(const ListNode *l)
{
    unsigned int len = 0;
    const ListNode *list = l->next;
    while (list != NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

ListNode *list_remove(ListNode *l, ListNode *n)
{
    /* remove slist head */
    ListNode *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (ListNode *)0) node->next = node->next->next;

    return l;
}