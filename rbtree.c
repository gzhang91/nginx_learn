#include "rbtree.h"

static void rbtree_left_rotate(rbtree_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);
static void rbtree_right_rotate(rbtree_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);

int rbtree_insert(rbtree_t *root, rbtree_node_t *node) {

	return 0;
}

int rbtree_delete(rbtree_t *root, rbtree_node_t *node) {

	return 0;
}

int rbtree_insert_value(rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel) {

	return 0;
}

rbtree_node_t *rbtree_next(rbtree_t *root, rbtree_node_t *node) {

	return NULL;
}

void rbtree_left_rotate(rbtree_t **root, rbtree_node_t *sentinel, rbtree_node_t *node) {
	rbtree_node_t *temp = node->right;
	
	if (temp->left) {
		node->right = temp->left;
	}


}

void rbtree_right_rotate(rbtree_t **root, rbtree_node_t *sentinel, rbtree_node_t *node) {

}