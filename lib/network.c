#include <stdlib.h>
#include <stdio.h>
#include <tribe/rel.h>
#include <tribe/network.h>

/* queue to hold 2 intervals and a relation */
typedef struct {
  unsigned int interval1;
  unsigned int interval2;
  unsigned int relset;
} tbe_net_rqueue_node;

typedef tbe_list tbe_net_rqueue;

/* relation list for each interval */
typedef struct {
  unsigned int interval;
  unsigned int relset;
} tbe_net_rlist_node;

typedef tbe_list tbe_net_rlist;

/* return TBE_OK if the intervals of the 2 tbe_net_rlist_nodes are equal */
static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2);
/* enqueues a new node to the queue */
static int tbe_net_rqueue_enq(tbe_net_rqueue *a_q,
                               unsigned int a_int1,
                               unsigned int a_int2,
                               unsigned int a_rs);
/* dequeues a node from the queue */
static int tbe_net_rqueue_deq(tbe_net_rqueue *a_q,
                              unsigned int *a_int1,
                              unsigned int *a_int2,
                              unsigned int *a_rs);
/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2);
/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node);
/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net *a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_relset);

/* return TBE_OK if the intervals of the 2 tbe_net_rlist_nodes are equal */
static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2)
{
  tbe_net_rlist_node *ptr1 = (tbe_net_rlist_node *) a_ptr1;
  tbe_net_rlist_node *ptr2 = (tbe_net_rlist_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE); 
}

/* enqueues a new node to the queue */
static int tbe_net_rqueue_enq(tbe_net_rqueue *a_q,
                               unsigned int a_int1,
                               unsigned int a_int2,
                               unsigned int a_rs)
{
  tbe_net_rqueue_node *node = NULL;

  if (!a_q)
    return TBE_NULLPTR;

  if (!(node = TBE_PTR_MALLOC(tbe_net_rqueue_node, 1)))
    return TBE_MALLOCFAILED;

  node->interval1 = a_int1;
  node->interval2 = a_int2;
  node->relset = a_rs;

  return tbe_list_add_tail(a_q, (void *) node);
}

