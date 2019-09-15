/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
* - Operating Systems Principles, Semester 2, 2019. 
******************************************************************************/

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef LIST_H
#define LIST_H

struct node
{
    struct node* next;
    struct node* prev;
    void* data;
};
struct list
{
    struct node* head;
    struct node* tail;
    long size;
    int (*cmp)(const void*, const void*);
    void (*data_free)(void*);
    bool newed;
};

#define LL_BEGIN(LIST) LIST->head
#define LL_END(LIST) NULL
#define LL_NEXT(NODE) NODE = NODE->next
#define LL_PREV(NODE) NODE = NODE->prev

void list_init(struct list*, int (*)(const void*, const void*),
               void (*)(void*));

struct list* list_make(int (*)(const void*, const void*), void (*)(void*));

bool list_push_front(struct list*, void*);

bool list_push_back(struct list*, void*);

bool list_push_sorted(struct list*, void*);

bool list_push_sorted_cmp(struct list*, void*, int (*)(void*, void*));

void* list_pop_front(struct list*);

void* list_pop_back(struct list*);

void* list_pop_find(struct list* thelist, const void* data);

void* list_pop_find_cmp(struct list*, const void*,
                        int (*)(const void*, const void*));

void list_free(struct list*);

void list_print(struct list*, int (*)(const void*));

void* list_pop_min_cmp(struct list*, int (*)(const void*, const void*));

void* list_pop_max_cmp(struct list*, int (*)(const void*, const void*));

void* list_pop_min(struct list*);

void* list_pop_max(struct list*);

void list_for_each(struct list*, const void*, void**,
                   void (*)(void*, void**, const void*));
bool list_remove(struct list*, const void*, int cmp(const void*, const void*));
#endif
