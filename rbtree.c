#include "rbtree.h"
#include <stdio.h>

static void rbtree_left_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);
static void rbtree_right_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node) {
	rbtree_node_t **root, *temp, *sentinel;
	// 插入
	root = &tree->root;
	sentinel = tree->sentinel;
	if (*root == sentinel) {
		node->color = BLACK;
		node->parent = 0;
		node->left = sentinel;
		node->right = sentinel;

		*root = node;
		return;
	}
	// 插入之后node为红色
	tree->insert(tree->root, node, sentinel);
	
	// 调整平衡
	while (node != *root && rbt_is_red(node->parent)) {
		// 左
		if (node->parent == node->parent->parent->left) {
			temp = node->parent->parent->right;
			if (rbt_is_red(temp)) {
				// 如果右子树为红色,说明黑节点数并没有改变,只需要涂色即可
				rbt_red(node->parent->parent);
				rbt_black(node->parent);
				rbt_black(temp);
				node = node->parent->parent;
			} else {
				// 否则,需要进行旋转,将两边的数目调整为一致
				// 如果是Z字形,先旋转成一字型
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(root, sentinel, node);
				}

				rbt_black(node->parent);
				rbt_red(node->parent->parent);
				rbtree_right_rotate(root, sentinel, node->parent->parent);
			}

		} else {
		// 右,和左堆成
			temp = node->parent->parent->left;
			if (rbt_is_red(temp)) {
				rbt_red(node->parent->parent);
				rbt_black(node->parent);
				rbt_black(temp);
				node = node->parent->parent;
			} else {
				// 对Z字形处理
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_right_rotate(root, sentinel, node);
				}

				rbt_black(node->parent);
				rbt_red(node->parent->parent);
				rbtree_left_rotate(root, sentinel, node->parent->parent);
			}
		}
	}

	rbt_black(*root);
}

void rbtree_delete(rbtree_t *tree, rbtree_node_t *node) {
	rbtree_color_e red;
	rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

	/* a binary tree delete */
	// 获取root根节点
	root = &tree->root;
	// 获取sentinel节点
	sentinel = tree->sentinel;
	// 如果左子树为空
	if (node->left == sentinel) {
		temp = node->right;
		subst = node;

	} else if (node->right == sentinel) {
		// 如果右子树为空
		temp = node->left;
		subst = node;

	} else {
		// 左右子树不为空
		// 找到右子树中最左边(最小)的节点, 也可以查找左子树的最右(最大)的节点
		subst = rbtree_min(node->right, sentinel);
		// 这里应该直接是temp=subst->right吧,一直左偏(作者这里的意思可能就没有按上面的ngx_rbtree_min/max来特定化,
		//	而是两种情况都考虑了)
		if (subst->left != sentinel) {
			temp = subst->left;
		} else {
			temp = subst->right;
		}
	}

	// 针对从根开始的左子树为空或者右子树为空或者左右子树都为空的情况
	if (subst == *root) {
		// 直接进行偏移
		*root = temp;
		// 将根节点必须设置为黑节点
		rbt_black(temp);

		/* DEBUG stuff */
		node->left = 0;
		node->right = 0;
		node->parent = 0;
		node->key = 0;

		return;
	}
	// 获取subst的颜色
	red = rbt_is_red(subst);

	// 这里保存subst(取代节点)的下继子树,subst的下继子树只能是一个单边的(要么左子树,要么右子树,不可能两者都是)
	// subst为左子树
	if (subst == subst->parent->left) {
		// 将subst摘下来,并将temp保存到原来subst的地方
		subst->parent->left = temp;

	} else {
		// subst为右子树
		// 将subst摘下来,并将temp保存到原来subst的地方
		subst->parent->right = temp;
	}
	// 这种情况有可能出现temp为sentinal
	if (subst == node) {
		temp->parent = subst->parent;

	} else {
		// 对于subst->parent==node的情况下,这里表示只能是node的右子树节点就是subst,而且subst左右子树为空
		if (subst->parent == node) {
			temp->parent = subst;

		} else {
			temp->parent = subst->parent;
		}
		// 开始赋值,将node->left赋值给subst->left
		subst->left = node->left;
		// 因为上面已经将subst的右子树挂接到parent上了,这里可以直接赋值
		subst->right = node->right;
		// 将parent也拷贝
		subst->parent = node->parent;
		// 颜色也拷贝
		rbt_copy_color(subst, node);
		// 如果是root节点,将root重新赋值
		if (node == *root) {
			*root = subst;

		} else {
			// 设置父指针
			if (node == node->parent->left) {
				node->parent->left = subst;
			} else {
				node->parent->right = subst;
			}
		}
		// 设置原来node的左右子树的parent
		if (subst->left != sentinel) {
			subst->left->parent = subst;
		}

		if (subst->right != sentinel) {
			subst->right->parent = subst;
		}
	}

	/* DEBUG stuff */
	node->left = 0;
	node->right = 0;
	node->parent = 0;
	node->key = 0;
	// 如果为红色节点,直接返回该节点
	if (red) {
		return;
	}

	/* a delete fixup */
	// 从此行开始, 表示进行delete的层级(可能)更新操作
	// temp是最底层影响的节点,从底层到上一直更新
	while (temp != *root && rbt_is_black(temp)) {
		// temp为parent的左子树
		if (temp == temp->parent->left) {
			// 获取右子树
			w = temp->parent->right;

			if (rbt_is_red(w)) {
				rbt_black(w);
				rbt_red(temp->parent);
				rbtree_left_rotate(root, sentinel, temp->parent);
				w = temp->parent->right;
			}

			//if (!w->left || !w->right) {
			//	break;
			//}

			// 如果w的左右都为黑色,涂色即可
			if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
				rbt_red(w);
				temp = temp->parent;

			} else {
				// 不都是黑色
				// 
				if (rbt_is_black(w->right)) {
					rbt_black(w->left);
					rbt_red(w);
					rbtree_right_rotate(root, sentinel, w);
					w = temp->parent->right;
				}

				rbt_copy_color(w, temp->parent);
				rbt_black(temp->parent);
				rbt_black(w->right);
				rbtree_left_rotate(root, sentinel, temp->parent);
				temp = *root;
			}

		} else {
			// temp为parent的右子树
			// 获取左子树 
			w = temp->parent->left;

			if (rbt_is_red(w)) {
				rbt_black(w);
				rbt_red(temp->parent);
				rbtree_right_rotate(root, sentinel, temp->parent);
				w = temp->parent->left;
			}

			/*if (!w->left || !w->right) {
				break;
			}*/

			// 左右子树都为黑色,涂色即可
			if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
				rbt_red(w);
				temp = temp->parent;

			} else {
				// 不都为黑色
				if (rbt_is_black(w->left)) {
					rbt_black(w->right);
					rbt_red(w);
					rbtree_left_rotate(root, sentinel, w);
					w = temp->parent->left;
				}

				rbt_copy_color(w, temp->parent);
				rbt_black(temp->parent);
				rbt_black(w->left);
				rbtree_right_rotate(root, sentinel, temp->parent);
				temp = *root;
			}
		}
	}

	rbt_black(temp);
}