/* dequeues a node from the queue */
static int tbe_net_rqueue_deq(tbe_net_rqueue *a_q,
                              unsigned int *a_int1,
                              unsigned int *a_int2,
                              unsigned int *a_rs)
{
  tbe_net_rqueue_node *node = NULL;
  int retval;

  if (!a_q || !a_int1 || !a_int2 || !a_rs)
    return TBE_NULLPTR;

  if ((retval = tbe_list_get_head(*a_q, (void *) &node)) != TBE_OK)
    return retval;

  if (!node)
    return TBE_FAILURE;

  *a_int1 = node->interval1;
  *a_int2 = node->interval2;
  *a_rs = node->relset;

  return tbe_list_del_head(a_q, NULL);
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

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node)
{
  tbe_net_node *node = (tbe_net_node *) a_node;

  if (node && node->rlist) {
    tbe_list_purge(node->rlist, NULL);
    TBE_PTR_FREE(node->rlist);
  }
}

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net *a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_relset)
{
  tbe_net_node nnode;
  tbe_net_node *nptr = NULL;
  tbe_net_rlist_node rnode;
  tbe_net_rlist_node *rptr = NULL;
  unsigned int relset;
  int retval;

  if (!a_net)
    return TBE_NULLPTR;

  /* firstly, we check whether we are trying to add some trivial info */
  if (a_int1 == a_int2)
    return TBE_OK;

  /* we then figure out what relset to store. the relset itself will be
   * stored in the smaller interval. if the intervals were given in the
   * right order (ie int1 <= int2) then we store the relset as it is. if given
   * in reverse order, we have to store the inverse of the relset */
  if (TBE_INT_MIN(a_int1, a_int2) == a_int1)
    relset = a_relset;
  else
    relset = tbe_rel_set_inverse(a_relset);

  /* get a reference of the node containing the smaller of the 2 intervals */
  nnode.interval = TBE_INT_MIN(a_int1, a_int2);
  if ((retval = tbe_list_get_data_one(*a_net, (void *) &nnode, tbe_net_cmp, (void *) &nptr)) != TBE_OK)
    return retval;

  /* now that we have a reference to the node containing the smaller
   * interval, we then check its relation list to see if the other interval
   * is already in it. */
  rnode.interval = TBE_INT_MAX(a_int1, a_int2);
  switch ((retval = tbe_list_get_data_one(*(nptr->rlist), (void *) &rnode, tbe_net_rlist_cmp, (void *) &rptr))) {
    case TBE_NOTFOUND : {
      /* larger interval  not in the list yet, so we have to add it */

      if (!(rptr = TBE_PTR_MALLOC(tbe_net_rlist_node, 1)))
        return TBE_MALLOCFAILED;

      rptr->interval = TBE_INT_MAX(a_int1, a_int2);
      rptr->relset = relset;

      return tbe_list_add_tail(nptr->rlist, (void *) rptr);
    }
    case TBE_OK :
      /* larger interval already in the list */
      if (TBE_REL_SET_ISFILL(relset)) {
        /* new relation contains all possible relations! remove it */
        return tbe_list_del_data(nptr->rlist,
                                 (void *) &rnode,
                                 tbe_net_rlist_cmp,
                                 NULL);
      }
      /* replace the relset with the new one */
      rptr->relset = relset;
      return TBE_OK;
    default :
      return retval;
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
int tbe_net_add_int(tbe_net *a_net, unsigned int a_int)
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

  return tbe_list_add_tail(a_net, (void *) node);
}

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_rel(tbe_net *a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_relset)
{
  int retval = TBE_OK;
  unsigned int rs;
  tbe_net_rqueue rqueue;

  if (!a_net)
    return TBE_NULLPTR;

  /* get intersection of what is already in the network and what is given */
  rs = TBE_REL_SET_INTERSECT(tbe_net_rel(*a_net, a_int1, a_int2), a_relset);
  if (TBE_REL_SET_ISCLEAR(rs))
    return TBE_INVALIDINPUT;

  /* intialise and load the queue */
  if ((retval = tbe_list_init(&rqueue)) != TBE_OK)
    return retval;
  if ((retval = tbe_net_rqueue_enq(&rqueue, a_int1, a_int2, rs)) != TBE_OK)
    return retval;

  while (tbe_list_length(rqueue) && retval == TBE_OK) {
    unsigned int i;
    unsigned int int1q;
    unsigned int int2q;
    unsigned int rsq;

    /* get relation from queue */
    if ((retval = tbe_net_rqueue_deq(&rqueue, &int1q, &int2q, &rsq)) != TBE_OK)
      break;

    /* now we add this new relation in the network */
    if ((retval = tbe_net_add_rel_noprop(a_net, int1q, int2q, rsq)) != TBE_OK)
      break;
 
    /* go through all intervals k that is not int1 and int2 */
    for (i = 0; i < tbe_list_length(*a_net) && retval == TBE_OK; i++) {
      unsigned int rs1;
      unsigned int rs2;
      unsigned int rs3;
      tbe_net_node *nnode = NULL;
 
      if ((retval = tbe_list_get_index(*a_net, i, (void *) &nnode)) != TBE_OK)
        break;
 
      if (nnode->interval == int1q || nnode->interval == int2q)
        continue;

      /* find r(k,j) given r1(k,i) and r2(i,j) */
      rs1 = tbe_net_rel(*a_net, nnode->interval, int1q);
      rs2 = tbe_net_rel(*a_net, nnode->interval, int2q);
      if (TBE_REL_SET_ISCLEAR(rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(rs1, rsq)))) {
        retval = TBE_FAILURE;
        break;
      }

      /* we only add if rs3 is more specific than rs2 */
      if (rs2 != rs3)
        if ((retval = tbe_net_rqueue_enq(&rqueue, nnode->interval, int2q, rs3)) != TBE_OK)
          break;
 
      /* find r(i,k), given r1(i,j), r2(j,k) */
      rs1 = tbe_net_rel(*a_net, int2q, nnode->interval);
      rs2 = tbe_net_rel(*a_net, int1q, nnode->interval);
      if (TBE_REL_SET_ISCLEAR(rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(rsq, rs1)))) {
        retval = TBE_FAILURE;
        break;
      }

      /* we only add if rs3 is more specific than rs2 */
      if (rs2 != rs3)
        if ((retval = tbe_net_rqueue_enq(&rqueue, int1q, nnode->interval, rs3)) != TBE_OK)
          break;
    }
  }

  tbe_list_purge(&rqueue, NULL);
  return retval;
}

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_rel(tbe_net a_net,
                         unsigned int a_int1,
                         unsigned int a_int2)
{
  tbe_net_node nnode;
  tbe_net_node *nptr = NULL;
  tbe_net_rlist_node rnode;
  tbe_net_rlist_node *rptr = NULL;
  unsigned int relset;

  TBE_REL_SET_CLEAR(relset);

  /* first, we eliminate the trivial case */
  if (a_int1 == a_int2) {
    TBE_REL_SET_ADD(relset, TBE_REL_EQL);
    return relset;
  }

  /* get a reference of the node containing the smaller of the 2 intervals */
  nnode.interval = TBE_INT_MIN(a_int1, a_int2);
  if (tbe_list_get_data_one(a_net, (void *) &nnode, tbe_net_cmp, (void *) &nptr) != TBE_OK)
    return relset;

  /* then we check if the larger interval exists */
  nnode.interval = TBE_INT_MAX(a_int1, a_int2);
  if (tbe_list_find_data(a_net, (void *) &nnode, tbe_net_cmp) != TBE_OK)
    return relset;

  /* now we look for the larger interval in the rel list of the first */
  rnode.interval = TBE_INT_MAX(a_int1, a_int2);
  switch (tbe_list_get_data_one(*(nptr->rlist), (void *) &rnode, tbe_net_rlist_cmp, (void *) &rptr)) {
    case TBE_NOTFOUND :
      /* interval2 not in the list, so return a full relset */
      TBE_REL_SET_FILL(relset);
      return relset;
    case TBE_OK :
      /* found it. now we determine whether we need to find the inverse */
      if (TBE_INT_MIN(a_int1, a_int2) == a_int1)
        return rptr->relset;
      else
        return tbe_rel_set_inverse(rptr->relset); 
    default :
      return relset;
  }

  return relset;
}

