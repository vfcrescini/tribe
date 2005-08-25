#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include <tribe/tribe.h>
#include <tribe/list.h>
#include <tribe/interval.h>

typedef tbe_list tbe_net;

/* create a new network */
int tbe_net_create(tbe_net *a_net);

/* destroy the given network */
void tbe_net_destroy(tbe_net *a_net);

/* normalise the relation. a relation A rs B is normalised if A <= B */
int tbe_net_normalise(unsigned int *a_int_id1,
                      unsigned int *a_int_id2,
                      unsigned int *a_rs);

/* add a new interval into the network */
int tbe_net_add_int(tbe_net a_net, unsigned int a_int_id);

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_rel(tbe_net a_net,
                    unsigned int a_int_id1,
                    unsigned int a_int_id2,
                    unsigned int a_rs);

/* bind the given (existing) interval with the given endpoints */
int tbe_net_add_ep(tbe_net a_net, unsigned int a_int_id, tbe_interval a_int);

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_rel(tbe_net a_net,
                         unsigned int a_int_id1,
                         unsigned int a_int_id2);

/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream);

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream);

#endif
