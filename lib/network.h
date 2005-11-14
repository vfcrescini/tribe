#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include <tribe/tribe.h>
#include <tribe/list.h>
#include <tribe/interval.h>

typedef void *tbe_net;

/* create a new network */
int tbe_net_create(tbe_net *a_net);

/* destroy the given network */
void tbe_net_destroy(tbe_net *a_net);

/* add a new interval into the network */
int tbe_net_add_interval(tbe_net a_net, unsigned int a_id);

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_relation(tbe_net a_net,
                         unsigned int a_id1,
                         unsigned int a_id2,
                         unsigned int a_rs);

/* bind the given (existing) interval with the given endpoints */
int tbe_net_add_endpoints(tbe_net a_net,
                          unsigned int a_id,
                          tbe_interval a_int);

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_get_relation1(tbe_net a_net,
                                   unsigned int a_id1,
                                   unsigned int a_id2);

/* returns the rel set between the given interval and endpoints */
unsigned int tbe_net_get_relation2(tbe_net a_net,
                                   unsigned int a_id,
                                   tbe_interval a_interval);

/* generates a list of intervals whose relation with a_id is a subset of rs,
 * then call a_fn for each interval generated */
int tbe_net_get_intervals(tbe_net a_net,
                          unsigned int a_id,
                          unsigned int a_rs,
                          int (*a_fn)(unsigned int, void *),
                          void *a_parm);

/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream);

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream);

#endif
