#ifndef __TBE_RQUEUE_H
#define __TBE_RQUEUE_H

#include <tribe/tribe.h>
#include <tribe/list.h>
#include <tribe/rel.h>

typedef tbe_list tbe_rqueue;

/* create a new rqueue */
int tbe_rqueue_create(tbe_rqueue *a_rqueue);

/* destroy the queue */
void tbe_rqueue_destroy(tbe_rqueue *a_rqueue);

/* purge the queue */
void tbe_rqueue_purge(tbe_rqueue a_rqueue);

/* add rel at the end of the queue */
int tbe_rqueue_enq1(tbe_rqueue a_rqueue,
                    unsigned int a_id1,
                    unsigned int a_id2,
                    unsigned int a_rs);

/* as above, but with a tbe_rel structure */
int tbe_rqueue_enq2(tbe_rqueue a_rqueue, tbe_rel a_rel);

/* remove rel from the front of the queue (and copy to given ptrs) */
int tbe_rqueue_deq1(tbe_rqueue a_rqueue,
                    unsigned int *a_id1,
                    unsigned int *a_id2,
                    unsigned int *a_rs);

/* as above, but with a tbe_rel structure */
int tbe_rqueue_deq2(tbe_rqueue a_rqueue, tbe_rel *a_rel);

#endif
