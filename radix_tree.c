#include <stdlib.h>
#include <stdio.h>
#include "radix_tree.h"

static radix_node_t *radix_alloc(radix_tree_t *tree);

radix_tree_t *radix_tree_create(int preallocate) {
    UINT key, mask, inc;
    radix_tree_t  *tree;
		// 申请radix_tree_t结构体对象
    tree = malloc(sizeof(radix_tree_t));
    if (tree == NULL) {
        return NULL;
    }
	
    tree->free = NULL;
    tree->start = NULL;
    tree->size = 0;
	// 申请root节点
    tree->root = radix_alloc(tree);
    if (tree->root == NULL) {
        return NULL;
    }

    tree->root->right = NULL;
    tree->root->left = NULL;
    tree->root->parent = NULL;
    // 设置NO_VALUE
    tree->root->value = RADIX_NO_VALUE;
	// preallocate == 0
    if (preallocate == 0) {
        return tree;
    }

    if (preallocate == -1) {
        switch (PAGE_SIZE / sizeof(radix_node_t)) {

        /* amd64 */
        case 128:
            preallocate = 6;
            break;

        /* i386, sparc64 */
        case 256:
            preallocate = 7;
            break;

        /* sparc64 in 32-bit mode */
        default:
            preallocate = 8;
        }
    }

    mask = 0;
    inc = 0x80000000;

    while (preallocate--) {

        key = 0;
        mask >>= 1;
        mask |= 0x80000000;
				printf("================================\n");
				int count = 0;
        do {
					printf("key:0x%08X, mask:0x%08X\n", key, mask);
            if (radix32tree_insert(tree, key, mask, RADIX_NO_VALUE) != 0) {
                return NULL;
            }

            key += inc;
						count ++;
						//printf("key=0x%X, mask=0x%X, inc=0x%X\n", key, mask, inc);
        } while (key);
				printf("total: %d\n", count);

        inc >>= 1;
    }

    return tree;
}

int radix32tree_insert(radix_tree_t *tree, UINT key, UINT mask, uintptr_t value) {
	UINT bit;
	radix_node_t  *node, *next;

	bit = 0x80000000;

	node = tree->root;
	next = tree->root;

	while (bit & mask) {
		if (key & bit) {
			next = node->right;

		} else {
			next = node->left;
		}
		
		if (next == NULL) {
			break;
		}

		bit >>= 1;
		node = next;
	}

	if (next) {
		if (node->value != RADIX_NO_VALUE) {
			return -1;
		}

		node->value = value;
		return 0;
	}

	while (bit & mask) {
		next = radix_alloc(tree);
		if (next == NULL) {
			return -2;
		}

		next->right = NULL;
		next->left = NULL;
		next->parent = node;
		next->value = RADIX_NO_VALUE;

		if (key & bit) {
			node->right = next;

		} else {
			node->left = next;
		}

		bit >>= 1;
		node = next;
	}

	node->value = value;

	return 0;
}

int radix32tree_delete(radix_tree_t *tree, UINT key, UINT mask) {
	UINT bit;
	radix_node_t  *node;

	bit = 0x80000000;
	node = tree->root;

	while (node && (bit & mask)) {
		if (key & bit) {
			node = node->right;

		} else {
			node = node->left;
		}

		bit >>= 1;
	}

	if (node == NULL) {
		return -2;
	}

	if (node->right || node->left) {
		if (node->value != RADIX_NO_VALUE) {
			node->value = RADIX_NO_VALUE;
			return 0;
		}

		return -2;
	}

	for ( ;; ) {
		if (node->parent->right == node) {
			node->parent->right = NULL;

		} else {
			node->parent->left = NULL;
		}

		node->right = tree->free;
		tree->free = node;

		node = node->parent;

		if (node->right || node->left) {
			break;
		}

		if (node->value != RADIX_NO_VALUE) {
			break;
		}

		if (node->parent == NULL) {
			break;
		}
	}

	return 0;
}

static radix_node_t *radix_alloc(radix_tree_t *tree) {
	radix_node_t  *p;
	// free列表存在
	if (tree->free) {
		p = tree->free;
		tree->free = tree->free->right;
		return p;
	}
	// 如果tree->size < sizeof(ngx_radix_node_t)
	if (tree->size < sizeof(radix_node_t)) {
		// ngx_pagesize 对齐
		tree->start = (char *)malloc(sizeof(char) * PAGE_SIZE);
		if (tree->start == NULL) {
			return NULL;
		}

		tree->size = PAGE_SIZE;
	}

	p = (radix_node_t *) tree->start;
	tree->start += sizeof(radix_node_t);
	tree->size -= sizeof(radix_node_t);

	return p;
}
