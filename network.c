#include <stdlib.h>
#include <stdio.h>
#include "rel.h"
#include "network.h"

static void tbe_net_free(void *a_node);
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2);

static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2);
static int tbe_net_rlist_add(tbe_net_rlist *a_list,
                             unsigned int a_interval,
                             unsigned int a_relset);
static void tbe_net_rlist_dump(tbe_net_rlist a_list, FILE *a_stream);

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node)
{
  tbe_net_node *node = (tbe_net_node *) a_node;

  if (node && node->rlist) {
    tbe_list_purge(node->rlist, NULL);
    TBE_PTR_FREE(node->rlist);
  }
}

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2)
{
  tbe_net_node *ptr1 = (tbe_net_node *) a_ptr1;
  tbe_net_node *ptr2 = (tbe_net_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE);
}

/* return TBE_OK if the intervals of the 2 tbe_net_rlist_nodes are equal */
static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2)
{
  tbe_net_rlist_node *ptr1 = (tbe_net_rlist_node *) a_ptr1;
  tbe_net_rlist_node *ptr2 = (tbe_net_rlist_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE); 
}

/* add interval and relset into the relation list */
static int tbe_net_rlist_add(tbe_net_rlist *a_list,
                             unsigned int a_interval,
                             unsigned int a_relset)
{
  tbe_net_rlist_node *node;

  if (!(node = TBE_PTR_MALLOC(tbe_net_rlist_node, 1)))
    return TBE_MALLOCFAILED;

  node->interval = a_interval;
  node->relset = a_relset;

  return tbe_list_add(a_list, (void *) node);
}

/* dump contents of the relation list */
static void tbe_net_rlist_dump(tbe_net_rlist a_list, FILE *a_stream)
{
  unsigned int i;

  if (a_stream) {
    for (i = 0; i < tbe_list_length(a_list); i++) {
      void *node;
      tbe_list_get_index(a_list, i, &node);
      if (!node)
        continue;
      fprintf(a_stream, "  ");
      tbe_rel_set_dump(((tbe_net_rlist_node *) node)->relset, a_stream);
      fprintf(a_stream, "%03u\n", ((tbe_net_rlist_node *) node)->interval);
    }
  }
}

/* initialise a new net */
int tbe_net_init(tbe_net *a_net)
{
  return tbe_list_init(a_net);
}

/* destroy the given net */
void tbe_net_purge(tbe_net *a_net)
{
  tbe_list_purge(a_net, tbe_net_free);
}

/* add a new interval into the network */
int tbe_net_int_add(tbe_net *a_net, unsigned int a_int)
{
  tbe_net_node tmp;
  tbe_net_node *node;
  tbe_net_rlist *rlist;
  int retval;

  if (!a_net)
    return TBE_NULLPTR;

  /* check if interval is not in list yet */
  tmp.interval = a_int;
  if ((retval = tbe_list_find_data(*a_net, &tmp, tbe_net_cmp)) != TBE_NOTFOUND)
    return ((retval == TBE_OK) ? TBE_DUPLICATE : retval);

  /* allocate mem for new node */
  if (!(node = TBE_PTR_MALLOC(tbe_net_node, 1)))
    return TBE_MALLOCFAILED;

  if (!(rlist = TBE_PTR_MALLOC(tbe_net_rlist, 1))) {
    TBE_PTR_FREE(node);
    return TBE_MALLOCFAILED;
  }

  /* create a new, empty relation list */
  if ((retval = tbe_list_init(rlist)) != TBE_OK) {
    TBE_PTR_FREE(node);
    TBE_PTR_FREE(rlist);
    return retval;
  }

  /* populate the new node */
  node->interval = a_int;
  node->rlist = rlist;

  return tbe_list_add(a_net, (void *) node);
}

/* add a new relation to an existing interval */
int tbe_net_rel_add(tbe_net *a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_relset)
{
  tbe_net_node nnode;
  tbe_net_node *nptr;
  tbe_net_rlist_node rnode;
  tbe_net_rlist_node *rptr;
  void *ntmp = NULL;
  void *rtmp = NULL;
  int retval;

  if (!a_net)
    return TBE_NULLPTR;

  /* get a reference to the node containing interval 1 */
  nnode.interval = a_int1;
  retval = tbe_list_get_data_one(*a_net, (void *) &nnode, tbe_net_cmp, &ntmp);
  nptr = (tbe_net_node *) ntmp;

  if (retval != TBE_OK)
    return retval;

  /* now that we have a reference to the node containing interval 1,
   * we then check its relation list to see if interval 2 is already in it.
   */

  rnode.interval = a_int2;
  retval = tbe_list_get_data_one(*(nptr->rlist), (void *) &rnode, tbe_net_rlist_cmp, &rtmp);
  rptr = (tbe_net_rlist_node *) rtmp;

  switch (retval) {
    case TBE_NOTFOUND :
      /* interval2 not in list yet, so we have to add it */
      return tbe_net_rlist_add(nptr->rlist, a_int2, a_relset);
    case TBE_OK :
      /* interval2 already in list. for now we just replace the relset */
      rptr->relset = a_relset;
      return TBE_OK;
    default :
      return retval;
  }
}

/* print the network */
void tbe_net_dump(tbe_net a_net, FILE *a_stream)
{
  unsigned int i;

  if (a_stream) {
    for (i = 0; i < tbe_list_length(a_net); i++) {
      void *node;
      tbe_list_get_index(a_net, i, &node);
      if (!node)
        continue;
      fprintf(a_stream, "%03u\n", ((tbe_net_node *) node)->interval);
      tbe_net_rlist_dump(*(((tbe_net_node *) node)->rlist), a_stream);
      fprintf(a_stream, "\n");
    }
  } 
}
