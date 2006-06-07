/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/list.h>

typedef struct __tbe_list_node { 
  void *data;
  struct __tbe_list_node *next;
} __tbe_list_node;

typedef struct {
  __tbe_list_node *head;
  __tbe_list_node *tail;
  unsigned int length;
} __tbe_list;

/* create a new list, returns NULL if unsucessful */
int tbe_list_create(tbe_list *a_list)
{
  __tbe_list *tmp;

  if (!a_list)
    return TBE_NULLPTR;

  if (!(tmp = TBE_MEM_MALLOC(__tbe_list, 1)))
    return TBE_MALLOCFAILED;

  *a_list = (tbe_list) tmp;

  tmp->head = NULL;
  tmp->tail = NULL;
  tmp->length = 0;

  return TBE_OK;
}

/* destroy the list, use a_fr to free nodes */
void tbe_list_destroy(tbe_list *a_list, void (*a_fr)(void *))
{
  if (a_list) {
    tbe_list_purge(*a_list, a_fr);
    TBE_MEM_FREE((__tbe_list *) *a_list);
  }
}

/* purge the list, use a_fr to free nodes */
void tbe_list_purge(tbe_list a_list, void (*a_fr)(void *))
{
  __tbe_list *tmp;
  __tbe_list_node *curr;
  __tbe_list_node *prev;

  if ((tmp = (__tbe_list *) a_list)) {
    curr = tmp->head;

    while (curr) {
      prev = curr;
      curr = curr->next;

      /* if a_fr was provided, we must use it upon the data field  */
      if (a_fr)
        a_fr(prev->data);

      TBE_MEM_FREE(prev);
    }

    tmp->head = NULL;
    tmp->tail = NULL;
    tmp->length = 0;
  }
}

/* generic function to free nodes to be used for destroy, purge, etc. */
void tbe_list_free(void *a_node)
{
  TBE_MEM_FREE(a_node);
}

/* return length */
unsigned int tbe_list_length(tbe_list a_list)
{
  return (a_list) ? ((__tbe_list *) a_list)->length : 0;
}

/* add node to the head of the list, assumes memory has been pre-allocated */
int tbe_list_add_head(tbe_list a_list, void *a_data)
{
  __tbe_list *tmp;
  __tbe_list_node *node;

  if (!(tmp = (__tbe_list *) a_list) || !a_data)
    return TBE_NULLPTR;

  if (!(node = TBE_MEM_MALLOC(__tbe_list_node, 1)))
    return TBE_MALLOCFAILED;

  node->data = a_data;
  node->next = tmp->head;

  if (!tmp->tail)
    tmp->tail = node;

  tmp->head = node;
  tmp->length = tmp->length + 1;

  return TBE_OK;
}

/* add node to the tail of the list, assumes memory has been pre-allocated */
int tbe_list_add_tail(tbe_list a_list, void *a_data)
{
  __tbe_list *tmp;
  __tbe_list_node *node;

  if (!(tmp = (__tbe_list *) a_list) || !a_data)
    return TBE_NULLPTR;

  if (!(node = TBE_MEM_MALLOC(__tbe_list_node, 1)))
    return TBE_MALLOCFAILED;

  if (tmp->tail)
    tmp->tail->next = node;
  else
    tmp->head = node;

  node->data = a_data;
  node->next = NULL;
  tmp->tail = node;
  tmp->length = tmp->length + 1;

  return TBE_OK;
}

/* deletes first node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_head(tbe_list a_list, void (*a_fr)(void *))
{
  __tbe_list *tmp;
  __tbe_list_node *node;

  if (!(tmp = (__tbe_list *) a_list))
    return TBE_NULLPTR;

  if (tmp->length <= 0)
    return TBE_OUTOFBOUNDS;
 
  node = tmp->head;
  tmp->head = tmp->head->next;

  if (!tmp->head)
    tmp->tail = NULL;

  if (a_fr)
    a_fr(node->data);

  TBE_MEM_FREE(node);

  tmp->length = tmp->length - 1;

  return TBE_OK;
}

/* deletes last node, give fr to free the pointer or NULL to not free it */
int tbe_list_del_tail(tbe_list a_list, void (*a_fr)(void *))
{
  if (!a_list)
    return TBE_NULLPTR;

  if (((__tbe_list *) a_list)->length <= 0)
    return TBE_OUTOFBOUNDS;

  /* unfortunately, the only way to get the prev ptr is to go through all the
   * nodes */
  return tbe_list_del_index(a_list, ((__tbe_list *) a_list)->length - 1, a_fr);
}

