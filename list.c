#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/* create a new list */
int tbe_list_init(tbe_list *a_list)
{
  if (!a_list)
    return TBE_NULLPTR;

  a_list->head = NULL;
  a_list->tail = NULL;
  a_list->length = 0;

  return TBE_OK;
}

/* purge list, use a_fr to free nodes */
void tbe_list_purge(tbe_list *a_list, void (*a_fr)(void *))
{
  tbe_list_node *curr;
  tbe_list_node *prev;

  if (a_list) {
    curr = a_list->head;

    while (curr) {
      prev = curr;
      curr = curr->next;

      /* if a_fr was provided, we must use it upon the data field  */
      if (a_fr != NULL)
        a_fr(prev->data);

      TBE_PTR_FREE(prev);
    }

    a_list->head = NULL;
    a_list->tail = NULL;
    a_list->length = 0;
  }
}

/* return length */
unsigned int tbe_list_length(tbe_list a_list)
{
  return a_list.length;
}

/* add pointer to list, assumes memory has been pre-allocated */
int tbe_list_add(tbe_list *a_list, void *a_data)
{
  tbe_list_node *node;

  if (!a_list || !a_data)
    return TBE_NULLPTR;

  if (!(node = TBE_PTR_MALLOC(tbe_list_node, 1)))
    return TBE_MALLOCFAILED;

  if (a_list->tail)
    a_list->tail->next = node;
  else
    a_list->head = node;

  node->data = a_data;
  node->next = NULL;
  a_list->tail = node;
  (a_list->length)++;

  return TBE_OK;
}

