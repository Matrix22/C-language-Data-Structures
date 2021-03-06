# Documentation for double linked list object (scl_dlist.h)

## How to create a double linked list and how to destroy it?

In the scl_dlist.h file you have two functions that helps you to create and delete a linked list.

```C
    dlist_t* create_dlist(
        int (*compare_data)(const void*, const void*),
        void (*print_data)(const void*),
        void (*free_data)(void *))
```

create_dlist function will allocate a list on heap and will return a pointer to its location. However you should provide 3 function that are neccessary for maintaing the linked list.

```C
    int compare_data(const void *elem1, const void *elem2)
```

Function will take 2 generic pointers and according to user needs will return:

    1. 1 - if elem1 > elem2
    2. -1 - if elem1 < elem2
    3.  0 - if elem1 == elem2

Example of compare_data function for **int** data type:

```C
    int compare_data(const void *elem1, const void *elem2) {
        const int *fa = elem1;
        const int *fb = elem2;

        return *fa - *fb;
    }
```

print_data function will display output on **stdout**, hovewer you cannot print a data by using a filestream. The single solution is to declare a globale scope **FILE** variable and use *fprintf*s or *fwrite* instead of *printf*. Also you can use freopen function to redirect stdout stream to another stream.

```C
    FILE *fin = NULL; // Allocate in main function
    void print_data(const void *elem) {
        fprintf(fin, "%d ", *(const int *)elem);
        // Instead of printf("%d", *(int *)elem);
        // For int data type
    }
```

free_data function is optional if you do not use basic data types such as int, float, double, or static arrays you should provide a function to remove a parameter. Evrery thing that you allocate on heap by yourslef must be specified in **free_data** function.
If no free is needed that NULL should be passed in `create_dlist` function.

Exaple of type:
```C
    typedef struct {
        char *name;
        int *marks;
        int GPA;
        char firstLetter;
        double average;
    } student;
```

An example of `free_data` function for this structure should be:

```C
    void free_data(void *elem1) {
        student *fa = elem1;

        if (fa->name)
            free(fa->name);

        if (fa->marks)
            free(fa->marks);
    }
```

> **NOTE:** If any proccess of `create_dlist` fails than an exception will be thrown.

**If you want to print the entire list** than you should call `print_dlist_front/back` function.

> **NOTE:** If list is not allocated than nothing will be printed. If list is empty that a set of square paired brackets will be printed **[ ]** and if list is not empty than every element from the list will be printed according to **print_data** function provided by the user.

The last function is for freeing memory from heap allocated for the list.

**`free_dlist`** will take a list as input and will free every element according to **free_data** function provided by user. If free_data function is NULL than content of one data will be not removed (because is allocated on stack not on the heap -- No memory leaks will be generated). Evry pointer that is freed is also set to NULL pointer.

Example of creating a double linked list containing int data types:
```C
    dlist_t *list = create_dlist(&compare_int, &print_int, NULL);
    print_dlist_front(list);
    free_dlist(list);
```

## How to insert and how to remove elements from double linked list ?

In this section we will cover 7 functions dedicated for insertion and deletion from double linked list object.

Insertion in double linked list works just like insertion in **vectors** from C++.

There are 4 types of insertion:

* dlist_insert - inserts an element to the end of the list
* dlist_insert_front - inserts an element to the beginnign of the list
* dlist_insert_order - insers an element in order in list according to compare_data function rule
* dlist_insert_index - inserts an element to an index in the list

Let's assume that we works with integers and we would like to mantain a linked list of integers
insertion would show like:

```C
    dlist_t *list = create_dlist(...);
    int data = 0; // Dummy value not necessary

    for (int i = 0; i < 10; ++i) {
        scanf("%d", &data); // You could replace with fscanf
        dlist_insert(list, &data, sizeof(data));
    }
```

If you have dynamic elements in a structure and want to store the strucutre in the list you shouls do as follows:

```C
    typedef struct {
        char *name;
        int age;
    }   person;

    ....

    dlist_t *list = create_dlist(...);
    person data;

    for (int i = 0; i < 10; ++i) {
        data.name = malloc(SET_A_WORD_SIZE);
        scanf("%d", &data.age);
        scanf("%s", data.name);
        dlist_insert(list, &data, sizeof(data));
    }

    // It is very important not to free manually data.name
    // This will be done in freeing the entire list

    free_dlist(list);
    // Will free every allocation made for list including data.name
```

> **NOTE:** In this case if no free_data function is provided the program will result in memory leaks. If we change from `char *name`, into `char name[SIZE_OF_A_WORD]` then no free_data should be provided.

