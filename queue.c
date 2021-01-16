#include "queue.h"

// ��ö����е��в��ڵ�
queue_t *queue_middle(queue_t *queue) {
	// ��ָ��
	queue_t *slow = queue_head(queue);
	if (slow == queue_last(queue)) {
		return slow;
	}

	// ��ָ��
	queue_t *fast = queue_head(queue);

	for (;;) {
		// ��ָ���ƶ�һ��
		slow = queue_next(slow);

		// ��ָ���ƶ�һ��
		fast = queue_next(fast);
		if (fast == queue_last(queue)) {
			return slow;
		}

		// ��ָ������ƶ�һ��
		fast = queue_next(fast);
		if (fast == queue_last(queue)) {
			return slow;
		}
	}
}

/*
	���ò�������
*/
void queue_sort(queue_t *queue, int (*cmp)(const queue_t *, const queue_t *)) {
	queue_t *q, *prev, *next;

	q = queue_head(queue);
	if (q == queue_last(queue)) {
		return;
	}

	for (q = queue_next(q); q != queue_sentinel(queue); q = next) {
		prev = queue_prev(q);
        next = queue_next(q);
		queue_remove(q);

		while (prev != queue_head(queue)) {
			if (cmp(prev, q) < 0) {
				break;
			}
		}

		queue_insert_after(prev, q);
	}
}