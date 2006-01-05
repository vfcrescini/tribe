#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include <tribe/tribe.h>
#include <tribe/list.h>
#include <tribe/clist.h>
#include <tribe/interval.h>

typedef void *tbe_net;

/* create a new network */
TBE_EXTERN int tbe_net_create(tbe_net *a_net);

/* destroy the given network */
TBE_EXTERN void tbe_net_destroy(tbe_net *a_net);

/* add a new interval into the network */
TBE_EXTERN int tbe_net_add_interval(tbe_net a_net, unsigned int a_id);

/* add a new relation to an existing interval, also propagate the relation */
TBE_EXTERN int tbe_net_add_relation(tbe_net a_net,
                                    unsigned int a_id1,
                                    unsigned int a_id2,
                                    unsigned int a_rs);

/* bind the given (existing) interval with the given endpoints */
TBE_EXTERN int tbe_net_add_endpoints(tbe_net a_net,
                                     unsigned int a_id,
                                     tbe_interval a_int);

/* returns the rel set between the given two intervals in the given network */
TBE_EXTERN unsigned int tbe_net_get_relation1(tbe_net a_net,
                                              unsigned int a_id1,
                                              unsigned int a_id2);

/* returns the rel set between the given interval and endpoints */
TBE_EXTERN unsigned int tbe_net_get_relation2(tbe_net a_net,
                                              unsigned int a_id,
                                              tbe_interval a_interval);

/* generates a list of intervals whose relation with a_id is a subset of rs,
 * then call a_fn for each interval generated */
TBE_EXTERN int tbe_net_get_intervals(tbe_net a_net,
                                     unsigned int a_id,
                                     unsigned int a_rs,
                                     int (*a_fn)(unsigned int, void *),
                                     void *a_parm);

/* generates a list of tuples (set of intervals) of size a_num_vars +
 * a_num_lits that satisfies the constraints in a_clist, then calls a_fn()
 * for each tuple, passing the tuple, its size and the given parameter.
 * a_arr_lits contains intervals that correspond to the first a_num_lits
 * variables in a_clist. */
TBE_EXTERN int tbe_net_get_tuples(tbe_net a_net,
                                  unsigned int a_num_vars,
                                  unsigned int a_num_lits,
                                  unsigned int a_arr_lits[],
                                  tbe_clist a_clist,
                                  int (*a_fn)(unsigned int[], unsigned int, void *),
                                  void *a_parm);

/* print the network as it is stored physically */
TBE_EXTERN void tbe_net_dump1(tbe_net a_net, FILE *a_stream);

/* print the network as it is conceptually */
TBE_EXTERN void tbe_net_dump2(tbe_net a_net, FILE *a_stream);

#endif
