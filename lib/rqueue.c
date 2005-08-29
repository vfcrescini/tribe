#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/rqueue.h>

/* return TBE_OK if the 2 rqueue nodes are equal */
static int tbe_rqueue_cmp(const void *a_ptr1, const void *a_ptr2);

/* return TBE_OK if the 2 rqueue nodes are equal */
static int tbe_rqueue_cmp(const void *a_ptr1, const void *a_ptr2)
{
  tbe_rel *ptr1;
  tbe_rel *ptr2;

  if (!(ptr1 = (tbe_rel *) a_ptr1))
    return TBE_NULLPTR;
  if (!(ptr2 = (tbe_rel *) a_ptr2))
    return TBE_NULLPTR;

  return TBE_REL_ISEQL(*ptr1, *ptr2) ? TBE_OK : TBE_FAILURE;
}

/* create a new rqueue */
int tbe_rqueue_create(tbe_rqueue *a_rqueue)
{
  return tbe_list_create(a_rqueue);
}

/* destroy the queue */
void tbe_rqueue_destroy(tbe_rqueue *a_rqueue)
{
  tbe_list_destroy(a_rqueue, tbe_rel_free);
}

/* purge the queue */
void tbe_rqueue_purge(tbe_rqueue a_rqueue)
{
  tbe_list_purge(a_rqueue, tbe_rel_free);
}

/* add rel at the end of the queue */
int tbe_rqueue_enq1(tbe_rqueue a_rqueue,
                    unsigned int a_int_id1,
                    unsigned int a_int_id2,
                    unsigned int a_rs)
{
  tbe_rel *ptr;
  int retval;

  if (!a_rqueue)
    return TBE_NULLPTR;

  if ((retval = tbe_rel_create(&ptr)) != TBE_OK)
    return retval;

  TBE_REL_INIT(*ptr, a_int_id1, a_int_id2, a_rs);

  /* a good place to normalise the relation */
  tbe_rel_normalise(ptr);

  retval = tbe_list_find_data(a_rqueue, (void *) ptr, tbe_rqueue_cmp);

  switch (retval) {
    case TBE_OK :
      /* a node containing these values already exist */
      TBE_MEM_FREE(ptr);
      return TBE_OK;
    case TBE_NOTFOUND :
      /* no such node exists, so we add it */
      return tbe_list_add_tail(a_rqueue, (void *) ptr);
  }

  return retval;
}

/* as above, but with a tbe_rel structure */
int tbe_rqueue_enq2(tbe_rqueue a_rqueue, tbe_rel a_rel)
{
  return tbe_rqueue_enq1(a_rqueue, a_rel.int_id1, a_rel.int_id2, a_rel.rs);
}

/* remove rel from the front of the queue (and copy to given ptrs) */
int tbe_rqueue_deq1(tbe_rqueue a_rqueue,
                    unsigned int *a_int_id1,
                    unsigned int *a_int_id2,
                    unsigned int *a_rs)
{
  tbe_rel *ptr;
  int retval;

  if (!a_rqueue)
    return TBE_NULLPTR;

  if (!a_int_id1 && !a_int_id2 && !a_rs)
    return TBE_OK;

  if ((retval = tbe_list_get_head(a_rqueue, (void *) &ptr)) != TBE_OK)
    return retval;

  if (a_int_id1)
    *a_int_id1 = ptr->int_id1;
  if (a_int_id2)
    *a_int_id2 = ptr->int_id2;
  if (a_rs)
    *a_rs = ptr->rs;

  return tbe_list_del_head(a_rqueue, tbe_rel_free);
}

/* as above, but with a tbe_rel structure */
int tbe_rqueue_deq2(tbe_rqueue a_rqueue, tbe_rel *a_rel)
{
  if (!a_rel)
    return TBE_NULLPTR;

  return tbe_rqueue_deq1(a_rqueue,
                         &(a_rel->int_id1),
                         &(a_rel->int_id2),
                         &(a_rel->rs));
}
