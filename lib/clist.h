#ifndef __TBE_CLIST_H
#define __TBE_CLIST_H

#include <tribe/tribe.h>

typedef void *tbe_clist;

/* create a new clist */
int tbe_clist_create(tbe_clist *a_clist);

/* destroy the clist */
void tbe_clist_destroy(tbe_clist *a_clist);

/* purge the clist */
void tbe_clist_purge(tbe_clist a_clist);

/* add constraint */
int tbe_clist_add(tbe_clist a_clist,
                  unsigned int a_v1,
                  unsigned int a_v2,
                  unsigned int a_rs);

/* check that all variables in the list are under the given maximum */
int tbe_clist_verify(tbe_clist a_clist, unsigned int a_max);

/* return TBE_OK if the given tuple of the given size satisfies the given
 * clist, TBE_FAILURE if it doesn't, but no other error occurs. calls a_fn()
 * with the given parameter to get the relset of 2 intervals. each variable in
 * the clist is an index of the tuple (minus the offset). important: assumes
 * a_tuple is of size a_size. */
int tbe_clist_validate(tbe_clist a_clist,
                       unsigned int *a_tuple,
                       unsigned int a_size,
                       unsigned int a_offset,
                       unsigned int (*a_fn)(unsigned int, unsigned int, void *),
                       void *a_parm);

#endif
