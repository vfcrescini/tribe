/*
 * simplelist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "simplelist.h"

/* initialise list */
int simplelist_init(simplelist_type *list)
{
  if (list == NULL)
    return -1; 

  list->list = NULL;
  list->length = 0;

  return 0;
}

/* return length */
int simplelist_length(simplelist_type list, unsigned int *length)
{
  if (length == NULL)
    return -1;

  *length = list.length;

  return 0;
}

/* add pointer to list, assumes memory has been allocated to it */
int simplelist_add(simplelist_type *list, void *data)
{
  simplelist_node *new_node;

  if (list == NULL || data == NULL)
    return -1;

  if ((new_node = (simplelist_node *) malloc(sizeof(simplelist_node))) == NULL)
    return -1;

  new_node->data = data;
  new_node->next = list->list;
  list->list = new_node;
  (list->length)++;

  return 0;
}

/* deletes index'th data, give fr function to free the pointer 
 * or NULL to not free it */
int simplelist_del_index(simplelist_type *list, 
                         unsigned int index, 
                         int (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;
  unsigned int i;

  if (list == NULL || list->length <= 0 || index >= list->length)
   return -1;

  prev = NULL;
  curr = list->list;

  for (i = 0; i < list->length - index - 1; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL)
    list->list = list->list->next;
  else
    prev->next = curr->next;

  if (fr != NULL)
    fr(curr->data);
  free(curr);

  (list->length)--;

  return 0;
}

/* deletes the node that matches data, uses cmp to compare, fr to free or
 * NULL to not free it */
int simplelist_del_data(simplelist_type *list, 
                        void *data,
                        int (*cmp)(void *, void *),
                        int (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;

  if (list == NULL ||
      list->length <= 0 ||
      data == NULL ||
      cmp == NULL ||
      fr == NULL)
    return -1;

  prev = NULL;
  curr = list->list;

  while (curr != NULL) {
    if (!cmp(curr->data, data)) {
      if (prev == NULL)
        list->list = list->list->next;
      else
        prev->next = curr->next;

      if (fr != NULL)
        fr(curr->data);
      free(curr);

      (list->length)--;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }

  return -1;
}

/* gives a reference to the index'th data */
int simplelist_get_index(simplelist_type list, 
                         unsigned int index, 
                         void **ref)
{
  unsigned int i;
  simplelist_node *curr;

  if (list.length <= 0 || index >= list.length || ref == NULL)
   return -1;

  curr = list.list;

  for (i = 0; i < list.length - index - 1; i++)
    curr = curr->next;

  *ref = curr->data;

  return 0;
}

/* gives a reference to the data that matches data */
int simplelist_get_data(simplelist_type list, 
		        void *data,
                        void **ref,
                        int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (list.length <= 0 ||
      data == NULL ||
      ref == NULL ||
      cmp == NULL) 
    return -1;

  curr = list.list;

  while (curr != NULL) {
    if (!cmp(curr->data, data)) {
      *ref = curr->data;
      return 0;
    }
    curr = curr->next;
  }

  return -1;
}

/* returns 0 if data is in the list, uses cmp to compare pointers */
int simplelist_find_data(simplelist_type list, 
                         void *data, 
                         int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (list.length <= 0 ||
      data == NULL ||
      cmp == NULL) 
    return -1;

  curr = list.list;

  while (curr != NULL) {
    if (!cmp(curr->data, data)) 
      return 0;

    curr = curr->next;
  }

  return -1;
}