void rbtree_insert_value(rbtree_node_t *temp, rbtree_node_t *node, rbtree_node_t *sentinel) {
	rbtree_node_t **p = 0;

	while (1) {
		p = ((temp->key > node->key) ? &temp->left : &temp->right); 

		if (*p == sentinel) {
			break;
		}

		temp = *p;
	}
	*p = node;
	rbt_red(*p);
	node->left = sentinel;
	node->right = sentinel;
	node->parent = temp;
}

rbtree_node_t *rbtree_first(rbtree_t *tree) {
	if (tree->root == tree->sentinel) {
		return NULL;
	}

	if (tree->root->left == tree->sentinel) {
		return tree->root;
	}

	return rbtree_min(tree->root->left, tree->sentinel);
}

rbtree_node_t *rbtree_next(rbtree_t *root, rbtree_node_t *node) {
	rbtree_node_t *parent = 0;

	if (node->right != root->sentinel) {
		return rbtree_min(node->right, root->sentinel);
	}

	while (1) {
		parent = node->parent;
		if (node == root->root) {
			return NULL;
		}

		if (node == parent->left) {
			return parent;
		}

		node = parent;
	}

	return NULL;
}

void rbtree_left_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node) {
	// 先保存node->right指针
	rbtree_node_t *temp = node->right;
	
	// 先将temp节点的数据信息设置好,否则可能出现问题
	// 将temp的左子树缀接到node的右子树上
	node->right = temp->left;
	temp->left = node;
	temp->parent = node->parent;

	if (*root == node) {
		*root = temp;
	} else if (node == node->parent->left) {
		// 设置node的父节点指向信息
		node->parent->left = temp;
	} else {
		node->parent->right = temp;
	}

	// 再设置node的信息
	if (node->right != sentinel) {
		node->right->parent = node;
	}

	node->parent = temp;
}

void rbtree_right_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node) {
	// 先保存
	rbtree_node_t *temp = node->left;

	// 先设置temp节点的信息
	node->left = temp->right;
	temp->right = node;
	temp->parent = node->parent;

	if (node->left != sentinel) {
		node->left->parent = node;
	}
	
	if (*root == node) {
		*root = temp;
	} else if (node == node->parent->left) {
		node->parent->left = temp;
	} else {
		node->parent->right = temp;
	}

	node->parent = temp;
}