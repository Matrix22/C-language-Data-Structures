# Documentation for AVL tree object (scl_avl_tree.h)

## How to create a AVL tree and how to destroy it?

If you read all previous documentations on lists, stacks and queue that the process of creating and removing are quite similar.

For creating a avl tree object you just need to:

```C
    // Somewhere in your program

    avl_tree_t *my_tree = create_avl(&compare_data, &free_data);
```

>**NOTE:** As you observe you should provide a compare and a free data function, however if the compare function is a must, because data is inserted in the avl according to compare function the free function is optional. If you insert data that is not dynamically allocated by yourself and it is maintained on the stack that you must not provide a free function.

Example of free function:

```C
    // Suppose your desire data is like
    typedef struct {
        char *name;
        int age;
    } person;

    // Than your free function mush be like

    void freePerson(void *data) {
        if (data == NULL) return;

        preson *t_data = data;

        free(t_data->name);
    }
```

>**NOTE:** In any other case that you do not allocate memory for your single data element that no free function should be provided.

If you want to delete a AVL tree object and to free evry byte of memory ocupied by it you should call the following function:

```C
    // Somewhere in your code

    free_avl(my_tree);

    // As simple as that, you also can pass a NULL tree
```

## How to insert and how to remove elements from AVL tree ?

According to following functions:

```C
    int avl_insert(avl_tree_t *tree, const void *data, size_t data_size);
    int avl_delete(avl_tree_t *tree, void *data, size_t data_size);
```

Let's assume that we work with integers in our program, so for now no need for a free function that's good:

```C
    int main(void) {
        avl_tree_t *my_tree = create_avl(&compare_int, 0);
        // You can find compare_int into scl_func_types.h

        for (int i = 0; i < 100; ++i) {
            int data = 0;
            scanf("%d", &data);
            avl_insert(my_tree, &data, sizeof(data));
        }

        int remove_data = 7;

        // Removes node containing 7 if it exists on my_tree
        avl_delete(my_tree, &remove_data, sizeof(remove_data));

        // Do not forghet to free memory
        free_avl(my_tree);

        return 0;
    }
```

>**NOTE:** You are not allowed to insert different object types into the avl Tree. The data has to have the same type, otherwise the behavior will evolve into a segmentation fault.

## Accessing nodes and data from AVL tree ?

For this section we have the following functions:

```C
    int is_avl_empty(avl_tree_t *tree);
    avl_tree_node_t* avl_find_data(avl_tree_t *tree, const void *data);
    int avl_node_level(avl_tree_node_t *baseNode);
    avl_tree_node_t* get_avl_root(avl_tree_t *tree);
    size_t get_avl_size(avl_tree_t *tree);
    avl_tree_node_t* avl_max_node(avl_tree_node_t *root);
    avl_tree_node_t* avl_min_node(avl_tree_node_t *root);
    void* avl_max_data(avl_tree_node_t *root);
    void* avl_min_data(avl_tree_node_t *root);
    avl_tree_node_t* avl_predecessor_node(avl_tree_t *tree, const void *data);
    avl_tree_node_t* avl_successor_node(avl_tree_t *tree, const void *data);
    void* avl_predecessor_data(avl_tree_t *tree, const void *data);
    void* avl_succecessor_data(avl_tree_t *tree, const void *data);
    avl_tree_node_t* avl_lowest_common_ancestor_node(
        avl_tree_t *tree, const void *data1, const void *data2
    );
    void* avl_lowest_common_ancestor_data(
        avl_tree_t *tree, const void *data1, const void *data2
    );
```

The functions do exacty what their name says, now let's see some quick examples of how to use them

```C
    /* Again we are working with int type,
       but you can work with any type even your defined type
    */

    // I will need a function to work with the nodes

    void printData(const avl_tree_node_t *node) {
        if (node == NULL || node->data == NULL)
            return;

        printf("%d ", *(const int *)node->data);
    }

    // Now let the carnage begin
    int main(void) {
        avl_tree_t *my_tree = create_avl(&compare_int, 0);

        for (int i = 0; i < 100; ++i) {
            int data = 0;
            scanf("%d", &data);
            avl_insert(my_tree, &data, sizeof(data));
        }

        int data = 4;
        printf("Level of node 4 is : %d\n", avl_node_level(avl_find_data(my_tree, &data)));

        printf("Successor and Predecessor of node 4 is:\n");
        printData(avl_successor_node(my_tree, &data));
        printData(avl_predecessor_node(my_tree, &data));

        int data = 7;
        // !!! I supposed here that there is a successor of a successor
        printf("Successor of node's 7 successor is : %d",
                *(const int *)avl_successor_data(my_tree, &data));

        

        free_avl(avl);
    }
```

>**NOTE:** The rest of the functions that were not described in the example above work just like them, for example avl_min_node returns the minimum node from AVL tree and avl_min_data return the minimum data from the AVL tree.

## How to print the AVL tree, can I modify all nodes ?

I have prepared 4 functions that will help you traverse you AVL tree:

* avl_traverse_inorder
* avl_traverse_preorder
* avl_traverse_postorder
* avl_traverse_level

>**NOTE:** All of the above functions take as input your AVL tree and traverse evry single node from it.

>**NOTE:** We will discuss juust one function of them, because they do the same thing but in different methods.

**Let's print some nodes** :  you want to print all nodes not just successor or the lowest common ancestor, first you will have to define an **action** function that will do this job for you.
The definition of an **action** function is:

```C
    void action(const avl_tree_node_t *node);
```

>**It takes one node and does whatever it wants**

If we want to print the nodes we will have to define the **printData** functions from the above code:

```C
    // this is an action function, takes a node and does something
    void print_data(const avl_tree_node_t *node) {
        if (node == NULL || node->data == NULL)
            return;

        printf("%d ", *(const int *)node->data);
    }

    int main() {
        // Suppose you've allocated a avl tree and inserted in it elements

        // One way of printing the nodes is

        avl_traverse_inorder(mytree, &print_data);

        // Yes !!! Simple as that
    }
```

If you want to do something more interesting you can define another action function as follows:

```C
    void map_nodes(const avl_tree_node_t *node) {
        if (node == NULL || node->data == NULL)
            return;

        int *fa = node->data;

        *fa = *fa + 10;
    }

    // This function will take every node and will increase it's value with 10
```

>**NOTE:** You are free to define any action function, **BUT** if your action functions modify the data from the nodes you **MUST** provide an injective function, it is a **MUST** because just injective mapping functions will preserve the binary searc proprety.

>**NOTE:** Suppose your mapping function is a square function which is not an injective function that **(-1)** node and **(1)** node will have the same value after mapping which will break the AVL tree.

## What if I want to print the output in a file ?

It is very easy to print your data from the avl in another file or another stream, you'll have to use the `freopen` function provided by standard library of C.

Example:

```C
    int main(void) {
        avl_tree_t *my_tree = create_avl(&compare_int, 0);

        // Here insert you're data as you like

        FILE *fout = NULL;

        if ((fout = freopen("outputfile.txt", "w", stdout)) == NULL) {
            printf("Could not redirect stream\n");
            exit(1);
        }

        // Now print every thing you like as printing
        // in stdout stream

        printf("Hi there!\n"); // This will be printed in outputfile.txt

        // This also will be printed in outputfile.txt
        avl_traverse_level(my_tree, &print_data);
        printf("\n");

        free_avl(my_tree);
        fclose(fout);
    }
```

>**NOTE:** You can also use freopen on **stdin** stream to read data from a desired file and not to worry about `fscanf, fgets, etc...`.

---
## For some other examples of using AVL trees you can look up at /examples/avl_tree