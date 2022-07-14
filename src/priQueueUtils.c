/**
 * @file priQueueUtils.c
 * @author Mihai Negru (determinant289@gmail.com)
 * @version 1.0.0
 * @date 2022-06-21
 * 
 * @copyright Copyright (C) 2022-2023 Mihai Negru <determinant289@gmail.com>
 * This file is part of C-language-Data-Structures.
 *
 * C-language-Data-Structures is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * C-language-Data-Structures is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with C-language-Data-Structures.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "./include/priQueueUtils.h"

/**
 * @brief Default capacity for a priority queue
 * and default ratio of reallocation for a priority queue
 * 
 */
#define DEFAULT_CAPACITY 10
#define DEFAULT_REALLOC_RATIO 2

/**
 * @brief Create a priority queue object
 * 
 * @param init_capacity 
 * @param compare_data 
 * @param compare_priority 
 * @param free_data 
 * @param free_priority 
 * @return priority_queue* 
 */
priority_queue* create_priority_queue(
    size_t init_capacity,
    int (*compare_data)(const void *, const void *),
    int (*compare_priority)(const void *, const void *),
    void (*free_data)(void *),
    void (*free_priority)(void *)
) {
    // Check if input data is valid
    if (compare_priority == NULL) {
        errno = EINVAL;
        perror("Compare function undefined for priority queue");
        return NULL;
    }

    // Set default capacity if necessary
    if (init_capacity == 0)
        init_capacity = DEFAULT_CAPACITY;

    // Allocate a new priority queue object on heap memory
    priority_queue *new_pri_queue = (priority_queue *)malloc(sizeof(priority_queue));

    // Check if priority queue was allocated successfully
    if (new_pri_queue) {
        // Set priority queue default functions
        new_pri_queue->compare_data = compare_data;
        new_pri_queue->compare_priority = compare_priority;
        new_pri_queue->free_data = free_data;
        new_pri_queue->free_priority = free_priority;

        // Set default size and capacity for priority queue
        new_pri_queue->capacity = init_capacity;
        new_pri_queue->size = 0;

        // Allocate memory for heap nodes
        new_pri_queue->nodes = (pri_node **)malloc(init_capacity * sizeof(pri_node *));

        // Check if heap nodes were allocated successfully
        if (new_pri_queue->nodes == NULL) {
            free(new_pri_queue);
            return NULL;
        }

        // Initialize every heap node as NULL
        for (size_t iter = 0; iter < init_capacity; ++iter)
            new_pri_queue->nodes[iter] = NULL;
    }

    // Return a new allocated priority queue or NULL
    return new_pri_queue;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param free_node 
 */
static void free_priority_queue_node(priority_queue *pqueue, pri_node **free_node) {
    // Check if node needs to be freed
    if (pqueue != NULL && free_node != NULL && (*free_node != NULL)) {
        // Free memory allocated for data content
        if (pqueue->free_data != NULL && (*free_node)->data != NULL)
            pqueue->free_data((*free_node)->data);

        // Free data pointer
        if ((*free_node)->data != NULL)
            free((*free_node)->data);

        // Set data pointer to default value
        (*free_node)->data = NULL;

        // Free memory allocated for priority content
        if (pqueue->free_priority != NULL && (*free_node)->pri != NULL)
            pqueue->free_priority((*free_node)->pri);

        // Free priority pointer
        if ((*free_node)->pri != NULL)
            free((*free_node)->pri);

        // Set priority pointer to default value
        (*free_node)->pri = NULL;

        // Free node pointer
        free((*free_node));

        // Set node pointer to default value
        (*free_node) = NULL;
    }
}

/**
 * @brief 
 * 
 * @param pqueue 
 */
void free_priority_queue(priority_queue *pqueue) {
    // Check if priority queue is valid
    if (pqueue != NULL) {
        // Check if heap nodes need to be freed
        if (pqueue->nodes != NULL) {
            // Free every heap node
            for (size_t iter = 0; iter < pqueue->size; ++iter)
                free_priority_queue_node(pqueue, &pqueue->nodes[iter]);

            // Free heap nodes pointer and set to default value
            free(pqueue->nodes);
            pqueue->nodes = NULL;  
        }

        // Free priority queue pointer and set to default value
        free(pqueue);
        pqueue = NULL;
    }
}

/**
 * @brief 
 * 
 */
#define get_node_left_child_pos(node_index) (2 * (node_index) + 1)

/**
 * @brief 
 * 
 */
#define get_node_right_child_pos(node_index) (2 * (node_index) + 2)

/**
 * @brief 
 * 
 */
#define get_node_parent_pos(node_index) (((node_index) - 1) / 2)

/**
 * @brief 
 * 
 * @param first_node 
 * @param second_node 
 */
static void swap_pri_queue_nodes(pri_node **first_node, pri_node **second_node) {
    // Check if nodes can be swapped
    if (first_node == NULL || second_node == NULL || (*first_node) == NULL || (*second_node) == NULL)
        return;

    // Swap node pointers
    pri_node *temp_node = *first_node;
    *first_node = *second_node;
    *second_node = temp_node;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param start_index 
 * @return int 
 */
static int sift_node_up(priority_queue *pqueue, size_t start_index) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->compare_priority == NULL)
        return 1;

    // Sift heap node up until it reaches its position according to its priority
    while (start_index > 0 && pqueue->compare_priority(pqueue->nodes[start_index]->pri, pqueue->nodes[get_node_parent_pos(start_index)]->pri) >= 1) {
        // Swap nodes
        swap_pri_queue_nodes(&pqueue->nodes[start_index], &pqueue->nodes[get_node_parent_pos(start_index)]);
        
        // Check new position of the current heap node
        start_index = get_node_parent_pos(start_index);
    }

    // Sifting up went successfully
    return 0;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param start_index 
 * @return int 
 */
static int sift_node_down(priority_queue *pqueue, size_t start_index) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->compare_priority == NULL)
        return 1;

    // Initialize swap index as current heap node index
    size_t swap_index = start_index;

    // Initialize index for checking as the left child index
    size_t check_index = get_node_left_child_pos(start_index);

    // Check if left child has a bigger/smaller priority than current heap node
    if (check_index < pqueue->size && pqueue->compare_priority(pqueue->nodes[check_index]->pri, pqueue->nodes[swap_index]->pri) >= 1)
        swap_index = check_index;

    // Set index for checking to right child index
    check_index = get_node_right_child_pos(start_index);

    // Check if right child has a bigge/smaller priority than current heap node or brother node
    if (check_index < pqueue->size && pqueue->compare_priority(pqueue->nodes[check_index]->pri, pqueue->nodes[swap_index]->pri) >= 1)
        swap_index = check_index;

    // If swap index is the same than start index than node was sifted down enough
    if (start_index != swap_index) {
        // Swap current heap node with swap heap node
        swap_pri_queue_nodes(&pqueue->nodes[start_index], &pqueue->nodes[swap_index]);

        // Sift down the new position of the current heap node
        return sift_node_down(pqueue, swap_index); 
    }

    // Sifting down went successfully
    return 0;
}

