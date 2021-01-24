#include "rbtree.h"
#include <stdio.h>

static void rbtree_left_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);
static void rbtree_right_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node);

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node) {
	rbtree_node_t **root, *temp, *sentinel;
	// ����
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
	// ����֮��nodeΪ��ɫ
	tree->insert(tree->root, node, sentinel);
	
	// ����ƽ��
	while (node != *root && rbt_is_red(node->parent)) {
		// ��
		if (node->parent == node->parent->parent->left) {
			temp = node->parent->parent->right;
			if (rbt_is_red(temp)) {
				// ���������Ϊ��ɫ,˵���ڽڵ�����û�иı�,ֻ��ҪͿɫ����
				rbt_red(node->parent->parent);
				rbt_black(node->parent);
				rbt_black(temp);
				node = node->parent->parent;
			} else {
				// ����,��Ҫ������ת,�����ߵ���Ŀ����Ϊһ��
				// �����Z����,����ת��һ����
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(root, sentinel, node);
				}

				rbt_black(node->parent);
				rbt_red(node->parent->parent);
				rbtree_right_rotate(root, sentinel, node->parent->parent);
			}

		} else {
		// ��,����ѳ�
			temp = node->parent->parent->left;
			if (rbt_is_red(temp)) {
				rbt_red(node->parent->parent);
				rbt_black(node->parent);
				rbt_black(temp);
				node = node->parent->parent;
			} else {
				// ��Z���δ���
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
	// ��ȡroot���ڵ�
	root = &tree->root;
	// ��ȡsentinel�ڵ�
	sentinel = tree->sentinel;
	// ���������Ϊ��
	if (node->left == sentinel) {
		temp = node->right;
		subst = node;

	} else if (node->right == sentinel) {
		// ���������Ϊ��
		temp = node->left;
		subst = node;

	} else {
		// ����������Ϊ��
		// �ҵ��������������(��С)�Ľڵ�, Ҳ���Բ���������������(���)�Ľڵ�
		subst = rbtree_min(node->right, sentinel);
		// ����Ӧ��ֱ����temp=subst->right��,һֱ��ƫ(�����������˼���ܾ�û�а������ngx_rbtree_min/max���ض���,
		//	�������������������)
		if (subst->left != sentinel) {
			temp = subst->left;
		} else {
			temp = subst->right;
		}
	}

	// ��ԴӸ���ʼ��������Ϊ�ջ���������Ϊ�ջ�������������Ϊ�յ����
	if (subst == *root) {
		// ֱ�ӽ���ƫ��
		*root = temp;
		// �����ڵ��������Ϊ�ڽڵ�
		rbt_black(temp);

		/* DEBUG stuff */
		node->left = 0;
		node->right = 0;
		node->parent = 0;
		node->key = 0;

		return;
	}
	// ��ȡsubst����ɫ
	red = rbt_is_red(subst);

	// ���ﱣ��subst(ȡ���ڵ�)���¼�����,subst���¼�����ֻ����һ�����ߵ�(Ҫô������,Ҫô������,���������߶���)
	// substΪ������
	if (subst == subst->parent->left) {
		// ��substժ����,����temp���浽ԭ��subst�ĵط�
		subst->parent->left = temp;

	} else {
		// substΪ������
		// ��substժ����,����temp���浽ԭ��subst�ĵط�
		subst->parent->right = temp;
	}
	// ��������п��ܳ���tempΪsentinal
	if (subst == node) {
		temp->parent = subst->parent;

	} else {
		// ����subst->parent==node�������,�����ʾֻ����node���������ڵ����subst,����subst��������Ϊ��
		if (subst->parent == node) {
			temp->parent = subst;

		} else {
			temp->parent = subst->parent;
		}
		// ��ʼ��ֵ,��node->left��ֵ��subst->left
		subst->left = node->left;
		// ��Ϊ�����Ѿ���subst���������ҽӵ�parent����,�������ֱ�Ӹ�ֵ
		subst->right = node->right;
		// ��parentҲ����
		subst->parent = node->parent;
		// ��ɫҲ����
		rbt_copy_color(subst, node);
		// �����root�ڵ�,��root���¸�ֵ
		if (node == *root) {
			*root = subst;

		} else {
			// ���ø�ָ��
			if (node == node->parent->left) {
				node->parent->left = subst;
			} else {
				node->parent->right = subst;
			}
		}
		// ����ԭ��node������������parent
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
	// ���Ϊ��ɫ�ڵ�,ֱ�ӷ��ظýڵ�
	if (red) {
		return;
	}

	/* a delete fixup */
	// �Ӵ��п�ʼ, ��ʾ����delete�Ĳ㼶(����)���²���
	// temp����ײ�Ӱ��Ľڵ�,�ӵײ㵽��һֱ����
	while (temp != *root && rbt_is_black(temp)) {
		// tempΪparent��������
		if (temp == temp->parent->left) {
			// ��ȡ������
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

			// ���w�����Ҷ�Ϊ��ɫ,Ϳɫ����
			if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
				rbt_red(w);
				temp = temp->parent;

			} else {
				// �����Ǻ�ɫ
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
			// tempΪparent��������
			// ��ȡ������ 
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

			// ����������Ϊ��ɫ,Ϳɫ����
			if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
				rbt_red(w);
				temp = temp->parent;

			} else {
				// ����Ϊ��ɫ
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
	// �ȱ���node->rightָ��
	rbtree_node_t *temp = node->right;
	
	// �Ƚ�temp�ڵ��������Ϣ���ú�,������ܳ�������
	// ��temp��������׺�ӵ�node����������
	node->right = temp->left;
	temp->left = node;
	temp->parent = node->parent;

	if (*root == node) {
		*root = temp;
	} else if (node == node->parent->left) {
		// ����node�ĸ��ڵ�ָ����Ϣ
		node->parent->left = temp;
	} else {
		node->parent->right = temp;
	}

	// ������node����Ϣ
	if (node->right != sentinel) {
		node->right->parent = node;
	}

	node->parent = temp;
}

void rbtree_right_rotate(rbtree_node_t **root, rbtree_node_t *sentinel, rbtree_node_t *node) {
	// �ȱ���
	rbtree_node_t *temp = node->left;

	// ������temp�ڵ����Ϣ
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