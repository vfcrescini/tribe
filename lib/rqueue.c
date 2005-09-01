#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/rel.h>
#include <tribe/rqueue.h>

typedef struct {
  unsigned int id1;
  unsigned int id2;
  unsigned int rs;
} __tbe_rqueue_node;

/* return TBE_OK if the 2 rqueue nodes are equal */
static int tbe_rqueue_cmp(const void *a_ptr1, const void *a_ptr2);

/* normalise the relation, a relation "A rs B" is normalised if A <= B */
static int tbe_rqueue_normalise(unsigned int *a_id1,
                                unsigned int *a_id2,
                                unsigned int *a_rs);

/* return TBE_OK if the 2 rqueue nodes are equal */
static int tbe_rqueue_cmp(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_rqueue_node *pnode1;
  __tbe_rqueue_node *pnode2;

  pnode1 = (__tbe_rqueue_node *) a_ptr1;
  pnode2 = (__tbe_rqueue_node *) a_ptr2;

  if (!pnode1 || !pnode2)
    return TBE_NULLPTR;

  return 
    (pnode1->id1 == pnode2->id1) &&
    (pnode1->id2 == pnode2->id2) &&
    (pnode1->rs == pnode2->rs) ? 
    TBE_OK : TBE_FAILURE;
}

/* normalise the relation, a relation "A rs B" is normalised if A <= B */
static int tbe_rqueue_normalise(unsigned int *a_id1,
                                unsigned int *a_id2,
                                unsigned int *a_rs)
{
  unsigned int min;
  unsigned int max;

  if (!a_id1 || !a_id2 || !a_rs)
    return TBE_NULLPTR;

  if (*a_id1 > *a_id2)
    *a_rs = tbe_rel_inverse(*a_rs);

  min = TBE_INT_MIN(*a_id1, *a_id2);
  max = TBE_INT_MAX(*a_id1, *a_id2);

  *a_id1 = min;
  *a_id2 = max;

  return TBE_OK;
}

/* create a new rqueue */
int tbe_rqueue_create(tbe_rqueue *a_rqueue)
{
  return tbe_list_create(a_rqueue);
}

/* destroy the queue */
void tbe_rqueue_destroy(tbe_rqueue *a_rqueue)
{
  tbe_list_destroy(a_rqueue, tbe_list_free);
}

/* purge the queue */
void tbe_rqueue_purge(tbe_rqueue a_rqueue)
{
  tbe_list_purge(a_rqueue, tbe_list_free);
}

/* add rel at the end of the queue (rels get normalised) */
int tbe_rqueue_enq(tbe_rqueue a_rqueue,
                   unsigned int a_id1,
                   unsigned int a_id2,
                   unsigned int a_rs)
{
  __tbe_rqueue_node *pnode;
  int retval;

  if (!a_rqueue)
    return TBE_NULLPTR;

  tbe_rqueue_normalise(&a_id1, &a_id2, &a_rs);

  if (!(pnode = TBE_MEM_MALLOC(__tbe_rqueue_node, 1)))
    return TBE_MALLOCFAILED;

  pnode->id1 = a_id1;
  pnode->id2 = a_id2;
  pnode->rs = a_rs;

  retval = tbe_list_find_data(a_rqueue, (void *) pnode, tbe_rqueue_cmp);

  switch (retval) {
    case TBE_OK :
      /* a node containing these values already exist */
      TBE_MEM_FREE(pnode);
      return TBE_OK;
    case TBE_NOTFOUND :
      /* no such node exists, so we add it */
      return tbe_list_add_tail(a_rqueue, (void *) pnode);
  }

  return retval;
}

/* remove rel from the front of the queue (and copy to given ptrs) */
int tbe_rqueue_deq(tbe_rqueue a_rqueue,
                   unsigned int *a_id1,
                   unsigned int *a_id2,
                   unsigned int *a_rs)
{
  __tbe_rqueue_node *pnode;
  int retval;

  if (!a_rqueue || !a_id1 || !a_id2 || !a_rs)
    return TBE_OK;

  if ((retval = tbe_list_get_head(a_rqueue, (void *) &pnode)) != TBE_OK)
    return retval;

  *a_id1 = pnode->id1;
  *a_id2 = pnode->id2;
  *a_rs = pnode->rs;

  return tbe_list_del_head(a_rqueue, tbe_list_free);
}
