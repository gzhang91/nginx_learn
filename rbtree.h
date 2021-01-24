#ifndef _RBTREE_H_
#define _RBTREE_H_

enum rbtree_color_s {
	BLACK = 0,
	RED
};

typedef enum rbtree_color_s rbtree_color_e;

struct rbtree_node_s {
	unsigned int key;
	struct rbtree_node_s *left;
	struct rbtree_node_s *right;
	struct rbtree_node_s *parent;
	rbtree_color_e color;
};

typedef struct rbtree_node_s rbtree_node_t;

typedef void (*rbtree_insert_pt) (rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel);

struct rbtree_s {
	rbtree_node_t *root;
	rbtree_node_t *sentinel;
	rbtree_insert_pt insert;
};

typedef struct rbtree_s rbtree_t;

// 关于color的宏函数
#define rbt_red(node)            ((node)->color = RED)
#define rbt_black(node)		     ((node)->color = BLACK)
#define rbt_is_red(node)         ((node)->color == RED)
#define rbt_is_black(node)       ((node)->color == BLACK)
#define rbt_copy_color(n1, n2)   ((n1)->color = (n2)->color)
#define rbtree_sentinel_init(node)  rbt_black(node)

// 操作函数
#define rbtree_init(tree, s, i) \
	rbtree_sentinel_init(s);			\
	(tree)->root = s;							\
	(tree)->sentinel = s;					\
	(tree)->insert = i

void rbtree_insert(rbtree_t *root, rbtree_node_t *node);
void rbtree_delete(rbtree_t *root, rbtree_node_t *node);
void rbtree_insert_value(rbtree_node_t *root, rbtree_node_t *node, rbtree_node_t *sentinel);
rbtree_node_t *rbtree_first(rbtree_t *tree);
rbtree_node_t *rbtree_next(rbtree_t *tree, rbtree_node_t *node);

static rbtree_node_t *rbtree_min(rbtree_node_t *node, rbtree_node_t *sentinel) {
	while (node->left != sentinel) {
		node = node->left;
	}

	return node;
}

#endif // _RBTREE_H_