/**
 * @brief Create a priority queue node object
 * 
 * @param data 
 * @param priority 
 * @param data_size 
 * @param pri_size 
 * @return pri_node* 
 */
static pri_node* create_priority_queue_node(const void *data, const void *priority, size_t data_size, size_t pri_size) {
    // Check if input data is valid
    if (priority == NULL || pri_size == 0)
        return NULL;

    // Allocate a new priority queue node on heap memory
    pri_node *new_pri_queue_node = (pri_node *)malloc(sizeof(pri_node));

    // Check if new priority queue node was allocated successfully
    if (new_pri_queue_node) {
        // Check if new node will have a valid data pointer
        if (data != NULL && data_size != 0) {
            // Allcoate memory for data content
            new_pri_queue_node->data = malloc(data_size);

            // Check if data content was allocated
            if (new_pri_queue_node->data == NULL) {
                free(new_pri_queue_node);
            
                errno = ENOMEM;
                perror("Not enough memory for data allocation");
            
                return NULL;
            }

            // Copy all bytes from data to data node pointer
            memcpy(new_pri_queue_node->data, data, data_size);
        } else {
            // New node does not have data so set it to default value
            new_pri_queue_node->data = NULL;
        }

        // Allocate memory for priority content
        new_pri_queue_node->pri = malloc(pri_size);

        // Check if priority content was allocated
        if (new_pri_queue_node->pri == NULL) {
            if (new_pri_queue_node->data != NULL)
                free(new_pri_queue_node->data);

            free(new_pri_queue_node);
            
            errno = ENOMEM;
            perror("Not enough memory for priority allocation");
            
            return NULL;
        }

        // Copy all bytes from priority to priority node pointer
        memcpy(new_pri_queue_node->pri, priority, pri_size);
    } else {
        errno = ENOMEM;
        perror("Not enough memory for priority queue node allocation");
    }

    // Return a new allocated priority queue node or NULL
    return new_pri_queue_node;
}

/**
 * @brief 
 * 
 * @param data 
 * @param priority 
 * @param data_size 
 * @param pri_size 
 * @param number_of_data 
 * @param compare_data 
 * @param compare_priority 
 * @param free_data 
 * @param free_priority 
 * @return priority_queue* 
 */
