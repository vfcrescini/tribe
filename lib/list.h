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

#ifndef __TBE_LIST_H
#define __TBE_LIST_H

#include <tribe/tribe.h>

typedef void *tbe_list;

/* create a new list */
TBE_EXTERN int tbe_list_create(tbe_list *a_list);

/* destroy the list, use a_fr to free nodes */
TBE_EXTERN void tbe_list_destroy(tbe_list *a_list, void (*a_fr)(void *));

/* purge the list, use a_fr to free nodes */
TBE_EXTERN void tbe_list_purge(tbe_list a_list, void (*a_fr)(void *));

/* generic function to free nodes to be used for destroy, purge, etc. */
TBE_EXTERN void tbe_list_free(void *a_node);

/* return length */
TBE_EXTERN unsigned int tbe_list_length(tbe_list a_list);

/* add node to the head of the list, assumes memory has been pre-allocated */
TBE_EXTERN int tbe_list_add_head(tbe_list a_list, void *a_data);

/* add node to the tail of the list, assumes memory has been pre-allocated */
TBE_EXTERN int tbe_list_add_tail(tbe_list a_list, void *a_data);

/* deletes first node, give fr to free the pointer or NULL to not free it */
TBE_EXTERN int tbe_list_del_head(tbe_list a_list, void (*a_fr)(void *));

/* deletes last node, give fr to free the pointer or NULL to not free it */
TBE_EXTERN int tbe_list_del_tail(tbe_list a_list, void (*a_fr)(void *));

/* deletes index'th data, give fr to free the pointer or NULL to not free it */
TBE_EXTERN int tbe_list_del_index(tbe_list a_list,
                                  unsigned int a_index,
                                  void (*a_fr)(void *));

/* deletes all the nod es that matches data, uses cmp to compare, fr to free or
 * NULL to not free them */
TBE_EXTERN int tbe_list_del_data(tbe_list a_list,
                                 const void *a_data,
                                 int (*a_cmp)(const void *, const void *),
                                 void (*a_fr)(void *));

/* gives a reference to the FIRST node data in the list */
TBE_EXTERN int tbe_list_get_head(tbe_list a_list, void **a_ref);

/* gives a reference to the LAST node data in the list */
TBE_EXTERN int tbe_list_get_tail(tbe_list a_list, void **a_ref);

/* gives a reference to the index'th data */
TBE_EXTERN int tbe_list_get_index(tbe_list a_list,
                                  unsigned int a_index,
                                  void **a_ref);

/* gives a reference to all the nodes that matches data. array + size */
TBE_EXTERN int tbe_list_get_data_all(tbe_list a_list,
                                     const void *a_data,
                                     int (*a_cmp)(const void *, const void*),
                                     void ***a_array,
                                     unsigned int *a_size);

/* gives a reference to the FIRST node that matches data */
TBE_EXTERN int tbe_list_get_data_one(tbe_list a_list,
                                     const void *a_data,
                                     int (*a_cmp)(const void *, const void*),
                                     void **a_ref);

/* gives a reference to the FIRST node whose hash value matches */
TBE_EXTERN int tbe_list_get_data_hash(tbe_list a_list,
                                      unsigned int a_hval,
                                      unsigned int (a_hfn)(const void *),
                                      void **a_ref);

/* returns 0 if data is in the list, uses cmp to compare pointers */
TBE_EXTERN int tbe_list_find_data(tbe_list a_list,
                                  const void *a_data,
                                  int (*a_cmp)(const void *, const void *));

/* makes an exact copy of l1 to l2, uses cpy to make a new copy */
TBE_EXTERN int tbe_list_copy(tbe_list a_list1,
                             tbe_list *a_list2,
                             int (*a_cpy)(const void *, void **));

/* traverses the list, calling fn on each node, stops at the end or when fn
 * returns something other that TBE_OK, in which that retval is returned. the
 * node ptr is passed as the first parameter and a_parm is passed second. */
TBE_EXTERN int tbe_list_traverse(tbe_list a_list,
                                 int (*a_fn)(const void *, void *),
                                 void *a_parm);

#endif
