#ifndef __TBE_IQUEUE_H
#define __TBE_IQUEUE_H

#include <tribe/tribe.h>
#include <tribe/list.h>

typedef tbe_list tbe_iqueue;

/* create a new iqueue */
int tbe_iqueue_create(tbe_iqueue *a_iqueue);

/* destroy the queue */
void tbe_iqueue_destroy(tbe_iqueue *a_iqueue);

/* purge the queue */
void tbe_iqueue_purge(tbe_iqueue a_iqueue);

/* add integers at the end of the queue */
int tbe_iqueue_enq(tbe_iqueue a_iqueue,
                   unsigned int a_int1,
                   unsigned int a_int2,
                   unsigned int a_int3);

/* remove integers from the front of the queue (and copy to given ptrs) */
int tbe_iqueue_deq(tbe_iqueue a_iqueue,
                   unsigned int *a_int1,
                   unsigned int *a_int2,
                   unsigned int *a_int3);

#endif
