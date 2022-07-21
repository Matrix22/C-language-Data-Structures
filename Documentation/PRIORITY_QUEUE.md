# Documentation for queue object (scl_priority_queue.h)

## How to create a priority queue and how to destroy it?

In the scl_priority_queue.h you have three functions that will help you by creating a queue and destroying it.

```C
    priority_queue_t* create_priority_queue(
        size_t init_capacity,
        int (*compare_data)(const void*, const void*),
        int (*compare_priority)(const void*, const void*),
        void (*free_data)(void*),
        void (*free_priority)(void*)
    );

    void free_priority_queue(
        priority_queue_t* pqueue
    );

    priority_queue_t* heapify(
        const void* data,
        const void* priority,
        size_t data_size,
        size_t pri_size,
        size_t number_of_data,
        int (*compare_data)(const void*, const void*),
        int (*compare_priority)(const void*, const void*), 
        void (*free_data)(void*),
        void (*free_priority)(void*)
    );
```

`create_priority_queue` and `heapify` will both allocate memory on heap and create a priority queue and will return a pointer to it's location on heap memory. Priority queue is implemented with **HEAPS**. First function `create_priority_queue` will create an empty priority queue, while `heapify` will take an array of the same data and priority type and will make a priority queue using input data (in C++ the heapify method is equal to **emplace** from priority_queue template).

Example of creating and deleting a priority queue:

```C
    #include <scl_datastruc.h>

    int main(void) {
        priority_queue_t* pq = create_priority_queue(
            20, &compare_int, &compare_int, NULL, NULL);

        // pq = [NULL, NULL, ... (20 times) .., NULL];

        /* Using heapify method */

        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        priority_queue* pq = heapify(
            arr, arr, sizeof(int), sizeof(int),
            sizeof(arr)/ sizeof(arr[0]), &compare_int, &compare_int,
            NULL, NULL);

        /*
            pq =    9
                  /   \
                 8     6
                / \   / \
               7   3 2   5
              / \
             1   4
        */
        
        free_priority_queue(pq);
    }
```

>**NOTE:** Your priority depends strictly on `compare_priority` function:
    Let **a** be first element taken and **b** second element taken, if a > b => >1 then your priority queue
    will be a max priority queue (Maximum number will be taken first). If a > b => <-1 then your priority queue
    will be a min priority queue for example you can take **compare_int** function from **scl_func_types** and to multiply return values with `-1` and you will get a min priority queue.

>**NOTE:** You can make **HEAPS** from priority queues. Because priority queues are implemented from heaps principles a priority queue will for a heap, however if you want to get just heap, set to NULL **data** and **compare_data** and **free_data** pointers.

Example of creating a heap:

```C
    priority_queue* pq = create_priority_queue(20, 0, &compare_int, 0, 0);

    free_priority_queue(pq);
```

>**NOTE:** Even if you are working with not NULL data pointer (you keep some keys) then you can set **compare_data** to NULL, this function is used just to find a desired data to change its priority but if you o not care about comparing data then you are free to set it to NULL. However all priority functions MUST not be **NULL**, because the data is arranged in priority queue according to its priority.

>**NOTE:** I recommend to read all the readmes of data structures to understand how to use free_data, free_priority, compare_data, compare_priority functions from every structure.

## How to insert and how to remove elements from priority queue?

You have 4 function that will maintain a queue:

```C
    int pri_queue_push(
        priority_queue_t* pqueue,
        const void* data,
        const void* priority,
        size_t data_size,
        size_t pri_size);

    const void* pri_queue_top(
        priority_queue_t* pqueue
    );

    const void* pri_queue_top_pri(
        priority_queue_t* pqueue
    );

    int pri_queue_pop(
        priority_queue_t* pqueue
    );
```

It is very important to understand that in order to maintain the propreties of the priority queue the user is not allowed to modify by it's own the priority of one node because it will destroy the priority queue so the return value of the **pri_queue_top_pri** is **const**, however you are allowed to do what you want with your data later we will cover a function to change your data.

`pri_queue_push` is a function to insert one node into the priority queue, the priority queue will itself arrange the node, also you can push a NULL data pointer, but you cannot push a NULL priority pointer, also the priority data size MUST not be 0, otherwise an exception will be thrown.

`pri_queue_pop` will remove the highest ranked node from the priority queue. If priority queue is a max heap than the maximum element will be removed, if priority queue is a min heap than the minimum element will be removed. (Remember you can arrange your data, according to your fuctions it should not be a max or min function, you can define your own functions that will compare the priority, for example in a string you can consider a bigger string with bigger length, or with more small letters than another string and so own, the concept of min and max is very abstract).

Example of using above functions:

