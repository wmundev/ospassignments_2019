/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114
 * - Operating Systems Principles, Semester 2, 2019.
 ******************************************************************************/

#include "list.h"

/**
 * initialise the list to be an empty list
 **/
void
list_init(struct list* thelist, int (*cmp)(const void*, const void*),
          void (*data_free)(void*))
{
    /* all parameters passed in must not be NULL */
    assert(thelist != NULL);
    assert(cmp != NULL);
    assert(data_free != NULL);

    /* zero out the whole structure */
    memset(thelist, 0, sizeof(struct list));
    /* set the function pointers*/
    if (cmp)
    {
        thelist->cmp = cmp;
    }
    if (data_free)
    {
        thelist->data_free = data_free;
    }
}

/**
 * allocate memory for the list and initialise it
 **/
struct list*
list_make(int (*cmp)(const void*, const void*), void (*data_free)(void*))
{
    struct list* thelist;
    /* allocate space for the list */
    thelist = (struct list*)malloc(sizeof(struct list));
    if (!thelist)
    {
        perror("alloc");
        return NULL;
    }
    /* init the list */
    list_init(thelist, cmp, data_free);
    thelist->newed = true;
    return thelist;
}

/**
 * allocate space for the node in a linked list - that way we only write
 * this code once
 **/
static struct node*
new_node(void* data)
{
    /* allocate space fore the new node */
    struct node* new = (struct node*)malloc(sizeof(struct node));
    if (!new)
    {
        perror("alloc");
        return NULL;
    }
    /* set the data for the node */
    new->data = data;
    /* ensure back and forward point to NULL to have less work to do later
     */
    new->prev = new->next = NULL;
    return new;
}

/* free the node pointer such as when popping data */
static void
node_free(struct node* thenode)
{
    /* free(thenode);*/
}

/**
 * prepend an element to the list
 **/
bool
list_push_front(struct list* thelist, void* data)
{
    /* allocate space for the data */
    struct node* new = new_node(data);
    if (!new)
    {
        return false;
    }
    new->next = thelist->head;
    new->prev = NULL;
    /* prepend to the rest of the list */
    if (thelist->head)
    {
        thelist->head->prev = new;
    }
    else
    {
        /* deal with an empty list */
        assert(!thelist->tail);
        thelist->tail = new;
    }
    thelist->head = new;
    thelist->size++;
    return true;
}

/**
 * add to the end of the list
 **/
bool
list_push_back(struct list* thelist, void* data)
{
    struct node* newnode;
    /* ensure none of the parameters are NULL */
    assert(thelist != NULL);
    assert(data != NULL);

    /* allocate space for the node */
    newnode = new_node(data);
    if (!newnode)
    {
        return false;
    }

    /* if the list is not empty */
    if (thelist->tail)
    {
        /* just append to the end */
        newnode->prev = thelist->tail;
        thelist->tail->next = newnode;
        thelist->tail = newnode;
    }
    else
    {
        /* we have an empty list */
        assert(!thelist->head);
        /* both head and tail should point to the same node */
        thelist->head = thelist->tail = newnode;
    }
    thelist->size++;
    return true;
}

/**
 * insert the data in sorted order by the default sort mechanism
 **/
bool
list_push_sorted(struct list* thelist, void* data)
{
    struct node* new;
    assert(thelist);
    assert(data);

    /* allocate the new node */
    new = new_node(data);
    if (!new)
    {
        return false;
    }

    /* insert at the head */
    if (!thelist->head)
    {
        thelist->head = new;
    }
    else
    {
        struct node* current;
        current = thelist->head;
        /* search for the insertion point in the list */
        while (current && thelist->cmp(current->data, data) < 0)
        {
            current = current->next;
        }
        /* insert elsewhere than the the tail */
        if (current)
        {
            new->next = thelist->head;
            thelist->head->prev = new;
            thelist->head = new;
        }
    }
    thelist->size++;
    return true;
}

/**
 * inserts an element into the list sorted based on the comparison function
 * provided
 **/
