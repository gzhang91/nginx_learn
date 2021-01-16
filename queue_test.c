#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "queue.h"

struct proc_node_s {
	char proc_name[16];
	int priority;
	int expire_time;
	queue_t queue;
};

typedef struct proc_node_s proc_node_t;

struct proc_s {
	int size;
	queue_t proc_queue;
};

typedef struct proc_s proc_t;

proc_t *InitProc();
void DestroyProc(proc_t *procs);
int AddProc(proc_t *procs, const char *name, int pri, int time);
void DelProc(proc_t *procs, queue_t *h);
void ListProc(proc_t *procs);
int ExecProc(proc_t *procs);
void FinishProc(proc_t *procs);

proc_t *InitProc() {
	proc_t *procs = (proc_t *)malloc(sizeof(*procs));
	if (!procs) {
		return NULL;
	}

	procs->size = 0;
	queue_init(&procs->proc_queue);
}

void DestroyProc(proc_t *procs) {
	while (procs->size) {
		queue_t *h = queue_head(&procs->proc_queue);
		DelProc(procs, h);
	}

	free(procs);
	procs = NULL;
}

int AddProc(proc_t *procs, const char *name, int pri, int time) {
	if (!procs) {
		return -1;
	}

	proc_node_t *node = (proc_node_t *)malloc(sizeof(*node));
	if (!node) {
		return -1;
	}

	memcpy(node->proc_name, name, sizeof(node->proc_name));
	node->priority = pri;
	node->expire_time = time;

	queue_insert_tail(&procs->proc_queue, &node->queue);
	procs->size ++;

	return 0;
}

void DelProc(proc_t *procs, queue_t *h) {
	queue_remove(h);
	proc_node_t *node = queue_data(h, proc_node_t, queue);
	free(node);
	procs->size --;
}

void ListProc(proc_t *procs) {
	if (!procs) {
		return;
	}

	queue_t *h = queue_head(&procs->proc_queue);
	queue_t *q  = h;
	proc_node_t *node = NULL;

	printf("=========================================================\n");
	for (int i = 0; i < procs->size; q = queue_next(q), i++) {
		node = queue_data(q, proc_node_t, queue);
		if (!node) {
			continue;
		}

		printf("name: %s\n", node->proc_name);
		printf("priority: %d\n", node->priority);
		printf("expire_time: %d\n\n", node->expire_time);
	}
}

int ExecProc(proc_t *procs) {
	if (!procs) {
		return -1;
	}

	int time_out = rand() % 100;
	
	queue_t *h = queue_head(&procs->proc_queue);
	proc_node_t *node = queue_data(h, proc_node_t, queue);
	
	printf("Exec process: %s, run_time: %d\n", node->proc_name, time_out);

	if (node->expire_time <= time_out) {
		printf("procss: %s finish\n", node->proc_name);
		DelProc(procs, h);
		return 0;
	}
	
	node->expire_time -= time_out;
	queue_remove(h);
	queue_insert_tail(&procs->proc_queue, h);

	return 0;
}

void FinishProc(proc_t *procs) {
	while (procs->size > 0) {
		ExecProc(procs);
		ListProc(procs);
		sleep(1);
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));

	proc_t *procs = InitProc();
	
	AddProc(procs, "proc1", 1, 90);
	AddProc(procs, "proc2", 12, 10);
	AddProc(procs, "proc3", 2, 30);	
	AddProc(procs, "proc4", 6, 80);
	ListProc(procs);
	FinishProc(procs);
	DestroyProc(procs);

	return 0;
}