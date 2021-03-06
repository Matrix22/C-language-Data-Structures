CFLAGS = -c -g -Wextra -Werror -Wall

FILES = ./scl_avl_tree.c ./scl_bst_tree.c \
		./scl_weighted_graph.c ./scl_list.c \
		./scl_dlist.c ./scl_priority_queue.c \
		./scl_hash_table.c ./scl_sort_algo.c \
		./scl_ordered_graph.c ./scl_red_black_tree.c \
		./scl_queue.c ./scl_stack.c \
		./scl_unordered_graph.c ./scl_func_types.c \

OUTFILES = scl_avl_tree.o scl_bst_tree.o scl_weighted_graph.o \
		 scl_dlist.o scl_hash_table.o \
		 scl_priority_queue.o scl_list.o scl_ordered_graph.o \
		 scl_red_black_tree.o scl_queue.o scl_sort_algo.o \
		 scl_stack.o scl_unordered_graph.o scl_func_types.o \
		 

LIB = libdatastruc.a
LIBNAME = datastruc

TESTFILE = ztest

build: objFiles staticLib clean

objFiles: $(FILES)
	@gcc $(CFLAGS) $(FILES)

staticLib: $(OUTFILES)
	@ar -rc $(LIB) *.o

delLib: $(LIB)
	@rm -rf $(LIB)

test: $(LIB) $(TESTFILE).c
	@gcc $(TESTFILE).c -g -Wall -Wextra -L. -l$(LIBNAME) -o $(TESTFILE)

run: test
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TESTFILE)

clean:
	@find . -type f -name "*.o" -exec rm -rf {} \;
	@rm -rf $(TESTFILE)