priority_queue* heapify(
    const void *data,
    const void *priority,
    size_t data_size,
    size_t pri_size,
    size_t number_of_data,
    int (*compare_data)(const void *, const void *),
    int (*compare_priority)(const void *, const void *),
    void (*free_data)(void *),
    void (*free_priority)(void *)
) {
    // Check if input data is valid
    if (priority == NULL || pri_size == 0 || number_of_data == 0 || compare_priority == NULL)
        return NULL;

    // Create a new priority queue
    priority_queue *new_pqueue = create_priority_queue(number_of_data, compare_data, compare_priority, free_data, free_priority);

    // Check if priority queue was created successfully
    if (new_pqueue) {
        // Fill up heap nodes pointers with real content without following heap rules
        for (size_t iter = 0; iter < new_pqueue->capacity; ++iter) {
            // Initialize a new priority queue node as default values
            pri_node *new_pqueue_node = NULL;

            // Create a new priority queue node depending by data pointer
            if (data != NULL && data_size != 0) {
                new_pqueue_node = create_priority_queue_node(data + iter * data_size, priority + iter * pri_size, data_size, pri_size);
            } else {
                new_pqueue_node = create_priority_queue_node(NULL, priority + iter * pri_size, 0, pri_size);
            }

            // Check if new priority queue node was created successfully
            if (new_pqueue_node == NULL)
                return NULL;
            
            // Link new created priority queue node to heap nodes
            new_pqueue->nodes[iter] = new_pqueue_node;

            // Increase priority queue's size
            ++(new_pqueue->size);
        }

        // Sift down every node that is not a leaf in binary-heap
        for (int iter = (int)((new_pqueue->size / 2) - 1); iter >= 0; --iter)
            sift_node_down(new_pqueue, (size_t)iter);
    }

    // Return a new allocated priority queue or NULL
    return new_pqueue;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param node_index 
 * @param new_pri 
 * @param pri_size 
 * @return int 
 */
int change_node_priority(priority_queue *pqueue, size_t node_index, const void *new_pri, size_t pri_size) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->nodes[node_index] == NULL || new_pri == NULL || pri_size == 0)
        return 1;

    // Check if selected node index is a valid index
    if (node_index == __SIZE_MAX__ || node_index >= pqueue->size)
        return 1;

    // Check if selected priority queue is a valid queue
    if (pqueue->nodes[node_index]->pri == NULL || pqueue->compare_priority == NULL)
        return 1;

    // Sift selected node down if new priority has a smaller rank 
    // than old priority according to compare function
    if (pqueue->compare_priority(pqueue->nodes[node_index]->pri, new_pri) >= 1) {
        // Copy new priority into old priority
        memmove(pqueue->nodes[node_index]->pri, new_pri, pri_size);

        // Sift node down
        return sift_node_down(pqueue, node_index);
    }

    // Sift selected node up if new priority has a bigger rank 
    // than old priority according to compare function
    if (pqueue->compare_priority(pqueue->nodes[node_index]->pri, new_pri) <= -1) {
        // copy new priority into old priority
        memmove(pqueue->nodes[node_index]->pri, new_pri, pri_size);

        // Sift node up
        return sift_node_up(pqueue, node_index);
    }

    // Old priority is equal to new priority
    return 0;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param data 
 * @return size_t 
 */
size_t pri_find_data_index(priority_queue *pqueue, const void *data) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->size == 0 || pqueue->nodes == NULL || data == NULL || pqueue->compare_data == NULL)
        return __SIZE_MAX__;

    // Find desired data index according to compare function
    for (size_t iter = 0; iter < pqueue->size; ++iter)
        if (pqueue->nodes[iter] != NULL)
            if (pqueue->compare_data(pqueue->nodes[iter]->data, data) == 0)
                return iter;

    // No data was found according to compare data function
    return __SIZE_MAX__;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param priority 
 * @return size_t 
 */
size_t pri_find_pri_index(priority_queue *pqueue, const void *priority) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->size == 0 || pqueue->nodes == NULL || priority == NULL || pqueue->compare_priority == NULL)
        return __SIZE_MAX__;

    // Find desired priority index according to compare function
    for (size_t iter = 0; iter < pqueue->size; ++iter)
        if (pqueue->nodes[iter] != NULL)
            if (pqueue->compare_priority(pqueue->nodes[iter]->pri, priority) == 0)
                return iter;

    // No priority was found according to compare priority function
    return __SIZE_MAX__;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param data 
 * @param priority 
 * @param data_size 
 * @param pri_size 
 * @return int 
 */
