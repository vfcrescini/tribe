#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/rel.h>
#include <tribe/rlist.h>

typedef struct {
  unsigned int id;
  unsigned int rs;
} __tbe_rlist_node;

typedef struct {
  unsigned int id;
  FILE *stream;
} __tbe_rlist_dump;

/* return TBE_OK if the intervals of the 2 tbe_rlist_nodes are equal */
static int tbe_rlist_cmp(const void *a_ptr1, const void *a_ptr2);

/* dump rlist node. use with tbe_list_traverse */
static int tbe_rlist_trav_dump(const void *a_node, void *a_dump);

/* return TBE_OK if the intervals of the 2 tbe_rlist_nodes are equal */
static int tbe_rlist_cmp(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_rlist_node *pnode1;
  __tbe_rlist_node *pnode2;

   pnode1 = (__tbe_rlist_node *) a_ptr1;
   pnode2 = (__tbe_rlist_node *) a_ptr2;

  if (!pnode1 || !pnode2)
    return TBE_NULLPTR;

  return ((pnode1->id == pnode2->id) ? TBE_OK : TBE_FAILURE);
}

/* dump rlist node. use with tbe_list_traverse */
static int tbe_rlist_trav_dump(const void *a_node, void *a_dump)
{
  __tbe_rlist_node *pnode;
  __tbe_rlist_dump *pdump;

  pnode = (__tbe_rlist_node *) a_node;
  pdump = (__tbe_rlist_dump *) a_dump;

  if (!pnode || !pdump)
    return TBE_NULLPTR;

  fprintf(pdump->stream, "%03u ", pdump->id);
  tbe_rel_dump(pnode->rs, pdump->stream);
  fprintf(pdump->stream, "%03u\n", pnode->id);

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
int tbe_rlist_add(tbe_rlist a_rlist, unsigned int a_id, unsigned int a_rs)
{
  __tbe_rlist_node *pnode;
  __tbe_rlist_node node;
  int retval;

  if (!a_rlist)
    return TBE_NULLPTR;

  node.id = a_id;

  /* see if a_id is already in this rlist */
  retval = tbe_list_get_data_one(a_rlist,
                                 (void *) &node,
                                 tbe_rlist_cmp,
                                 (void *) &pnode);

  if (retval == TBE_OK) {
    /* a_id is already in. decide if we should remove or replace rs */
    if (TBE_REL_SET_ISFILL(a_rs)) {
      /* a_rs is all relations, so we need to remove from the rlist */
      return tbe_list_del_data(a_rlist,
                               (void *) &node,
                               tbe_rlist_cmp,
                               tbe_list_free);
    }
    else {
      /* a_rs is not all relations so we blindly replace it */
      pnode->rs = a_rs;
      return TBE_OK;
    }
  }
  else if (retval == TBE_NOTFOUND) {
    /* a_id is not in the rlist. see if we need to add */
    if (TBE_REL_SET_ISFILL(a_rs)) {
      /* a_rs is all relations, DO NOTHING! */
      return TBE_OK;
    }
    else {
      /* now we add to rlist */
      if (!(pnode = TBE_MEM_MALLOC(__tbe_rlist_node, 1)))
        return TBE_MALLOCFAILED;

      pnode->id = a_id;
      pnode->rs = a_rs;

      return tbe_list_add_tail(a_rlist, (void *) pnode);
    }
  }

  return retval;
}

/* gives the rs of a_id. if a_id isn't in, a_rs is set to all */
int tbe_rlist_get(tbe_rlist a_rlist, unsigned int a_id, unsigned int *a_rs)
{
  __tbe_rlist_node *pnode;
  __tbe_rlist_node node;
  int retval;

  if (!a_rlist || !a_rs)
    return TBE_NULLPTR;

  node.id = a_id;

  retval = tbe_list_get_data_one(a_rlist,
                                 (void *) &node,
                                 tbe_rlist_cmp,
                                 (void *) &pnode);

  switch (retval) {
    case TBE_OK :
      *a_rs = pnode->rs;
      return TBE_OK;
    case TBE_NOTFOUND :
      TBE_REL_SET_FILL(*a_rs);
      return TBE_OK;
  }

  return retval;
}

/* dumps the list, each printed with a_id */
int tbe_rlist_dump(tbe_rlist a_rlist, unsigned int a_id, FILE *a_stream)
{
  __tbe_rlist_dump dump;

  if (!a_rlist || !a_stream)
    return TBE_NULLPTR;

  dump.id = a_id;
  dump.stream = a_stream;

  /* if the given rlist is empty, just print the given interval */
  if (tbe_list_length(a_rlist) == 0) {
    fprintf(a_stream, "%03u\n", a_id);
    return TBE_OK;
  }

  /* go through each node and dump */
  return tbe_list_traverse(a_rlist, tbe_rlist_trav_dump, (void *) &dump);
}
