#ifndef __TBE_LIST_H
#define __TBE_LIST_H

#include <tribe/tribe.h>

typedef struct tbe_list_node {
  void *data;
  struct tbe_list_node *next;
} tbe_list_node;

typedef struct {
  tbe_list_node *head;
  tbe_list_node *tail;
  unsigned int length;
} tbe_list;

/* create a new list */
int tbe_list_init(tbe_list *a_list);
/* purge list, use a_fr to free nodes */
void tbe_list_purge(tbe_list *a_list, void (*a_fr)(void *));
/* return length */
unsigned int tbe_list_length(tbe_list a_list);
/* add node to the head of the list, assumes memory has been pre-allocated */
int tbe_list_add_head(tbe_list *a_list, void *a_data);
/* add node to the tail of the list, assumes memory has been pre-allocated */
int tbe_list_add_tail(tbe_list *a_list, void *a_data);
/* deletes first node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_head(tbe_list *a_list, void (*a_fr)(void *));
/* deletes last node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_tail(tbe_list *a_list, void (*a_fr)(void *));
/* deletes index'th data, give fr to free the pointer or NULL to not free it */
int tbe_list_del_index(tbe_list *a_list,
                       unsigned int a_index,
                       void (*a_fr)(void *));
/* deletes all the nodes that matches data, uses cmp to compare, fr to free or
 * NULL to not free them */
int tbe_list_del_data(tbe_list *a_list,
                      void *a_data,
                      int (*a_cmp)(void *, void *),
                      void (*a_fr)(void *));
/* gives a reference to the FIRST node in the list */
int tbe_list_get_data_head(tbe_list a_list, void **a_ref);
/* gives a reference to the LAST node in the list */
int tbe_list_get_data_tail(tbe_list a_list, void **a_ref);
/* gives a reference to the index'th data */
int tbe_list_get_index(tbe_list a_list, unsigned int a_index, void **a_ref);
/* gives a reference to all the nodes that matches data. array + size */
int tbe_list_get_data_all(tbe_list a_list,
                          void *a_data,
                          int (*a_cmp)(void *, void*),
                          void ***a_array,
                          unsigned int *a_size);
/* gives a reference to the FIRST node that matches data */
int tbe_list_get_data_one(tbe_list a_list,
                          void *a_data,
                          int (*a_cmp)(void *, void*),
                          void **a_ref);
/* gives a reference to the FIRST node whose hash value matches */
int tbe_list_get_data_hash(tbe_list a_list,
                           unsigned int a_hval,
                           unsigned int (a_hfn)(void *),
                           void **a_ref);
/* returns 0 if data is in the list, uses cmp to compare pointers */
int tbe_list_find_data(tbe_list a_list,
                       void *a_data,
                       int (*a_cmp)(void *, void *));
/* makes an exact copy of l1 to l2, uses cpy to make a new copy */
int tbe_list_copy(tbe_list a_list1,
                  tbe_list *a_list2,
                  int (*a_cpy)(void *, void **));
#endif
