#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include "tribe.h"
#include "list.h"

/* relation list for each interval */
typedef struct {
  unsigned int interval;
  unsigned int relset;
} tbe_net_rlist_node;

typedef tbe_list tbe_net_rlist;

/* the network */
typedef struct {
  unsigned int interval;
  tbe_net_rlist *rlist;
} tbe_net_node;

typedef tbe_list tbe_net;

/* initialise a new net */
int tbe_net_init(tbe_net *a_net);
/* destroy the given net */
void tbe_net_purge(tbe_net *a_net);
/* add a new interval into the network */
int tbe_net_int_add(tbe_net *a_net, unsigned int a_int);
/* add a new relation to an existing interval */
int tbe_net_rel_add(tbe_net *a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_relset);
/* print the network */
void tbe_net_dump(tbe_net a_net, FILE *a_stream);

#endif
