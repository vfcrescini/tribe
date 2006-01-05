#ifndef __TBE_RQUEUE_H
#define __TBE_RQUEUE_H

#include <tribe/tribe.h>
#include <tribe/list.h>

typedef tbe_list tbe_rqueue;

/* create a new rqueue */
TBE_EXTERN int tbe_rqueue_create(tbe_rqueue *a_rqueue);

/* destroy the queue */
TBE_EXTERN void tbe_rqueue_destroy(tbe_rqueue *a_rqueue);

/* purge the queue */
TBE_EXTERN void tbe_rqueue_purge(tbe_rqueue a_rqueue);

/* add rel at the end of the queue (rels get normalised) */
TBE_EXTERN int tbe_rqueue_enq(tbe_rqueue a_rqueue,
                              unsigned int a_id1,
                              unsigned int a_id2,
                              unsigned char a_type1,
                              unsigned char a_type2,
                              unsigned int a_rs);

/* remove rel from the front of the queue (and copy to given ptrs) */
TBE_EXTERN int tbe_rqueue_deq(tbe_rqueue a_rqueue,
                              unsigned int *a_id1,
                              unsigned int *a_id2,
                              unsigned char *a_type1,
                              unsigned char *a_type2,
                              unsigned int *a_rs);

#endif
