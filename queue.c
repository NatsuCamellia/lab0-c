#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Merge two queues */
static void q_merge2(struct list_head *a, struct list_head *b, bool descend)
{
    struct list_head head, *node;
    INIT_LIST_HEAD(&head);
    int flip = descend ? -1 : 1;
    while (!list_empty(a) && !list_empty(b)) {
        if (flip * strcmp(list_first_entry(a, element_t, list)->value,
                          list_first_entry(b, element_t, list)->value) <=
            0)
            node = &q_remove_head(a, NULL, 0)->list;
        else
            node = &q_remove_head(b, NULL, 0)->list;
        list_add_tail(node, &head);
    }
    if (!list_empty(a))
        list_splice_tail_init(a, &head);
    else if (!list_empty(b))
        list_splice_tail_init(b, &head);

    list_splice(&head, a);
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
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    list_del_init(node);

    element_t *e = list_entry(node, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_head(head->prev->prev, sp, bufsize);
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
    if (!head || list_empty(head))
        return false;

    struct list_head *pending;
    element_t *node, *cut;
    list_for_each_entry(node, head, list) {
        cut = list_entry(node->list.next, element_t, list);
        while (&cut->list != head && !strcmp(node->value, cut->value))
            cut = list_entry(cut->list.next, element_t, list);
        cut = list_entry(cut->list.prev, element_t, list);

        if (cut != node) {
            /* Duplicated */
            pending = q_new();
            node = list_entry(node->list.prev, element_t, list);
            list_cut_position(pending, &node->list, &cut->list);
            q_free(pending);
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    return q_reverseK(head, 2);
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
    if (!head || list_empty(head))
        return;

    struct list_head pending;
    INIT_LIST_HEAD(&pending);
    struct list_head *node = head->next, *tail;
    for (int len = q_size(head); len >= k; len -= k) {
        tail = node->prev;
        for (int i = 0; i < k; i++) {
            node = node->next;
        }
        list_cut_position(&pending, tail, node->prev);
        q_reverse(&pending);
        list_splice_init(&pending, tail);
    }
}

/* Sort a queue with length k */
static void q_sortK(struct list_head *head, bool descend, int k)
{
    if (!head || k <= 1)
        return;

    struct list_head left;
    INIT_LIST_HEAD(&left);

    int half = k / 2;
    struct list_head *cut = head;
    for (int i = 0; i < half; i++)
        cut = cut->next;
    list_cut_position(&left, head, cut);

    q_sortK(&left, descend, half);
    q_sortK(head, descend, k - half);
    q_merge2(&left, head, descend); /* For stable sort */
    list_splice_init(&left, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    q_sortK(head, descend, q_size(head));
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return q_size(head);

    /* Iterate all nodes backwards. Let prev and next adjacent nodes.
     * If prev < next then remove prev */
    struct list_head *curr = head->prev->prev, *safe;
    while (curr != head) {
        /* safe <- curr <- next */
        safe = curr->prev;
        element_t *curr_e = list_entry(curr, element_t, list);
        const element_t *next_e = list_entry(curr->next, element_t, list);
        if (strcmp(curr_e->value, next_e->value) > 0) {
            list_del_init(curr);
            /* Delete is needed to pass the test */
            free(curr_e->value);
            free(curr_e);
        }
        curr = safe;
    }

    return q_size(head);

    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return q_size(head);

    /* Iterate all nodes backwards. Let prev and next adjacent nodes.
     * If prev < next then remove prev */
    struct list_head *curr = head->prev->prev, *safe;
    while (curr != head) {
        /* safe <- curr <- next */
        safe = curr->prev;
        element_t *curr_e = list_entry(curr, element_t, list);
        const element_t *next_e = list_entry(curr->next, element_t, list);
        if (strcmp(curr_e->value, next_e->value) < 0) {
            list_del_init(curr);
            /* Delete is needed to pass the test */
            free(curr_e->value);
            free(curr_e);
        }
        curr = safe;
    }

    return q_size(head);
}

/* Merge K queues given their queue contexts */
static void q_mergeK(struct list_head *head, bool descend, int k)
{
    if (k <= 1)
        return;

    int half = k / 2;
    struct list_head *left = head, *right = head;
    for (int i = 0; i < half; i++)
        right = right->next;

    q_mergeK(left, descend, half);
    q_mergeK(right, descend, k - half);
    q_merge2(list_entry(left, queue_contex_t, chain)->q,
             list_entry(right, queue_contex_t, chain)->q, descend);
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
