#ifndef __TBE_RLIST_H
#define __TBE_RLIST_H

#ifdef __cplusplus
  #include <cstdio>
#else
  #include <stdio.h>
#endif

#include <tribe/tribe.h>
#include <tribe/list.h>

typedef tbe_list tbe_rlist;

/* create a new rlist */
TBE_EXTERN int tbe_rlist_create(tbe_rlist *a_rlist);

/* destroy the rlist */
TBE_EXTERN void tbe_rlist_destroy(tbe_rlist *a_rlist);

/* purge the rlist */
TBE_EXTERN void tbe_rlist_purge(tbe_rlist a_rlist);

/* add to the list. if a_rs contains all rels, it is removed (or not added) */
TBE_EXTERN int tbe_rlist_add(tbe_rlist a_rlist,
                             unsigned int a_id,
                             unsigned char a_type,
                             unsigned int a_rs);

/* gives the rs. if the interval is not in, a_rs is set to all */
TBE_EXTERN int tbe_rlist_get(tbe_rlist a_rlist,
                             unsigned int a_id,
                             unsigned char a_type,
                             unsigned int *a_rs);

/* dumps the list, each prefixed with an interval */
TBE_EXTERN int tbe_rlist_dump(tbe_rlist a_rlist,
                              unsigned int a_id,
                              unsigned char a_type,
                              FILE *a_stream);

#endif
