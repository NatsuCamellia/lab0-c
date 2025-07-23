#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Merge two headless singly linked lists */
static struct list_head *list_merge(struct list_head *a,
                                    struct list_head *b,
                                    bool descend)
{
    struct list_head *head = NULL, **tail = &head;
    int toggle = descend ? -1 : 1;
    while (a && b) {
        const char *str_a = list_entry(a, element_t, list)->value;
        const char *str_b = list_entry(b, element_t, list)->value;
        if (toggle * strcmp(str_a, str_b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
        }
    }
    *tail = a ? a : b;
    return head;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list) {
        free(entry->value);
        free(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        e = NULL;
        return false;
    }

    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        e = NULL;
        return false;
    }

    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    list_del_init(node);

    element_t *e = list_entry(node, element_t, list);
    if (sp)
        strncpy(sp, e->value, bufsize - 1);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    list_del_init(node);

    list_del_init(node);
    element_t *e = list_entry(node, element_t, list);
    if (sp)
        strncpy(sp, e->value, bufsize - 1);
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *node;
    list_for_each(node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    /* slow is the middle node */
    list_del(slow);

    element_t *e = list_entry(slow, element_t, list);
    free(e->value);
    free(e);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *left, *right;
    left = head->next;
    right = left->next;
    while (left != head && right != head) {
        right->prev = left->prev;
        left->prev = right;
        left->next = right->next;
        right->next = left;

        left->next->prev = left;
        right->prev->next = right;

        left = left->next;
        right = left->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node, *next;
    list_for_each_safe(node, next, head) {
        /* Swap next and prev pointer */
        node->next = node->prev;
        node->prev = next;
    }
    /* node == head, next == head.next */
    head->next = head->prev;
    head->prev = next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort a headless singly linked list with length k */
static struct list_head *list_sortK(struct list_head *head, bool descend, int k)
{
    if (k == 1)
        return head;

    int half = k / 2;
    struct list_head *tail = head, *left = head, *right;
    for (int i = 0; i < half - 1; i++)
        tail = tail->next;
    right = tail->next;
    tail->next = NULL;

    left = list_sortK(left, descend, half);
    right = list_sortK(right, descend, k - half);
    return list_merge(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    int len = 0;
    struct list_head *node;
    list_for_each(node, head)
        len++;
    if (len <= 1)
        return;

    head->prev->next = NULL;
    node = list_sortK(head->next, descend, len);
    head->next = node;
    node->prev = head;
    while (node->next) {
        node->next->prev = node;
        node = node->next;
    }
    node->next = head;
    head->prev = node;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge two queues given their queue contexts */
static void q_merge2(struct list_head *a, struct list_head *b, bool descend)
{
    queue_contex_t *context_a = list_entry(a, queue_contex_t, chain);
    queue_contex_t *context_b = list_entry(b, queue_contex_t, chain);
    struct list_head *qa = context_a->q;
    struct list_head *qb = context_b->q;

    qa->prev->next = NULL;
    qb->prev->next = NULL;
    struct list_head *list = list_merge(qa->next, qb->next, descend);
    qa->next = list;
    list->prev = qa;
    while (list->next) {
        list->next->prev = list;
        list = list->next;
    }
    list->next = qa;
    qa->prev = list;
    INIT_LIST_HEAD(qb);
}

/* Merge K queues given their queue contexts */
static void q_mergeK(struct list_head *head, bool descend, int k)
{
    if (k == 1)
        return;
    if (k == 2) {
        q_merge2(head, head->next, descend);
        return;
    }

    int half = k / 2;
    struct list_head *left = head, *right = head;
    for (int i = 0; i < half; i++, right = right->next)
        ;

    q_mergeK(left, descend, half);
    q_mergeK(right, descend, k - half);
    q_merge2(left, right, descend);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (list_empty(head))
        return 0;

    struct list_head *node;
    int len = 0;
    list_for_each(node, head)
        len++;

    q_mergeK(head->next, descend, len);
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}
