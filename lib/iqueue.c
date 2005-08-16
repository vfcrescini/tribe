#include <stdlib.h>
#include <tribe/mem.h>
#include <tribe/iqueue.h>

/* queue to hold 2 intervals and a relation set */
typedef struct {
  unsigned int int1;
  unsigned int int2;
  unsigned int int3;
} __tbe_iqueue_node;

/* return TBE_OK if the 2 iqueue nodes are equal */
static int tbe_iqueue_cmp(const void *a_ptr1, const void *a_ptr2);

/* return TBE_OK if the 2 iqueue nodes are equal */
static int tbe_iqueue_cmp(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_iqueue_node *ptr1;
  __tbe_iqueue_node *ptr2;

  if (!(ptr1 = (__tbe_iqueue_node *) a_ptr1))
    return TBE_NULLPTR;
  if (!(ptr2 = (__tbe_iqueue_node *) a_ptr2))
    return TBE_NULLPTR;

  if (ptr1->int1 == ptr2->int1 &&
      ptr1->int2 == ptr2->int2 &&
      ptr1->int3 == ptr2->int3)
    return TBE_OK;

  return TBE_FAILURE;
}

/* create a new iqueue */
int tbe_iqueue_create(tbe_iqueue *a_iqueue)
{
  return tbe_list_create(a_iqueue);
}

/* destroy the queue */
void tbe_iqueue_destroy(tbe_iqueue *a_iqueue)
{
  tbe_list_destroy(a_iqueue, tbe_list_free);
}

/* purge the queue */
void tbe_iqueue_purge(tbe_iqueue a_iqueue)
{
  tbe_list_purge(a_iqueue, tbe_list_free);
}

/* add integers at the end of the queue */
int tbe_iqueue_enq(tbe_iqueue a_iqueue,
                   unsigned int a_int1,
                   unsigned int a_int2,
                   unsigned int a_int3)
{
  __tbe_iqueue_node *ptr;
  int retval;

  if (!a_iqueue)
    return TBE_NULLPTR;

  if (!(ptr = TBE_MEM_MALLOC(__tbe_iqueue_node, 1)))
    return TBE_MALLOCFAILED;

  ptr->int1 = a_int1;
  ptr->int2 = a_int2;
  ptr->int3 = a_int3;

  retval = tbe_list_find_data(a_iqueue, (void *) ptr, tbe_iqueue_cmp);

  switch (retval) {
    case TBE_OK :
      /* a node containing these values already exist */
      TBE_MEM_FREE(ptr);
      return TBE_OK;
    case TBE_NOTFOUND :
      /* no such node exists, so we add it */
      return tbe_list_add_tail(a_iqueue, (void *) ptr);
  }

  return retval;
}

/* remove integers from the front of the queue (and copy to given ptrs) */
int tbe_iqueue_deq(tbe_iqueue a_iqueue,
                   unsigned int *a_int1,
                   unsigned int *a_int2,
                   unsigned int *a_int3)
{
  __tbe_iqueue_node *ptr;
  int retval;

  if (!a_iqueue)
    return TBE_NULLPTR;

  if (!a_int1 && !a_int2 && !a_int3)
    return TBE_OK;

  if ((retval = tbe_list_get_head(a_iqueue, (void *) &ptr)) != TBE_OK)
    return retval;

  if (a_int1)
    *a_int1 = ptr->int1;
  if (a_int2)
    *a_int2 = ptr->int2;
  if (a_int3)
    *a_int3 = ptr->int3;

  return tbe_list_del_head(a_iqueue, tbe_list_free);
}
