/*
 * simplelist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_SYM_SIMPLELIST_H
#define __EPI_SYM_SIMPLELIST_H

/* simple list works like a queue, except the api allows the data
 * to be accessed given its ordinal index (the order in which it was
 * added to the list */

typedef struct simplelist_node
{
  void *data;
  struct simplelist_node *next;
} simplelist_node;

typedef struct simplelist_type
{
  simplelist_node *list;
  unsigned int length;
} simplelist_type;

/* initialise list */
int simplelist_init(simplelist_type *list);

/* return length */
int simplelist_length(simplelist_type list, unsigned int *length);

/* add pointer to list, assumes memory has been allocated to it */
int simplelist_add(simplelist_type *list, void *data);

/* deletes index'th data, give fr function to free the pointer 
 * or NULL to not free it */
int simplelist_del_index(simplelist_type *list, 
                         unsigned int index, 
                         int (*fr)(void *));

/* deletes the node that matches data, uses cmp to compare, fr to free or
 * NULL to not free it */
int simplelist_del_data(simplelist_type *list, 
                        void *data,
                        int (*cmp)(void *, void *),
                        int (*fr)(void *));

/* gives a reference to the index'th data */
int simplelist_get_index(simplelist_type list, 
                         unsigned int index, 
                         void **ref);

/* gives a reference to the data that matches data */
int simplelist_get_data(simplelist_type list, 
		        void *data,
                        void **ref,
                        int (*cmp)(void *, void*));

/* returns 0 if data is in the list, uses cmp to compare pointers */
int simplelist_find_data(simplelist_type list, 
                         void *data, 
                         int (*cmp)(void *, void *));
#endif