bool
list_push_sorted_cmp(struct list* thelist, void* data, int (*cmp)(void*, void*))
{
    struct node* new;
    assert(thelist);
    assert(data);

    /* allocate the new node */
    new = new_node(data);
    if (!new)
    {
        return false;
    }

    /* insert at the head */
    if (!thelist->head)
    {
        thelist->head = new;
    }
    else
    {
        struct node* current;
        current = thelist->head;
        /* search for the insertion point in the list */
        while (current && cmp(current->data, data) < 0)
        {
            current = current->next;
        }
        /* insert elsewhere than the the tail */
        if (current)
        {
            new->next = thelist->head;
            thelist->head->prev = new;
            thelist->head = new;
        }
    }
    thelist->size++;
    return true;
}

/**
 * remove an element from the front of the list
 **/
void*
list_pop_front(struct list* thelist)
{
    struct node* container = thelist->head;
    void* data;
    /* if there is actually an element at the front of the list */
    if (container)
    {
        /* move the head forward by one element */
        thelist->head = container->next;
        /* if the list is now empty, ensure that list and tail are consistent */
        if (thelist->head)
        {
            thelist->head->prev = NULL;
        }
        else
        {
            thelist->tail = NULL;
        }
    }
    else
    {
        /* the list was empty */
        return NULL;
    }
    /* free the container and reduce the size by one */
    data = container->data;
    node_free(container);
    thelist->size--;
    return data;
}

/**
 * remove an element from the end of the list
 **/
void*
list_pop_back(struct list* thelist)
{
    /**
     * remove the last element from the list
     **/
    struct node* container = thelist->tail;
    void* data;
    /* if there is a tail, remove the links forwards and backwards */
    if (container)
    {
        thelist->tail = container->prev;
        if (thelist->tail)
        {
            thelist->tail->next = NULL;
        }
        else
        {
            thelist->head = NULL;
        }
    }
    else
    {
        /* the list was empty */
        return NULL;
    }
    /* free the data and reduce the count by one */
    data = container->data;
    node_free(container);
    thelist->size--;
    return data;
}

/**
 * finds the node that matches the data passed in based on the default
 * comparator provided when the list was initialised
 **/
void*
list_pop_find(struct list* thelist, const void* data)
{
    struct node* current;
    struct node* found = NULL;
    void* found_data;

    /**
     * search for the required node
     **/
    current = thelist->head;
    while (current && current->data && thelist->cmp(current->data, data) != 0)
    {
        current = current->next;
    }
    /* if we didn't reach the end of the list and the current node matches */
    if (current && thelist->cmp(current->data, data) == 0)
    {
        found = current;
    }
    else
    {
        /* we didn't find a matching node */
        return NULL;
    }

    /* free the node and decrement the size of the list */
    found_data = found->data;
    node_free(found);
    --thelist->size;
    return found_data;
}

/**
 * find the node that matches our requirement based on the comparison function
 *passed in
 **/
void*
list_pop_find_cmp(struct list* thelist, const void* data,
                  int (*cmp)(const void*, const void*))
{
    struct node* current;
    struct node* found = NULL;
    void* found_data;

    /**
     * the list is empty so there's no point in doing searching
     **/
    if (thelist->size == 0)
    {
        return NULL;
    }
    /**
     * search for the required item
     **/
    current = thelist->head;
    while (current && current->data && cmp(current->data, data) != 0)
    {
        current = current->next;
    }
    /* have we found the item ? */
    if (current && cmp(current->data, data) == 0)
    {
        found = current;
    }
    else
    {
        /* we did not find the required item */
        return NULL;
    }

    found_data = found->data;
    /* remove the element from the lsit */
    if (found->prev)
    {
        found->prev->next = found->next;
    }
    else
    {
        thelist->head = thelist->head->next;
        if (thelist->head)
        {
            thelist->head->prev = NULL;
        }
    }
    if (found->next)
    {
        found->next->prev = found->prev;
    }
    else
    {
        thelist->tail = thelist->tail->prev;
        if (thelist->tail)
        {
            thelist->tail->next = NULL;
        }
    }

    /* free the node and decrement the size of the list */
    node_free(found);
    --thelist->size;
    return found_data;
}

/**
 * frees the list
 **/
void
list_free(struct list* thelist)
{
    struct node* current;
    /* iterate over the list */
    current = thelist->head;
    while (current)
    {
        /* grab the next pointer */
        struct node* next = current->next;
        /* free the data and the node */
        thelist->data_free(current->data);
        node_free(current);
        /* move on to the next node */
        current = next;
    }
    if (thelist->newed)
    {
        /* free the list header structure if it was dynamically allocated */
        free(thelist);
    }
}

/**
 * iterate over the list and print each element using the print_func argument
 **/
