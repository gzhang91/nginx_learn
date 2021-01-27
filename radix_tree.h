#ifndef _RADIX_TREE_H_
#define _RADIX_TREE_H_

#include <stdint.h>
#include <stddef.h>

typedef unsigned int UINT;

#define PAGE_SIZE (4 * 1024)
#define RADIX_NO_VALUE (uintptr_t) -1

typedef struct radix_node_s radix_node_t;

struct radix_node_s {
	radix_node_t *right;
	radix_node_t *left;
	radix_node_t *parent;
	uintptr_t value;
};

typedef struct {
	radix_node_t *root;
	radix_node_t *free;
	char *start;
	size_t size;
} radix_tree_t;

radix_tree_t *radix_tree_create(int preallocate);

int radix32tree_insert(radix_tree_t *tree, UINT key, UINT mask, uintptr_t value);
int radix32tree_delete(radix_tree_t *tree, UINT key, UINT mask);
uintptr_t radix32tree_find(radix_tree_t *tree, UINT key);

#endif // _RADIX_TREE_H_