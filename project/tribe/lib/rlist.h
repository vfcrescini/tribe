#ifndef __TBE_RLIST_H
#define __TBE_RLIST_H

#include <tribe/tribe.h>
#include <tribe/list.h>

typedef tbe_list tbe_rlist;

/* create a new rlist */
int tbe_rlist_create(tbe_rlist *a_rlist);

/* destroy the rlist */
void tbe_rlist_destroy(tbe_rlist *a_rlist);

/* purge the rlist */
void tbe_rlist_purge(tbe_rlist a_rlist);

/* add to the list. if a_rs contains all rels, it is removed (or not added) */
int tbe_rlist_add(tbe_rlist a_rlist, unsigned int a_int, unsigned int a_rs);

/* gives the rs of the given interval. if a_int isn't in, a_rs is set to all */
int tbe_rlist_get(tbe_rlist a_rlist, unsigned int a_int, unsigned int *a_rs);

/* dumps the list, each printed with a_int */
int tbe_rlist_dump(tbe_rlist a_rlist, unsigned int a_int, FILE *a_stream);

#endif