void
list_print(struct list* thelist, int (*print_func)(const void*))
{
    struct node* curr;
    for (curr = LL_BEGIN(thelist); curr != LL_END(thelist); LL_NEXT(curr))
    {
        print_func(curr->data);
    }
}

/**
 * remove the smallest element from the list as decided
 **/
void*
list_pop_min_cmp(struct list* thelist, int (*cmp)(const void*, const void*))
{
    struct node* min_node = NULL;
    struct node* curr;
    void* result = NULL;

    /* if the list is empty, nothing to search */
    if (thelist->size == 0)
    {
        return NULL;
    }
    /**
     * iterate over the list and capture the minimum node in the list
     **/
    for (curr = LL_BEGIN(thelist); curr != LL_END(thelist); LL_NEXT(curr))
    {
        if (min_node == NULL || cmp(curr->data, min_node->data) < 0)
        {
            min_node = curr;
        }
    }
    /* remove the node from the list */
    if (min_node->prev)
    {
        min_node->prev->next = min_node->next;
    }
    else
    {
        thelist->head = thelist->head->next;
    }
    if (min_node->next)
    {
        min_node->next->prev = min_node->prev;
    }
    else
    {
        thelist->tail = thelist->tail->prev;
    }
    /* free the node and reduce the list size */
    result = min_node->data;
    free(min_node);
    --thelist->size;
    return result;
}

/**
 * remove the largest element from the list
 **/
void*
list_pop_max_cmp(struct list* thelist, int (*cmp)(const void*, const void*))
{
    struct node* curr;
    struct node* max_node = NULL;
    void* result = NULL;

    /* the list is empty so no point in searching */
    if (!thelist->head)
    {
        return NULL;
    }

    /**
     * find the minimum element from the lsit
     **/
    for (curr = LL_BEGIN(thelist); curr != LL_END(thelist); LL_NEXT(curr))
    {
        if (max_node == NULL || cmp(curr->data, max_node->data) > 0)
        {
            max_node = curr;
        }
    }
    /**
     * remove the maximum element from the list
     **/
    if (max_node->prev)
    {
        max_node->prev->next = max_node->next;
    }
    else
    {
        thelist->head = thelist->head->next;
    }
    if (max_node->next)
    {
        max_node->next->prev = max_node->prev;
    }
    else
    {
        thelist->tail = thelist->tail->prev;
    }
    /** free the node and decrement the list size **/
    --thelist->size;
    result = max_node->data;
    free(max_node);
    return result;
}
/**
 * find the smallest element based on the default comparator provided when
 * initialising the list
 **/
void*
list_pop_min(struct list* thelist)
{
    return list_pop_min_cmp(thelist, thelist->cmp);
}

/**
 * find the largest element of the list based on the default comparator
 * provided when initialising the list
 **/
void*
list_pop_max(struct list* thelist)
{
    return list_pop_max_cmp(thelist, thelist->cmp);
}

/**
 * perform some operation on the linked list, such as a global search
 **/
void
list_for_each(struct list* thelist, const void* desired, void** best,
              void (*perform)(void*, void**, const void*))
{
    struct node* curr;

    for (curr = LL_BEGIN(thelist); curr != LL_END(thelist); LL_NEXT(curr))
    {
        perform(curr->data, best, desired);
    }
}

/**
 * remove the item specified from the list based on the comparator passed in
 **/
bool
list_remove(struct list* thelist, const void* item,
            int cmp(const void*, const void*))
{
    struct node* curr;
    /* find the node that contains the item */
    for (curr = LL_BEGIN(thelist); curr != LL_END(thelist); LL_NEXT(curr))
    {
        if (cmp(curr->data, item) == 0)
        {
            break;
        }
    }
    /* could not find the item */
    if (!curr)
    {
        return false;
    }
    /* remove the node from the list */
    if (curr->prev)
    {
        curr->prev->next = curr->next;
    }
    else
    {
        thelist->head = thelist->head->next;
        if (thelist->head)
        {
            thelist->head->prev = NULL;
        }
    }
    if (curr->next)
    {
        curr->next->prev = curr->prev;
    }
    else
    {
        thelist->tail = thelist->tail->prev;
        if (thelist->tail)
        {
            thelist->tail->next = NULL;
        }
    }
    /* free the node and decrement the size of the list */
    free(curr);
    thelist->size--;
    return true;
}
