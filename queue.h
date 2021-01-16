#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stddef.h>

struct queue_s {
	struct queue_s *prev;
	struct queue_s *next;
};

typedef struct queue_s queue_t;

#define queue_init(q) \
	(q)->prev = (q);	\
	(q)->next = (q)

#define queue_empty(h) \
	(h)->prev == (h)

#define queue_insert_head(h, e) \
	(e)->prev = (h);		\
	(e)->next = (h)->next;	\
	(h)->next->prev = (e);	\
	(h)->next = (e)

#define queue_insert_after queue_insert_head

#define queue_insert_tail(h, e) \
	(e)->next = (h);			\
	(e)->prev = (h)->prev;	\
	(h)->prev->next = (e);	\
	(h)->prev = (e)

#define queue_head(h) \
	(h)->next

#define queue_last(h) \
	(h)->prev

#define queue_next(q) \
	(q)->next

#define queue_prev(q) \
	(q)->prev

#define queue_sentinel(h) \
    (h)

#define queue_remove(x) \
	(x)->prev->next = (x)->next; \
	(x)->next->prev = (x)->prev

#define queue_data(q, type, link) \
	(type *)((unsigned char *)q - offsetof(type, link))

queue_t *queue_middle(queue_t *queue);
void queue_sort(queue_t *queue, int (*cmp)(const queue_t *, const queue_t *));

#endif // _QUEUE_H_