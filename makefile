# makefile

SRCS=main.c             \
  rbtree.c              \
  queue.c               \
  open_file_cache.c     

OBJS=main.o             \
  rbtree.o            \
  queue.o             \
  open_file_cache.o   

BIN=ngx_copy

QUEUE_TEST=queue_test
BTREE_TEST=rbtree_test

$(BIN): $(SRCS)
	gcc -g -o $@ $^

$(QUEUE_TEST): queue_test.c queue.c
	gcc -g -o $@ $^
	
$(BTREE_TEST): rbtree_test.c rbtree.c
	gcc -g -o $@ $^

clean:
	rm -rf $(BIN) $(QUEUE_TEST) $(OBJS)
