#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "rbtree.h"

typedef enum file_type {
	DIR_TYPE = 0,
	FILE_TYPE = 1,
} file_type;

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

void rbtree_file_insert(rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel) {
	rbtree_node_t **p = NULL;

	while (1) {
		if (root->key > node->key) {
			p = &root->left;
		} else if (root->key < node->key) {
			p = &root->right;
		} else {
			// == equal ===
			file_node_t *file_temp = (file_node_t *)root;
			file_node_t *file_node = (file_node_t *)node;

			p = (strcmp(file_temp->file_name, file_node->file_name) < 0 ? &root->right : &root->left);
		}
		
		if (*p == sentinel) {
			break;
		}

		root = *p;
	}

	*p = node;
	node->left = sentinel;
	node->right = sentinel;
	node->parent = root;
	rbt_red(node);
}

int file_cache_create_node(file_cache_t *cache, int key, const char *file_name, file_type type) {
	if (!cache) {
		return -1;
	}

	file_node_t *node = (file_node_t *)malloc(sizeof(*node));
	if (!node) {
		printf("malloc create node failed!\n");
		return -1;
	}

	strncpy(node->file_name, file_name, sizeof(node->file_name));
	node->type = type;
	node->load_time = time(NULL);
	node->node.key = key;

	rbtree_insert(&cache->root, &node->node);

	return 0;
}

file_node_t *file_cache_lookup_by_name(file_cache_t *cache, const char *file_name) {
	rbtree_node_t **p = NULL;
	rbtree_node_t *temp = cache->root.root;
	file_node_t *file_node = NULL;

	if (temp == &cache->sentinel) {
		return NULL;
	}

	for (; ;) {
		file_node = (file_node_t *)temp;
		int cmp_val = strcmp(file_node->file_name, file_name);
		
		if (cmp_val > 0) {
			p = &temp->left;
		} else if (cmp_val < 0) {
			p = &temp->right;
		} else {
			return file_node;
		}

		temp = *p;
	}

	return NULL;
}

file_node_t *file_cache_lookup_by_key(file_cache_t *cache, int key) {
	rbtree_node_t **p = NULL;
	rbtree_node_t *temp = cache->root.root;
	file_node_t *file_node = NULL;

	if (temp == &cache->sentinel) {
		return NULL;
	}

	for (; ;) {
		file_node = (file_node_t *)temp;
		if (file_node->node.key > key) {
			p = &temp->left;
		} else if (file_node->node.key < key) {
			p = &temp->right;
		} else {
			return file_node;
		}

		temp = *p;
	}

	return NULL;
}

int file_cache_delete_node_by_name(file_cache_t *cache, const char *file_name) {
	file_node_t *file_node = file_cache_lookup_by_name(cache, file_name);
	if (!file_node) {
		printf("Cannot find this node, file name is %s\n", file_name);
		return -1;
	}

	rbtree_delete(&cache->root, &file_node->node);
	free(file_node);

	return 0;
}

int file_cache_delete_node_by_key(file_cache_t *cache, int key) {
	file_node_t *file_node = file_cache_lookup_by_key(cache, key);
	if (!file_node) {
		printf("Cannot find this node, key is %d\n", key);
		return -1;
	}

	rbtree_delete(&cache->root, &file_node->node);
	free(file_node);

	return 0;
}

void file_cache_show(file_cache_t *cache) {
	file_node_t *file_node = NULL;
	if (!cache || cache->root.root == &cache->sentinel) {
		return;
	}

	printf("====== show tree =======\n");
	rbtree_node_t *start = rbtree_first(&cache->root);
	for (; start != NULL; start = rbtree_next(&cache->root, start)) {
		file_node = (file_node_t *)start;
		printf("file key: %d\n", file_node->node.key);
		printf("file name: %s\n", file_node->file_name);
		printf("file type: %s\n", (file_node->type == DIR_TYPE ? "directory" : "file"));
		printf("create time: %ld\n", file_node->load_time);
	}
	printf("\n");
}

int main(int argc, char **argv) {
	file_cache_t file_cache;
	file_node_t *find_node = NULL;

	rbtree_init(&file_cache.root, &file_cache.sentinel, rbtree_file_insert);
	file_cache_create_node(&file_cache, 1, "/etc/smb.conf", FILE_TYPE);
	file_cache_create_node(&file_cache, 8, "/etc", DIR_TYPE);
	file_cache_create_node(&file_cache, 3, "/tmp", DIR_TYPE);
	file_cache_create_node(&file_cache, 17, "/usr/local/ls", FILE_TYPE);
	file_cache_create_node(&file_cache, 5, "/usr/bin/ls", FILE_TYPE);
	file_cache_show(&file_cache);

	file_cache_delete_node_by_key(&file_cache, 1);
	file_cache_show(&file_cache);
	// file_cache_delete_node_by_key(&file_cache, 17);
	file_cache_delete_node_by_name(&file_cache, "/usr/local/ls");
	file_cache_show(&file_cache);
	file_cache_delete_node_by_key(&file_cache, 8);
	file_cache_show(&file_cache);
	file_cache_delete_node_by_key(&file_cache, 3);
	file_cache_show(&file_cache);
	file_cache_delete_node_by_key(&file_cache, 5);
	file_cache_show(&file_cache);

	file_cache_delete_node_by_key(&file_cache, 100);
	file_cache_show(&file_cache);

	return 0;
}