/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream)
{
  unsigned int i;

  if (!a_stream) 
    return;

  for (i = 0; i < tbe_list_length(a_net); i++) {
    unsigned int j;
    tbe_net_node *nnode = NULL;

    /* retrieve i'th net node */
    tbe_list_get_index(a_net, i, (void *) &nnode);

    if (!nnode)
      continue;

    /* if the interval doesn't have any rels, just dump the interval */
    if (tbe_list_length(*(nnode->rlist)) == 0) {
      fprintf(a_stream, "%03u\n", nnode->interval);
      continue;
    }

    /* now get this interval's rel list */
    for (j = 0; j < tbe_list_length(*(nnode->rlist)); j++) {
      tbe_net_rlist_node *rnode = NULL;

      /* print interval 1 */
      fprintf(a_stream, "%03u ", nnode->interval);

      /* get j'th rel node */
      tbe_list_get_index(*(nnode->rlist), j, (void *) &rnode);

      /* dump the relset of these nodes */
      if (rnode)
        tbe_rel_set_dump(rnode->relset, a_stream);

      /* print interval 2 */
      fprintf(a_stream, "%03u\n", rnode->interval);
    }
  }
}

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream)
{
  unsigned int i;
  unsigned int j;
  unsigned int int1;
  unsigned int int2;
  tbe_net_node *nnode = NULL;

  if (!a_stream)
    return;

  for (i = 0; i < tbe_list_length(a_net); i++) {
    /* first interval */
    tbe_list_get_index(a_net, i, (void *) &nnode);
    int1 = nnode->interval;

    for (j = 0; j < tbe_list_length(a_net); j++) {
      /* second interval */
      tbe_list_get_index(a_net, j, (void *) &nnode);
      int2 = nnode->interval;

      /* print interval 1 */
      fprintf(a_stream, "%03u ", int1);

      /* the relset */
      tbe_rel_set_dump(tbe_net_rel(a_net, int1, int2), a_stream);

      /* print interval 2 */
      fprintf(a_stream, "%03u\n", int2);
    }
  }
}
