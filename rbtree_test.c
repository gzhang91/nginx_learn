#include <stdio.h>
#include <time.h>
#include "rbtree.h"

enum file_type {
	DIR = 0,
	FILE = 1,
};

struct file_node_s {
	rbtree_node_t node;
	char file_name[32];
	file_type type;
	time_t load_time;
};

typedef struct file_node_s file_node_t;

struct file_cache_s {
	rbtree_t root;
	rbtree_node_t sentinel;
	// ...
};

typedef struct file_cache_s file_cache_t;

void rbtree_file_insert(rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel);

void rbtree_file_insert(rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel) {

}

int main(int argc, char **argv) {
	file_cache_t file_cache;

	rbtree_init(&file_cache.root, &file_cache.sentinel, rbtree_file_insert);

}