int pri_queue_push(priority_queue *pqueue, const void *data, const void *priority, size_t data_size, size_t pri_size) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->capacity == 0 || priority == NULL || pri_size == 0)
        return 1;

    // Check if priority queue is full
    // If true allocate more heap nodes
    if (pqueue->size >= pqueue->capacity) {
        // Increase capacity by default reallocation ratio
        pqueue->capacity *= DEFAULT_REALLOC_RATIO;
        
        // Try to realloc heap nodes
        pri_node **try_realloc = (pri_node **)realloc(pqueue->nodes, pqueue->capacity * sizeof(pri_node *));

        // If reallocation went wrong exit pushing function
        // and throw an error
        if (try_realloc == NULL) {
            pqueue->capacity /= DEFAULT_REALLOC_RATIO;

            errno = ENOMEM;
            perror("Not enough memory to reallocate new nodes");

            return 1;
        }

        // Set heap nodes pointer to the new memory location
        pqueue->nodes = try_realloc;
    }

    // Create a new priority queue node
    pri_node *add_node = create_priority_queue_node(data, priority, data_size, pri_size);

    // If new node was not created exit pushing function
    if (add_node == NULL)
        return 1;

    // Link new created node to heap nodes
    pqueue->nodes[pqueue->size] = add_node;

    // Increase priority queue size
    ++(pqueue->size);

    // Sift node up to preserve heap rules
    return sift_node_up(pqueue, (pqueue->size - 1));
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @return const void* 
 */
const void* pri_queue_top(priority_queue *pqueue) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->nodes[0] == NULL)
        return NULL;

    // Return the peek data pointer
    return pqueue->nodes[0]->data;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @return const void* 
 */
const void* pri_queue_top_pri(priority_queue *pqueue) {
    // Check if input data is valid
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->nodes[0] == NULL)
        return NULL;

    // Return the highest ranking priority
    return pqueue->nodes[0]->pri;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @return int 
 */
int pri_queue_pop(priority_queue *pqueue) {
    // Check if input data is valid and if there are nodes to pop from priority queue
    if (pqueue == NULL || pqueue->nodes == NULL || pqueue->capacity == 0 || pqueue->size == 0)
        return 1;

    // Swap first heap node with last heap node
    swap_pri_queue_nodes(&pqueue->nodes[0], &pqueue->nodes[pqueue->size - 1]);

    // Free memory allocated for last heap node
    free_priority_queue_node(pqueue, &pqueue->nodes[pqueue->size - 1]);

    // Decrease priority queue size
    --(pqueue->size);

    // Sift first heap node down to reestablish heap propreties
    return sift_node_down(pqueue, 0);
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @return size_t 
 */
size_t pri_queue_size(priority_queue *pqueue) {
    // Check if priority queue is valid
    if (pqueue == NULL)
        return __SIZE_MAX__;

    // Return priority queue size
    return pqueue->size;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @return int 
 */
int is_priq_empty(priority_queue *pqueue) {
    // Check fi priority queue is valid and if it is empty
    if (pqueue == NULL || pqueue->capacity == 0 || pqueue->nodes == NULL || pqueue->size == 0)
        return 1;

    // Priority queue is not empty
    return 0;
}

/**
 * @brief 
 * 
 * @param pqueue 
 * @param action 
 */
void pri_queue_traverse(priority_queue *pqueue, void (*action)(const pri_node *)) {
    // Check if input data value is valid
    if (pqueue == NULL || pqueue->nodes == NULL || action == NULL)
        return;

    // Traverse every single node and make an action to node
    // according with input function
    for (size_t iter = 0; iter < pqueue->size; ++iter)
        action(pqueue->nodes[iter]);
}

/**
 * @brief 
 * 
 * @param arr 
 * @param number_of_arr 
 * @param arr_elem_size 
 * @param compare_arr 
 */
void heap_sort(void *arr, size_t number_of_arr, size_t arr_elem_size, int (*compare_arr)(const void *, const void *)) {
    // Check if input data is valid
    if (arr == NULL || number_of_arr == 0 || arr_elem_size == 0 || compare_arr == NULL)
        return;

    // Heapify the input array in O(N) complexity
    priority_queue *heap = heapify(NULL, arr, 0, arr_elem_size, number_of_arr, NULL, compare_arr, NULL, NULL);

    // Rearrange input array data from heap structure in O(NlogN) complexity  
    for (size_t iter = 0; iter < number_of_arr; ++iter) {
        memcpy(arr + iter * arr_elem_size, pri_queue_top_pri(heap), arr_elem_size);
        pri_queue_pop(heap);
    }

    // Free heap memory in O(N) complexity
    free_priority_queue(heap);

    // Overall complexity O(NlogN)
}