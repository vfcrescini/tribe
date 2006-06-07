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
#include <tribe/rel.h>
#include <tribe/list.h>
#include <tribe/clist.h>

/* the node of the clist */
typedef struct {
  unsigned int v1;
  unsigned int v2;
  unsigned int rs;
} __tbe_clist_node;

/* the clist itself. the array keeps track of the variable used */
typedef struct {
  tbe_list list;
  unsigned int var_num;
  unsigned int *var_arr;
} __tbe_clist;

/* strucutre used for traversing to validate a given tuple */
typedef struct {
  unsigned int *tuple;
  unsigned int size;
  unsigned int (*fn)(unsigned int, unsigned int, void *);
  void *parm;
} __tbe_clist_trav;

/* return TBE_OK if the 2 clist nodes are equal */
static int tbe_clist_cmp(const void *a_ptr1, const void *a_ptr2);

/* add a_var to a_array if it's not already in, increment a_count */
static int tbe_clist_add_array(unsigned int *a_count,
                               unsigned int **a_array,
                               unsigned int a_var);

/* to be used by traverse. goes through each node, and checks whether the
 * given tuple satisfies each constraint, in which case TBE_OK is returned.
 * the tuple must be an array of the specified size */
static int tbe_clist_trav_validate(const void *a_node, void *a_parm);

/* return TBE_OK if the 2 clist nodes are equal */
static int tbe_clist_cmp(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_clist_node *nptr1;
  __tbe_clist_node *nptr2;

  nptr1 = (__tbe_clist_node *) a_ptr1;
  nptr2 = (__tbe_clist_node *) a_ptr2;

  if (!nptr1 || !nptr2)
    return TBE_NULLPTR;

  return 
    (nptr1->v1 == nptr2->v1) &&
    (nptr1->v1 == nptr2->v1) &&
    (nptr1->rs == nptr2->rs) ? 
    TBE_OK : TBE_FAILURE;
}

/* add a_var to a_array if it's not already in, increment a_count */
static int tbe_clist_add_array(unsigned int *a_count,
                               unsigned int **a_array,
                               unsigned int a_var)
{
  unsigned int i;

  if (!a_count || !a_array)
    return TBE_NULLPTR;

  for (i = 0; i < *a_count; i++) {
    if ((*a_array)[i] == a_var)
      return TBE_OK;
  }

  /* allocate new space, then add */
  if (!(*a_array = TBE_MEM_REALLOC(*a_array, unsigned int, *a_count + 1)))
    return TBE_MALLOCFAILED;

  (*a_array)[*a_count] = a_var;

  *a_count = *a_count + 1;

  return TBE_OK;
}

/* to be used by traverse. goes through each node, and checks whether the
 * given tuple satisfies each constraint, in which case TBE_OK is returned.
 * the tuple must be an array of the specified size */
static int tbe_clist_trav_validate(const void *a_node, void *a_parm)
{
  __tbe_clist_node *nptr;
  __tbe_clist_trav *tptr;
  unsigned int int1;
  unsigned int int2;
  unsigned int rs;

  nptr = (__tbe_clist_node *) a_node;
  tptr = (__tbe_clist_trav *) a_parm;

  if (!nptr || !tptr)
    return TBE_NULLPTR;

  /* tuple can't be size 0, nor can any var be bigger than the tuple size */
  if (tptr->size == 0 || tptr->size <= nptr->v1 || tptr->size <= nptr->v2)
    return TBE_INVALIDINPUT;

  /* the variables are actually indices of the tuple. there's no way to check
   * the size of the tuple. if this barfs, it is because of a bad tuple. */
  int1 = tptr->tuple[nptr->v1];
  int2 = tptr->tuple[nptr->v2];

  /* we assume vptr->fn() is a function that will give the relset */
  rs = tptr->fn(int1, int2, tptr->parm);
 
  /* now this relset must be a subset of the constraint relset */
  return (TBE_REL_SET_INTERSECT(rs, nptr->rs) == rs) ? TBE_OK : TBE_FAILURE;
}

/* create a new clist */
int tbe_clist_create(tbe_clist *a_clist)
{
  __tbe_clist *cptr;
  int retval;

  if (!a_clist)
    return TBE_NULLPTR;

  if (!(cptr = TBE_MEM_MALLOC(__tbe_clist, 1)))
    return TBE_MALLOCFAILED;

  if ((retval = tbe_list_create(&(cptr->list))) != TBE_OK) {
    TBE_MEM_FREE(cptr);
    return retval;
  }

  cptr->var_num = 0;
  cptr->var_arr = NULL;
  *a_clist = (tbe_clist) cptr;

  return TBE_OK;
}