```C
    #include "./include/scl_datastruc.h"

    void print_data(const pri_node_t *node) {
        if (node == NULL)
            return;

        printf("(%d) -> (%d)\n", *(int *)node->pri, *(int *)node->data);
    }

    int main(void) {

        FILE *fin = NULL;
        FILE *fout = NULL;

        if ((fin = freopen("ztest_in.txt", "r", stdin)) == NULL ||
            (fout = freopen("ztest_out.txt", "w", stdout)) == NULL) {
                exit(1);
            }

        int nr_of_elem = 0;
        scanf("%d", &nr_of_elem);

        priority_queue_t* pq = create_priority_queue(
            10, &compare_int, &compare_int, NULL, NULL);

        for (int i = 0; i < nr_of_elem; ++i) {
            int data = 0;
            scanf("%d", &data);

            pri_queue_push(pq, &data, &data, sizeof(data), sizeof(data));
        }

        pri_queue_pop(pq);
        pri_queue_pop(pq);
        pri_queue_pop(pq);
        pri_queue_pop(pq);

        int new_data = -100;
        pri_queue_push(pq, &new_data, &new_data, sizeof(new_data), sizeof(new_data));

        int *top_data = pri_queue_top(pq);

        if (NULL != top_data) {
            printf("Maximum element is: (%d)\n", *top_data);
        } else {
            printf("Could not fetch top element from pq\n");
        }

        /* 
         * Function to traverse every node and do stuff with them
         * but be aware not to modify the priority or if you modify the
         * priority be sure the mapping is injective
         * Check the doxygen documentation for the function for more information
         */
        pri_queue_traverse(pq, &print_data);
        free_priority_queue(pq);

        fclose(fin);
        fclose(fout);

        return 0;
    }
```

>**NOTE:** pri_queue_top_pri works same as pri_queue_top.

## How to find data or priority in priority queue object and to change it?

In the header file are specified 4 functions to find and change data and priority:

```C
    int change_node_priority(
        priority_queue_t* pqueue,
        size_t node_index,
        const void* new_pri,
        size_t pri_size
    );
    
    int change_node_data(
        priority_queue_t* pqueue,
        size_t node_index,
        const void* new_data,
        size_t data_size
    );

    size_t pri_find_data_index(
        priority_queue_t* pqueue,
        const void* data
    );

    size_t pri_find_pri_index(
        priority_queue_t* pqueue,
        const void* priority
    );
```

The `change_node_data` function will take a priority queue object and a index (the index can be found using pri_find_data_index), also the function will take a pointer to new data location and its size (should be the same size you can use union if want to process different data types in the same priority queue). The following function will not modify the priority queue structure so all the process is requires O(data_size, because of the memcpy which will copy data_size bytes from **new_data** pointer into the old **data** pointer. However the data is not important for priority queue structure user must not modify by him/herself the data pointer, because it may cause an error when priority queue is freed.

The `change_node_priority` does the same work as the `change_node_data`, but this function will modify the priority queue heap structure. Because the priority is changed then it will highly affect the data structure so user MUST not in any scenario modify but it's own the priority of one node (it is a bad thing not to have private member in a structure). To rearrange the node function will call static member functions as sift up or sift down according to new priority

Example of using above functions:

```C
    int main(void) {
        /* 
         * Let's supposse we have a priority queue of int allocated named pq with {5,4,3,2,1} elements
         * where the priority is the same as the data
        */

        priority_queue_t* pq = (...);

        int pri = 1;
        int new_pri = 10;

        if (change_node_priority(pq, pri_find_pri_index(pq, &pri), &new_pri, sizeof(pri))) {
            printf("Something went wrong, must call the cops\n");
        } else {
            printf("Well you now how to change data and priority now\n");
        }

        /* If changing went successfully priority queue should look like this pq = {10, 5, 3, 2, 4}*/

        free_priority_queue(pq);
        return 0;
    }
```

## Other functions that you can find some usage

```C
    size_t pri_queue_size(
        priority_queue_t* pqueue
    );

    int is_priq_empty(
        priority_queue_t* pqueue
    );

    void heap_sort(
        void* arr,
        size_t number_of_arr,
        size_t arr_elem_size,
        int (*compare_arr)(const void*, const void*)
    );
```

I will talk just about heap sort function. Even if a priority queue as heap is created in O(NlogN) time, the heap from heap_sort uses the **heapify** method that will create a heap(priority_queue) in O(N) time which is an improvement. However the cration of the heap can be as fast as possible but popping from the heap requires O(logN) time and popping N elements the resulting time for executing the function will be O(NLogN) time. However this sorting methid is very stabble and and has the same worst and best time as the average time execution complexity. The usage of the heap sort is the same as the qsort implemented in c language standard library.

## For some other examples of using queues you can look up at /examples/priority_queue
