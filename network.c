#include <stdlib.h>
#include <stdio.h>
#include "rel.h"
#include "network.h"

static void tbe_net_list_free(void *a_node)
{
  tbe_net_list_node *node = (tbe_net_list_node *) a_node;

  if (node)
    tbe_net_rel_list_purge(node->rellist);
}

int tbe_net_rel_list_init(tbe_net_rel_list *a_list)
{
  return tbe_list_init(a_list);
}

void tbe_net_rel_list_purge(tbe_net_rel_list *a_list)
{
  tbe_list_purge(a_list, NULL);
}

int tbe_net_rel_list_add(tbe_net_rel_list *a_list,
                         unsigned int a_interval,
                         unsigned int a_relset)
{
  tbe_net_rel_list_node *node;

  if (!(node = TBE_PTR_MALLOC(tbe_net_rel_list_node, 1)))
    return TBE_MALLOCFAILED;

  node->interval = a_interval;
  node->relset = a_relset;

  return tbe_list_add(a_list, (void *) node);
}

void tbe_net_rel_list_dump(tbe_net_rel_list a_list, FILE *a_stream)
{
  unsigned int i;

  if (a_stream) {
    for (i = 0; i < tbe_list_length(a_list); i++) {
      void *node;
      tbe_list_get_index(a_list, i, &node);
      if (!node)
        continue;
      fprintf(a_stream, "  ");
      tbe_rel_set_dump(((tbe_net_rel_list_node *) node)->relset, a_stream);
      fprintf(a_stream, "%03u\n", ((tbe_net_rel_list_node *) node)->interval);
    }
  }
}

int tbe_net_list_init(tbe_net_list *a_list)
{
  return tbe_list_init(a_list);
}

void tbe_net_list_purge(tbe_net_list *a_list)
{
  tbe_list_purge(a_list, tbe_net_list_free);
}

int tbe_net_list_add(tbe_net_list *a_list,
                     unsigned int a_interval,
                     tbe_net_rel_list *a_rellist)
{
  tbe_net_list_node *node;

  if (!a_rellist)
    return TBE_NULLPTR;

  if (!(node = TBE_PTR_MALLOC(tbe_net_list_node, 1)))
    return TBE_MALLOCFAILED;

  node->interval = a_interval;
  node->rellist = a_rellist;

  return tbe_list_add(a_list, (void *) node);
}

void tbe_net_list_dump(tbe_net_list a_list, FILE *a_stream)
{
  unsigned int i;

  if (a_stream) {
    for (i = 0; i < tbe_list_length(a_list); i++) {
      void *node;
      tbe_list_get_index(a_list, i, &node);
      if (!node)
        continue;
      fprintf(a_stream, "%03u\n", ((tbe_net_list_node *) node)->interval);
      tbe_net_rel_list_dump(*(((tbe_net_list_node *) node)->rellist), a_stream);
      fprintf(a_stream, "\n");

    }
  } 
}