> **NOTE:** dlist_insert_front, dlist_insert_order and dlist_insert_index works just like dlist_insert, but have different proprieties.

> **NOTE:** in dlist_insert_index if provided index is greter or equal to list size than element will be inserted at the end of the list.

**If you want** to delete some elements from a list you have 3 choices:

* Delete element by data - provide a data for function to find and delete (dlist_delete_data)
* Delete element by index - provide an index for function to find and delete (dlist_delete_index)
* Delete a range of elements - provide a left and a right index for function to erase (dlist_erase)

Let's take the above example of insertion with integers and let remove some elements:

```C
    // By data
    data = 4;
    if (dlist_delete_data(list, &data))
        printf("Failed to remove %d\n", data);

    // By index
    if (dlist_delete_index(list, 4))
        printf("Failed to remove index 4\n");

    // By range
    if (dlist_erase(list, 1, 3))
        printf("Failed to remove range [1,3]\n");
```

These 3 functions can return either 0 if deletion was successfully or 1 if something went wrong.
Some cases where functions may fail are:

1. pointer to data is NULL or data value is not in the double linked list
2. provided index is to large and gets out from list range
3. list is empty and no element can be removed

> **NOTE:** It is a good practise to embrace a if around deletion elements but you can do same pattern when inserting an element in the list.

> **NOTE:** for dlist_erase, you can provide different left and right index even in a random form. For example if you will pass left and right index out of bound then the last element will be removed, if left index is greater than right one, they both will be swapped.

## How to access items within the list ?

I also provided two functions that can find and return pointers to nodes from list

* dlist_find_data - returns a pointer to first occurence of node that contains **data** value
* dlist_find_index - returns a pointer to node positioned at provided index in the list

> **NOTE:** If no node is found than NULL pointer will be returned

Example of using list finds:

```C
    // list = 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> NULL
    int data = 3;

    // By data
    dlist_node_t* node1 = dlist_find_data(list, &data);

    // By index
    dlist_node_t* node2 = dlist_find_index(list, 4);

    // Now you cand do stuff with nodes for example swap them 
```

## Can I modify value of a node data, can I modify type of a node data ?

You easily can modify a value of a node data, but you **CANNOT** change it's data type.

If you try to change data between typed that have different size you can encounter a memory leak even a segmentation fault. If types have the same size that program may result into an **unknown behavior**.

Two functions that change data of nodes are:

* dlist_change_data - changes data of a node with another value
* dlist_swap_data - interchanges data from two valid nodes

Example of changing and swapping two data:

```C
    // list = 1 -> 2 -> 3 -> 4 -> 5
    dlist_node_t* node1 = dlist_find_index(list, 0);
    dlist_node_t* node2 = dlist_find_index(list, list->size - 1);

    dlist_swap_data(list, node1, node2);
    
    data = 20;
    dlist_change_data(list, node1, &data, sizeof(data));

    // list = 20 -> 2 -> 3 -> 4 -> 5
```

> **NOTE:** A very important asspect is than dlist_swap_data will swap data pointers not the entire node pointer so node1 will point for the rest of his life to the beginnign of the list and node2 to the end of the list. dlist_change_data will not allocate a new node and delete the current one but will copy bytes from data pointer provided as input and will move them into the current data pointer location.

## Some special functions

Two important functions provided in **scl_dlist.h** are:

* Filter function - creates a new list and selects some elements based of filter rule
* Map function - modifies in-place the current list by mapping evry element into a new element of the same type

> **NOTE:** Filter function must return 1 for true and 0 for false. When true, element is added in the new list.

Examples of list filter and mapping

```C
    int filter(const void *elem) {
        const int *fa = elem;

        if (1 == *fa) return 1;

        return 0;
    }

    const void* map(void *elem) {
        int *fa = elem;

        *fa = (*fa) % 2;

        return fa;
    }

    ...

    // list = 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10

    dlist_map(list, &map, sizeof(int));

    // list = 1 -> 0 -> 1 -> 0 -> 1 -> 0 -> 1 -> 0 -> 1 -> 0

    dlist_t *new_list = dlist_filter(list, &filter, sizeof(int));

    // newList = 1 -> 1 -> 1 -> 1 -> 1 -> NULL

    printf("We have %d odd numbers\n", new_list->size);

    // Do not forget to free space
    free_dlist(new_list);
    free_dlist(list);
```

> **NOTE:** If filter function return **0** for every element then NULL pointer will be returned and no newList will be created, however you can pass a NULL double linked list pointer to **free_dlist**, but it will have no effect.

## For some other examples of using double linked lists you can look up at /examples/dlist