/* deletes index'th data, give fr to free the pointer or NULL to not free it */
int tbe_list_del_index(tbe_list a_list,
                       unsigned int a_index,
                       void (*a_fr)(void *))
{
  __tbe_list *tmp;
  __tbe_list_node *prev;
  __tbe_list_node *curr;
  unsigned int i;

  if (!(tmp = (__tbe_list *) a_list))
    return TBE_NULLPTR;

  if (tmp->length <= 0 || a_index >= tmp->length)
    return TBE_OUTOFBOUNDS;

  prev = NULL;
  curr = tmp->head;

  for (i = 0; i < a_index; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (!prev) {
    if (curr == tmp->tail)
      tmp->tail = NULL;
    tmp->head = tmp->head->next;
  }
  else {
    if (curr == tmp->tail)
      tmp->tail = prev;
    prev->next = curr->next;
  }

  if (a_fr)
    a_fr(curr->data);

  TBE_MEM_FREE(curr);

  tmp->length = tmp->length - 1;

  return TBE_OK;
}

/* deletes all the nodes that matches data, uses cmp to compare, fr to free or
 * NULL to not free them */
int tbe_list_del_data(tbe_list a_list,
                      const void *a_data,
                      int (*a_cmp)(const void *, const void *),
                      void (*a_fr)(void *))
{
  __tbe_list *tmp;
  __tbe_list_node *prev;
  __tbe_list_node *curr;
  char found = 0;

  if (!(tmp = (__tbe_list *) a_list) || !a_data || !a_cmp)
    return TBE_NULLPTR;

  prev = NULL;
  curr = tmp->head;

  while (curr) {
    if (a_cmp(curr->data, a_data) == TBE_OK) {
      __tbe_list_node *ptmp;

      found = 1;

      if (!prev) {
        if (curr == tmp->tail)
          tmp->tail = NULL;
        tmp->head = tmp->head->next;
      }
      else {
        if (curr == tmp->tail)
          tmp->tail = prev;
        prev->next = curr->next;
      }

      ptmp = curr;
      curr = curr->next;

      if (a_fr)
        a_fr(ptmp->data);

      TBE_MEM_FREE(ptmp);

      tmp->length = tmp->length - 1;
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  return (found ? TBE_OK : TBE_NOTFOUND);
}

/* gives a reference to the FIRST node data in the list */
int tbe_list_get_head(tbe_list a_list, void **a_ref)
{
  __tbe_list *tmp;

  if (!(tmp = (__tbe_list *) a_list) || !a_ref)
    return TBE_NULLPTR;

  if (tmp->length <= 0)
    return TBE_NOTFOUND;

  *a_ref = tmp->head->data;

  return TBE_OK;
}

/* gives a reference to the LAST node in the list */
int tbe_list_get_tail(tbe_list a_list, void **a_ref)
{
  __tbe_list *tmp;

  if (!(tmp = (__tbe_list *) a_list) || !a_ref)
    return TBE_NULLPTR;

  if (tmp->length <= 0)
    return TBE_NOTFOUND;

  *a_ref = tmp->tail->data;

  return TBE_OK;
}

/* gives a reference to the index'th data */
int tbe_list_get_index(tbe_list a_list, unsigned int a_index, void **a_ref)
{
  __tbe_list *tmp;
  __tbe_list_node *curr;
  unsigned int i;

  if (!(tmp = (__tbe_list *) a_list) || !a_ref)
    return TBE_NULLPTR;

  if (tmp->length <= 0 || a_index >= tmp->length)
    return TBE_OUTOFBOUNDS;

  curr = tmp->head;

  for (i = 0; i < a_index; i++)
    curr = curr->next;

  *a_ref = curr->data;

  return TBE_OK;
}

/* gives a reference to all the nodes that matches data. array + size */
int tbe_list_get_data_all(tbe_list a_list,
                          const void *a_data,
                          int (*a_cmp)(const void *, const void*),
                          void ***a_array,
                          unsigned int *a_size)
{
  __tbe_list_node *curr;

  if (!a_list || !a_data || !a_cmp || !a_array || !a_size)
    return TBE_NULLPTR;

  curr = ((__tbe_list *) a_list)->head;
  *a_size = 0;
  *a_array = NULL;

  while (curr) {
    if (a_cmp(curr->data, a_data) == TBE_OK) {

      if (!(*a_array = TBE_MEM_REALLOC(*a_array, void *, *a_size + 1)))
        return TBE_MALLOCFAILED;

      (*a_array)[*a_size] = curr->data;
      *a_size = *a_size + 1;
    }
    curr = curr->next;
  }

  return ((*a_size > 0) ? TBE_OK : TBE_NOTFOUND);
}

/* gives a reference to the FIRST node that matches data */
int tbe_list_get_data_one(tbe_list a_list,
                          const void *a_data,
                          int (*a_cmp)(const void *, const void *),
                          void **a_ref)
{
  __tbe_list_node *curr;

  if (!a_list || !a_data || !a_cmp || !a_ref)
    return TBE_NULLPTR;

  curr = ((__tbe_list *) a_list)->head;

  while (curr) {
    if (a_cmp(a_data, curr->data) == TBE_OK) {
      *a_ref = curr->data;
      return TBE_OK;
    }

    curr = curr->next;
  }

  return TBE_NOTFOUND;
}

/* gives a reference to the FIRST node whose hash value matches */
int tbe_list_get_data_hash(tbe_list a_list,
                           unsigned int a_hval,
                           unsigned int (a_hfn)(const void *),
                           void **a_ref)
{
  __tbe_list_node *curr;

  if (!a_list || !a_hfn || !a_ref)
    return TBE_NULLPTR;

  curr = ((__tbe_list *) a_list)->head;

  while (curr) {
    if (a_hfn(curr->data) == a_hval) {
      *a_ref = curr->data;
      return TBE_OK;
    }

    curr = curr->next;
  }

  return TBE_NOTFOUND;
}

/* returns 0 if data is in the list, uses cmp to compare pointers */
int tbe_list_find_data(tbe_list a_list,
                       const void *a_data,
                       int (*a_cmp)(const void *, const void *))
{
  __tbe_list_node *curr;

  if (!a_list || !a_data || !a_cmp)
    return TBE_NULLPTR;

  curr = ((__tbe_list *) a_list)->head;

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
                  int (*a_cpy)(const void *, void **))
{
  __tbe_list *tmp;
  __tbe_list_node *curr;
  __tbe_list_node *node;
  int retval;

  if (!a_list1 || !a_list2 || !a_cpy)
    return TBE_NULLPTR;

  /* create a new list */
  if ((retval = tbe_list_create(a_list2)) != TBE_OK)
    return retval;

  /* now copy each node from the first list to the second */
  tmp = (__tbe_list *) *a_list2;
  curr = ((__tbe_list *) a_list1)->head;

  while (curr) {
    void *data = NULL;

    /* up to the caller to destroy the new list if an error occurs */
    if (a_cpy(curr->data, &data) != TBE_OK)
      return TBE_FAILURE;

    if (!(node = TBE_MEM_MALLOC(__tbe_list_node, 1)))
      return TBE_MALLOCFAILED;

    if (tmp->tail)
      tmp->tail->next = node;
    else
      tmp->head = node;

    node->data = data;
    node->next = NULL;
    tmp->tail = node;
    tmp->length = tmp->length + 1;
    curr = curr->next;
  }

  return TBE_OK;
}

/* traverses the list, calling fn on each node, stops at the end or when fn
 * returns something other that TBE_OK, in which that retval is returned. the
 * node ptr is passed as the first parameter and a_parm is passed second. */
int tbe_list_traverse(tbe_list a_list,
                      int (*a_fn)(const void *, void *),
                      void *a_parm)
{
  __tbe_list *tmp;
  __tbe_list_node *curr;
  int retval = TBE_OK;

  if (!(tmp = (__tbe_list *) a_list))
    return TBE_NULLPTR;

  /* nothing to do */
  if (!a_fn)
    return TBE_OK;

  curr = tmp->head;

  while (curr && retval == TBE_OK) {
    retval = a_fn((const void *) curr->data, a_parm);
    curr = curr->next;
  }

  return retval;
}
