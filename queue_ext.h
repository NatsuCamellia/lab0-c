#ifndef QUEUE_EXT_H
#define QUEUE_EXT_H

/**
 * q_shuffle - Shuffle all nodes of queue.
 * @head: header of queue
 *
 * No effect if queue is NULL or empty. If there is only one element, do
 * nothing.
 */
void q_shuffle(struct list_head *head);

#endif /* QUEUE_EXT_H */