#include <stdio.h>
#include "radix_tree.h"

int main(int argc, char **argv) {
	radix_tree_t *tree = radix_tree_create(-1);
	if (!tree) {
		printf("create tree failed!\n");
		return -1;
	}

	

	return 0;
}