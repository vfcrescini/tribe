#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/rel.h>
#include <tribe/rlist.h>

typedef struct {
  unsigned int interval;
  unsigned int relset;
} __tbe_rlist_node;

typedef struct {
  unsigned int interval;
  FILE *stream;
} __tbe_rlist_dump;

/* return TBE_OK if the intervals of the 2 tbe_rlist_nodes are equal */
static int tbe_rlist_cmp(void *a_ptr1, void *a_ptr2);

/* dump rlist node. use with tbe_list_traverse */
static int tbe_rlist_trav_dump(const void *a_node, void *a_dump);

/* return TBE_OK if the intervals of the 2 tbe_rlist_nodes are equal */
static int tbe_rlist_cmp(void *a_ptr1, void *a_ptr2)
{
  __tbe_rlist_node *ptr1 = (__tbe_rlist_node *) a_ptr1;
  __tbe_rlist_node *ptr2 = (__tbe_rlist_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE);
}

/* dump rlist node. use with tbe_list_traverse */
static int tbe_rlist_trav_dump(const void *a_node, void *a_dump)
{
  __tbe_rlist_node *rptr = (__tbe_rlist_node *) a_node;
  __tbe_rlist_dump *dptr = (__tbe_rlist_dump *) a_dump;

  if (!rptr || !dptr)
    return TBE_NULLPTR;

  if (!dptr->stream)
    return TBE_INVALIDINPUT;

  fprintf(dptr->stream, "%03u ", dptr->interval);
  tbe_rel_set_dump(rptr->relset, dptr->stream);
  fprintf(dptr->stream, "%03u\n", rptr->interval);

  return TBE_OK;
}

/* create a new rlist */
int tbe_rlist_create(tbe_rlist *a_rlist)
{
  return tbe_list_create(a_rlist);
}

/* destroy the rlist */
void tbe_rlist_destroy(tbe_rlist *a_rlist)
{
  tbe_list_destroy(a_rlist, tbe_list_free);
}

/* purge the rlist */
void tbe_rlist_purge(tbe_rlist a_rlist)
{
  tbe_list_purge(a_rlist, tbe_list_free);
}

/* add to the list. if a_rs contains all rels, it is removed (or not added) */
int tbe_rlist_add(tbe_rlist a_rlist, unsigned int a_int, unsigned int a_rs)
{
  __tbe_rlist_node *nptr;
  __tbe_rlist_node *tptr;
  __tbe_rlist_node tnode;
  int retval;

  if (!a_rlist)
    return TBE_NULLPTR;

  tnode.interval = a_int;

  /* see if a_int is already in this rlist */
  retval = tbe_list_get_data_one(a_rlist,
                                 (void *) &tnode,
                                 tbe_rlist_cmp,
                                 (void *) &tptr);

  if (retval == TBE_OK) {
    /* a_int is already in. decide if we should remove or replace rs */
    if (TBE_REL_SET_ISFILL(a_rs)) {
      /* a_rs is all relations, so we need to remove from the rlist */
      return tbe_list_del_data(a_rlist,
                               (void *) &tnode,
                               tbe_rlist_cmp,
                               tbe_list_free);
    }
    else {
      /* a_rs is not all relations so we blindly replace it */
      tptr->relset = a_rs;
      return TBE_OK;
    }
  }
  else if (retval == TBE_NOTFOUND) {
    /* a_int is not in the rlist. see if we need to add */
    if (TBE_REL_SET_ISFILL(a_rs)) {
      /* a_rs is all relations, DO NOTHING! */
      return TBE_OK;
    }
    else {
      /* now we add to rlist */
      if (!(nptr = TBE_MEM_MALLOC(__tbe_rlist_node, 1)))
        return TBE_MALLOCFAILED;

      nptr->interval = a_int;
      nptr->relset = a_rs;

      return tbe_list_add_tail(a_rlist, (void *) nptr);
    }
  }

  return retval;
}

/* gives the rs of the given interval. if a_int isn't in, a_rs is set to all */
int tbe_rlist_get(tbe_rlist a_rlist, unsigned int a_int, unsigned int *a_rs)
{
  __tbe_rlist_node *tptr;
  __tbe_rlist_node tnode;
  int retval;

  if (!a_rlist || !a_rs)
    return TBE_NULLPTR;

  tnode.interval = a_int;

  retval = tbe_list_get_data_one(a_rlist,
                                 (void *) &tnode,
                                 tbe_rlist_cmp,
                                 (void *) &tptr);

  switch (retval) {
    case TBE_OK :
      *a_rs = tptr->relset;
      return TBE_OK;
    case TBE_NOTFOUND :
      TBE_REL_SET_FILL(*a_rs);
      return TBE_OK;
  }

  return retval;
}

/* dumps the list, each printed with a_int */
int tbe_rlist_dump(tbe_rlist a_rlist, unsigned int a_int, FILE *a_stream)
{
  __tbe_rlist_dump dtmp;

  if (!a_rlist || !a_stream)
    return TBE_NULLPTR;

  dtmp.interval = a_int;
  dtmp.stream = a_stream;

  /* if the given rlist is empty, just print the given interval */
  if (tbe_list_length(a_rlist) == 0) {
    fprintf(a_stream, "%03u\n", a_int);
    return TBE_OK;
  }

  /* go through each node and dump */
  return tbe_list_traverse(a_rlist, tbe_rlist_trav_dump, (void *) &dtmp);
}
