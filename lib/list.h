#ifndef __TBE_LIST_H
#define __TBE_LIST_H

#include <tribe/tribe.h>

typedef void *tbe_list;

/* create a new list */
int tbe_list_create(tbe_list *a_list);

/* destroy the list, use a_fr to free nodes */
void tbe_list_destroy(tbe_list *a_list, void (*a_fr)(void *));

/* purge the list, use a_fr to free nodes */
void tbe_list_purge(tbe_list a_list, void (*a_fr)(void *));

/* generic function to free nodes to be used for destroy, purge, etc. */
void tbe_list_free(void *a_node);

/* return length */
unsigned int tbe_list_length(tbe_list a_list);

/* add node to the head of the list, assumes memory has been pre-allocated */
int tbe_list_add_head(tbe_list a_list, void *a_data);

/* add node to the tail of the list, assumes memory has been pre-allocated */
int tbe_list_add_tail(tbe_list a_list, void *a_data);

/* deletes first node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_head(tbe_list a_list, void (*a_fr)(void *));

/* deletes last node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_tail(tbe_list a_list, void (*a_fr)(void *));

/* deletes index'th data, give fr to free the pointer or NULL to not free it */
int tbe_list_del_index(tbe_list a_list,
                       unsigned int a_index,
                       void (*a_fr)(void *));
/* deletes all the nodes that matches data, uses cmp to compare, fr to free or
 * NULL to not free them */
int tbe_list_del_data(tbe_list a_list,
                      const void *a_data,
                      int (*a_cmp)(const void *, const void *),
                      void (*a_fr)(void *));

/* gives a reference to the FIRST node data in the list */
int tbe_list_get_head(tbe_list a_list, void **a_ref);

/* gives a reference to the LAST node data in the list */
int tbe_list_get_tail(tbe_list a_list, void **a_ref);

/* gives a reference to the index'th data */
int tbe_list_get_index(tbe_list a_list, unsigned int a_index, void **a_ref);

/* gives a reference to all the nodes that matches data. array + size */
int tbe_list_get_data_all(tbe_list a_list,
                          const void *a_data,
                          int (*a_cmp)(const void *, const void*),
                          void ***a_array,
                          unsigned int *a_size);

/* gives a reference to the FIRST node that matches data */
int tbe_list_get_data_one(tbe_list a_list,
                          const void *a_data,
                          int (*a_cmp)(const void *, const void*),
                          void **a_ref);

/* gives a reference to the FIRST node whose hash value matches */
int tbe_list_get_data_hash(tbe_list a_list,
                           unsigned int a_hval,
                           unsigned int (a_hfn)(const void *),
                           void **a_ref);

/* returns 0 if data is in the list, uses cmp to compare pointers */
int tbe_list_find_data(tbe_list a_list,
                       const void *a_data,
                       int (*a_cmp)(const void *, const void *));

/* makes an exact copy of l1 to l2, uses cpy to make a new copy */
int tbe_list_copy(tbe_list a_list1,
                  tbe_list *a_list2,
                  int (*a_cpy)(const void *, void **));

/* traverses the list, calling fn on each node, stops at the end or when fn
 * returns something other that TBE_OK, in which that retval is returned. the
 * node ptr is passed as the first parameter and a_parm is passed second. */
int tbe_list_traverse(tbe_list a_list,
                      int (*a_fn)(const void *, void *),
                      void *a_parm);

#endif
