#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include "tribe.h"
#include "list.h"

typedef struct {
  unsigned int interval;
  unsigned int relset;
} tbe_net_rel_list_node;

typedef tbe_list tbe_net_rel_list;

typedef struct {
  unsigned int interval;
  tbe_net_rel_list *rellist;
} tbe_net_list_node;

typedef tbe_list tbe_net_list;

int tbe_net_rel_list_init(tbe_net_rel_list *a_list);
void tbe_net_rel_list_purge(tbe_net_rel_list *a_list);
int tbe_net_rel_list_add(tbe_net_rel_list *a_list,
                         unsigned int a_interval,
                         unsigned int a_relset);
void tbe_net_rel_list_dump(tbe_net_rel_list a_list, FILE *a_stream);
int tbe_net_list_init(tbe_net_list *a_list);
void tbe_net_list_purge(tbe_net_list *a_list);
int tbe_net_list_add(tbe_net_list *a_list,
                     unsigned int a_interval,
                     tbe_net_rel_list *a_rellist);
void tbe_net_list_dump(tbe_net_list a_list, FILE *a_stream);

#endif