/* destroy the clist */
void tbe_clist_destroy(tbe_clist *a_clist)
{
  __tbe_clist *cptr;

  if ((cptr = (__tbe_clist *) *a_clist)) {
    if (cptr->var_arr)
      TBE_MEM_FREE(cptr->var_arr);

    tbe_list_destroy(&(cptr->list), tbe_list_free);

    TBE_MEM_FREE(cptr);
  }
}

/* purge the clist */
void tbe_clist_purge(tbe_clist a_clist)
{
  __tbe_clist *cptr;

  if ((cptr = (__tbe_clist *) a_clist)) {
    if (cptr->var_arr)
      TBE_MEM_FREE(cptr->var_arr);

    cptr->var_num = 0;

    tbe_list_purge(&(cptr->list), tbe_list_free);
  }
}

/* add constraint */
int tbe_clist_add(tbe_clist a_clist,
                  unsigned int a_v1,
                  unsigned int a_v2,
                  unsigned int a_rs)
{
  __tbe_clist *cptr;
  __tbe_clist_node *nptr;
  int retval;

  if (!(cptr = (__tbe_clist *) a_clist))
    return TBE_NULLPTR;

  if (!(nptr = TBE_MEM_MALLOC(__tbe_clist_node, 1)))
    return TBE_MALLOCFAILED;

  nptr->v1 = a_v1;
  nptr->v2 = a_v2;
  nptr->rs = a_rs;

  retval = tbe_list_find_data(cptr->list, (void *) nptr, tbe_clist_cmp);

  switch (retval) {
    case TBE_OK :
      /* a node containing these values already exist */
      TBE_MEM_FREE(nptr);
      return TBE_OK;
    case TBE_NOTFOUND :
      /* no such node exists, so we add it */
      if ((retval = tbe_list_add_tail(cptr->list, (void *) nptr)) != TBE_OK)
        return retval;

      /* add v1 into var array */
      retval = tbe_clist_add_array(&(cptr->var_num), &(cptr->var_arr), a_v1);
      if (retval != TBE_OK)
        return retval;

      /* add v2 into var array */
      retval = tbe_clist_add_array(&(cptr->var_num), &(cptr->var_arr), a_v2);
      if (retval != TBE_OK)
        return retval;
  }

  return retval;
}

/* check that all variables in the list are under the given maximum */
int tbe_clist_verify(tbe_clist a_clist, unsigned int a_max)
{
  __tbe_clist *cptr;
  unsigned int i;

  if (!(cptr = (__tbe_clist *) a_clist))
    return TBE_NULLPTR;

  for (i = 0; i < cptr->var_num; i++) {
    if (cptr->var_arr[i] >= a_max)
      return TBE_FAILURE;
  }

  return TBE_OK;
}

/* return TBE_OK if the given tuple of the given size satisfies the given
 * clist, TBE_FAILURE if it doesn't, but no other error occurs. calls a_fn()
 * with the given parameter to get the relset of 2 intervals. each variable in
 * the clist is an index of the tuple. important: assumes a_tuple is of size
 * a_size. */
int tbe_clist_validate(tbe_clist a_clist,
                       unsigned int *a_tuple,
                       unsigned int a_size,
                       unsigned int (*a_fn)(unsigned int, unsigned int, void *),
                       void *a_parm)
{
  __tbe_clist *cptr;
  __tbe_clist_trav tnode;

  if (!(cptr = (__tbe_clist *) a_clist) || !a_tuple || !a_fn)
    return TBE_NULLPTR;

  if (a_size == 0)
    return TBE_INVALIDINPUT;

  /* of course, if there are no constraints, the tuple is ok */
  if (tbe_list_length(cptr->list) == 0)
    return TBE_OK;

  /* clist is not empty, so we have to traverse it */
  tnode.tuple = a_tuple;
  tnode.size = a_size;
  tnode.fn = a_fn;
  tnode.parm = a_parm;

  return tbe_list_traverse(cptr->list,
                           tbe_clist_trav_validate,
                           (void *) &tnode);
}