/* deletes index'th data, give fr to free the pointer or NULL to not free it */
int tbe_list_del_index(tbe_list *a_list,
                       unsigned int a_index,
                       void (*a_fr)(void *))
{
  tbe_list_node *prev;
  tbe_list_node *curr;
  unsigned int i;

  if (!a_list)
    return TBE_NULLPTR;

  if (a_list->length <= 0 || a_index >= a_list->length)
    return TBE_OUTOFBOUNDS;

  prev = NULL;
  curr = a_list->head;

  for (i = 0; i < a_index; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (!prev) {
    if (curr == a_list->tail)
      a_list->tail = NULL;
    a_list->head = a_list->head->next;
  }
  else {
    if (curr == a_list->tail)
      a_list->tail = prev;
    prev->next = curr->next;
  }

  if (a_fr)
    a_fr(curr->data);
  TBE_PTR_FREE(curr);

  (a_list->length)--;

  return TBE_OK;
}

/* deletes all the nodes that matches data, uses cmp to compare, fr to free or
 * NULL to not free them */
int tbe_list_del_data(tbe_list *a_list,
                      void *a_data,
                      int (*a_cmp)(void *, void *),
                      void (*a_fr)(void *))
{
  tbe_list_node *prev;
  tbe_list_node *curr;
  char found = 0;

  if (!a_list || !a_data || !a_cmp)
    return TBE_NULLPTR;

  prev = NULL;
  curr = a_list->head;

  while (curr) {
    if (a_cmp(curr->data, a_data) == TBE_OK) {
      tbe_list_node *ptmp;

      found = 1;

      if (!prev) {
        if (curr == a_list->tail)
          a_list->tail = NULL;
        a_list->head = a_list->head->next;
      }
      else {
        if (curr == a_list->tail)
          a_list->tail = prev;
        prev->next = curr->next;
      }

      ptmp = curr;
      curr = curr->next;

      if (a_fr)
        a_fr(ptmp->data);

      TBE_PTR_FREE(ptmp);

      (a_list->length)--;
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  return (found ? TBE_OK : TBE_NOTFOUND);
}

/* gives a reference to the index'th data */
int tbe_list_get_index(tbe_list a_list, unsigned int a_index, void **a_ref)
{
  tbe_list_node *curr;
  unsigned int i;

  if (!a_ref)
    return TBE_NULLPTR;

  if (a_list.length <= 0 || a_index >= a_list.length)
    return TBE_OUTOFBOUNDS;

  curr = a_list.head;

  for (i = 0; i < a_index; i++)
    curr = curr->next;

  *a_ref = curr->data;

  return TBE_OK;
}
/* gives a reference to all the nodes that matches data. array + size */
int tbe_list_get_data_all(tbe_list a_list,
                          void *a_data,
                          int (*a_cmp)(void *, void*),
                          void ***a_array,
                          unsigned int *a_size)
{
  tbe_list_node *curr;

  if (!a_data || !a_cmp || !a_array || !a_size)
    return TBE_NULLPTR;

  curr = a_list.head;
  *a_size = 0;

  while (curr) {
    if (a_cmp(curr->data, a_data) == TBE_OK) {

      if (!(*a_array = TBE_PTR_REALLOC(*a_array, void *, *a_size + 1)))
        return TBE_MALLOCFAILED;

      (*a_array)[*a_size] = curr->data;
      (*a_size)++;
    }
    curr = curr->next;
  }
  return ((*a_size > 0) ? TBE_OK : TBE_NOTFOUND);
}

/* gives a reference to the FIRST node that matches data */
int tbe_list_get_data_one(tbe_list a_list,
                          void *a_data,
                          int (*a_cmp)(void *, void*),
                          void **a_ref)
{
  tbe_list_node *curr;

  if (!a_data || !a_cmp || !a_ref)
    return TBE_NULLPTR;

  curr = a_list.head;

  while (curr) {
    if (a_cmp(a_data, curr->data) == TBE_OK) {
      *a_ref = curr->data;
      break;
    }
    curr = curr->next;
  }
  return ((*a_ref) ? TBE_OK : TBE_NOTFOUND);
}

/* gives a reference to the FIRST node whose hash value matches */
int tbe_list_get_data_hash(tbe_list a_list,
                           unsigned int a_hval,
                           unsigned int (a_hfn)(void *),
                           void **a_ref)
{
  tbe_list_node *curr;

  if (!a_hfn || !a_ref)
    return TBE_NULLPTR;

  curr = a_list.head;

  while (curr) {
    if (a_hfn(curr->data) == a_hval) {
      *a_ref = curr->data;
      break;
    }
    curr = curr->next;
  }
  return ((*a_ref) ? TBE_OK : TBE_NOTFOUND);
}

/* returns 0 if data is in the list, uses cmp to compare pointers */
int tbe_list_find_data(tbe_list a_list,
                       void *a_data,
                       int (*a_cmp)(void *, void *))
{
  tbe_list_node *curr;

  if (!a_data || !a_cmp)
    return TBE_NULLPTR;

  curr = a_list.head;

  while (curr) {
    if (a_cmp(curr->data, a_data) == TBE_OK)
      return TBE_OK;

    curr = curr->next;
  }

  return TBE_NOTFOUND;
}

/* makes an exact copy of l1 to l2, uses cpy to make a new copy */
int tbe_list_copy(tbe_list a_list1,
                  tbe_list *a_list2,
                  int (*a_cpy)(void *, void **))
{
  tbe_list_node *curr;
  tbe_list_node *node;

  if (!a_list2 || !a_cpy)
    return TBE_NULLPTR;

  curr = a_list1.head;
  a_list2->head = NULL;
  a_list2->tail = NULL;
  a_list2->length = 0;

  while (curr) {
    void *data;

    if (a_cpy(curr->data, &data) != TBE_OK)
      return TBE_FAILURE;

    if (!(node = TBE_PTR_MALLOC(tbe_list_node, 1)))
      return TBE_MALLOCFAILED;

    if (a_list2->tail)
      a_list2->tail->next = node;
    else
      a_list2->head = node;

    node->data = data;
    node->next = NULL;
    a_list2->tail = node;
    (a_list2->length)++;
    curr = curr->next;
  }
  return TBE_OK;
}
