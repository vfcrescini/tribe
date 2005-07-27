#include <stdlib.h>
#include <stdio.h>
#include <tribe/rel.h>
#include <tribe/mem.h>
#include <tribe/network.h>
#include <tribe/iqueue.h>

#define TBE_NET_SKIP(X,Y) \
  ( \
    TBE_REL_SET_ISCLEAR(X) || \
    TBE_REL_SET_ISCLEAR(Y) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_BEF) && TBE_REL_SET_ISIN(Y,TBE_REL_BEI)) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_BEI) && TBE_REL_SET_ISIN(Y,TBE_REL_BEF)) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_DUR) && TBE_REL_SET_ISIN(Y,TBE_REL_DUI)) \
  )

/* relation list for each interval */
typedef struct {
  unsigned int interval;
  unsigned int relset;
} __tbe_net_rlist_node;

typedef tbe_list __tbe_net_rlist;

/* the network */
typedef struct {
  unsigned int interval;
  tbe_list rlist;
} __tbe_net_node;

/* return TBE_OK if the intervals of the 2 tbe_net_rlist_nodes are equal */
static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2);

/* deletes a_int from the rlist */
static int tbe_net_rlist_del(__tbe_net_rlist a_rlist, unsigned int a_int);

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_rlist_node *tbe_net_get_rlist_ref(__tbe_net_rlist a_rlist,
                                                   unsigned int a_int);

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2);

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(tbe_net a_net, unsigned int a_int);

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node);

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_relset);

/* return TBE_OK if the intervals of the 2 tbe_net_rlist_nodes are equal */
static int tbe_net_rlist_cmp(void *a_ptr1, void *a_ptr2)
{
  __tbe_net_rlist_node *ptr1 = (__tbe_net_rlist_node *) a_ptr1;
  __tbe_net_rlist_node *ptr2 = (__tbe_net_rlist_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE); 
}

/* deletes a_int from the rlist */
static int tbe_net_rlist_del(__tbe_net_rlist a_rlist, unsigned int a_int)
{
  __tbe_net_rlist_node rnode;

  rnode.interval = a_int;

  return tbe_list_del_data(a_rlist,
                           (void *) &rnode,
                           tbe_net_rlist_cmp,
                           tbe_list_free);
}

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_rlist_node *tbe_net_get_rlist_ref(__tbe_net_rlist a_rlist,
                                                   unsigned int a_int)
{
  __tbe_net_rlist_node node;
  __tbe_net_rlist_node *nptr;
  int retval;

  node.interval = a_int;
  retval = tbe_list_get_data_one(a_rlist,
                                 (void *) &node,
                                 tbe_net_rlist_cmp,
                                 (void *) &nptr);
  if (retval == TBE_OK)
    return nptr;

  return NULL;
}

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2)
{
  __tbe_net_node *ptr1 = (__tbe_net_node *) a_ptr1;
  __tbe_net_node *ptr2 = (__tbe_net_node *) a_ptr2;

  if (!ptr1 || !ptr2)
    return TBE_NULLPTR;

  return ((ptr1->interval == ptr2->interval) ? TBE_OK : TBE_FAILURE);
}

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(tbe_net a_net, unsigned int a_int)
{
  __tbe_net_node node;
  __tbe_net_node *nptr;
  int retval;

  node.interval = a_int;

  retval = tbe_list_get_data_one(a_net,
                                 (void *) &node,
                                 tbe_net_cmp,
                                 (void *) &nptr);

  if (retval == TBE_OK)
    return nptr;

  return NULL;
}

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_nptr)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_nptr;

  if (nptr) {
    if (nptr->rlist)
      tbe_list_destroy(&(nptr->rlist), tbe_list_free);

    TBE_MEM_FREE(nptr);
  }
}

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_relset)
{
  __tbe_net_node *nptr;
  __tbe_net_rlist_node *rptr;

  if (!a_net)
    return TBE_NULLPTR;

  /* firstly, we check whether we are trying to add something trivial */
  if (a_int1 == a_int2)
    return TBE_OK;

  /* make sure relation is already normalised */
  if (a_int1 > a_int2)
    return TBE_INVALIDINPUT;

  /* get a reference of the node containing the first interval */
  if (!(nptr = tbe_net_get_ref(a_net, a_int1)))
    return TBE_INVALIDINPUT;

  /* check if the second interval exists */
  if (!tbe_net_get_ref(a_net, a_int2))
    return TBE_INVALIDINPUT;

  /* now check if the second interval is in the rlist of the first */
  if ((rptr = tbe_net_get_rlist_ref(nptr->rlist, a_int2))) {
    /* second interval already in the rlist, now check if the new rel contains
     * all possible rels. if so, we simply remove it */
    if (TBE_REL_SET_ISFILL(a_relset))
      return tbe_net_rlist_del(nptr->rlist, a_int2);

    /* replace the relset with the new one */
    rptr->relset = a_relset;

    return TBE_OK;
  }
  else {
    /* second interval not in the rlist yet */

    if (TBE_REL_SET_ISFILL(a_relset))
      return TBE_OK;
  
    /* we only add if the relset is not all relations */
    if (!(rptr = TBE_MEM_MALLOC(__tbe_net_rlist_node, 1)))
      return TBE_MALLOCFAILED;

    rptr->interval = a_int2;
    rptr->relset = a_relset;

    return tbe_list_add_tail(nptr->rlist, (void *) rptr);
  }
}

/* create a new network */
int tbe_net_create(tbe_net *a_net)
{
  return tbe_list_create((tbe_list *) a_net);
}

/* destroy the given net */
void tbe_net_destroy(tbe_net *a_net)
{
  tbe_list_destroy(a_net, tbe_net_free);
}

/* normalise the relation. a relation A rs B is normalised if A <= B */
int tbe_net_normalise(unsigned int *a_int1,
                      unsigned int *a_int2,
                      unsigned int *a_relset)
{
  unsigned int a;
  unsigned int b;

  if (!a_int1 || !a_int2 || !a_relset)
    return TBE_NULLPTR;

  a = *a_int1;
  b = *a_int2;

  *a_int1 = TBE_INT_MIN(a, b);
  *a_int2 = TBE_INT_MAX(a, b);

  if (a != *a_int1)
    *a_relset = tbe_rel_set_inverse(*a_relset);

  return TBE_OK;
}

/* add a new interval into the network */
int tbe_net_add_int(tbe_net a_net, unsigned int a_int)
{
  __tbe_net_node *nptr;
  __tbe_net_rlist rlist;
  int retval;

  if (!a_net)
    return TBE_NULLPTR;

  /* check if interval is not in list yet */
  if (tbe_net_get_ref(a_net, a_int))
    return TBE_DUPLICATE;

  /* allocate mem for new node */
  if (!(nptr = TBE_MEM_MALLOC(__tbe_net_node, 1)))
    return TBE_MALLOCFAILED;

  /* create a new empty rlist */
  if ((retval = tbe_list_create(&rlist)) != TBE_OK) {
    TBE_MEM_FREE(nptr);
    return retval;
  }

  /* populate the new node */
  nptr->interval = a_int;
  nptr->rlist = rlist;

  if ((retval = tbe_list_add_tail(a_net, (void *) nptr)) != TBE_OK) {
    TBE_MEM_FREE(nptr);
    tbe_list_destroy(&rlist, tbe_list_free);
  }

  return retval;
}

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_rel(tbe_net a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_relset)
{
  int retval;
  unsigned int rs;
  tbe_iqueue iqueue;

  if (!a_net)
    return TBE_NULLPTR;

  /* check if the relset to be added contains all possible relations */
  if (TBE_REL_SET_ISFILL(a_relset))
    return TBE_OK;

  /* get intersection of what is already in the network and what is given */
  rs = TBE_REL_SET_INTERSECT(tbe_net_rel(a_net, a_int1, a_int2), a_relset);

  if (TBE_REL_SET_ISCLEAR(rs))
    return TBE_INVALIDINPUT;

  /* intialise and load the queue */
  if ((retval = tbe_iqueue_create(&iqueue)) != TBE_OK)
    return retval;

  if ((retval = tbe_iqueue_enq(iqueue, a_int1, a_int2, rs)) != TBE_OK)
    return retval;

  retval = TBE_OK;

  while (tbe_list_length(iqueue) && retval == TBE_OK) {
    unsigned int i;
    unsigned int int1q;
    unsigned int int2q;
    unsigned int rsq;

    /* get relation from queue */
    if ((retval = tbe_iqueue_deq(iqueue, &int1q, &int2q, &rsq)) != TBE_OK)
      break;

    tbe_net_normalise(&int1q, &int2q, &rsq);

    /* now we add this new relation to the network */
    if ((retval = tbe_net_add_rel_noprop(a_net, int1q, int2q, rsq)) != TBE_OK)
      break;
 
    /* go through all intervals k that is not int1 and int2 */
    for (i = 0; i < tbe_list_length(a_net) && retval == TBE_OK; i++) {
      unsigned int rs1;
      unsigned int rs2;
      unsigned int rs3;
      __tbe_net_node *nptr;
 
      if ((retval = tbe_list_get_index(a_net, i, (void *) &nptr)) != TBE_OK)
        break;
 
      if (nptr->interval == int1q || nptr->interval == int2q)
        continue;

      /* find rs(k,j), given rs(k,i) and rs(i,j) */
      rs1 = tbe_net_rel(a_net, nptr->interval, int1q);

      if (!TBE_NET_SKIP(rs1, rsq)) {
        rs2 = tbe_net_rel(a_net, nptr->interval, int2q);
        rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(rs1, rsq));

        /* if the intersection of "what is in the network" and "what we
         * have concluded is the empty set, something is wrong */
        if (TBE_REL_SET_ISCLEAR(rs3)) {
          retval = TBE_FAILURE;
          break;
        }

        if (rs2 != rs3) {
          retval = tbe_iqueue_enq(iqueue, nptr->interval, int2q, rs3);
          if (retval != TBE_OK)
            break;
        }
      }

      /* find rs(i,k), given rs(i,j), rs(j,k) */
      rs1 = tbe_net_rel(a_net, int2q, nptr->interval);

      if (!TBE_NET_SKIP(rsq, rs1)) {
        rs2 = tbe_net_rel(a_net, int1q, nptr->interval);
        rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(rsq, rs1));

        /* if the intersection of "what is in the network" and "what we
         * have concluded is the empty set, something is wrong */
        if (TBE_REL_SET_ISCLEAR(rs3)) {
          retval = TBE_FAILURE;
          break;
        }

        if (rs2 != rs3) {
          retval = tbe_iqueue_enq(iqueue, int1q, nptr->interval, rs3);
          if (retval != TBE_OK)
            break;
        }
      }
    }

    if (retval != TBE_OK)
      break;
  }

  tbe_iqueue_destroy(&iqueue);

  return retval;
}

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_rel(tbe_net a_net,
                         unsigned int a_int1,
                         unsigned int a_int2)
{
  __tbe_net_node *nptr;
  __tbe_net_rlist_node *rptr;
  unsigned int int1 = TBE_INT_MIN(a_int1, a_int2);
  unsigned int int2 = TBE_INT_MAX(a_int1, a_int2);
  unsigned int relset;

  TBE_REL_SET_CLEAR(relset);

  /* first, we eliminate the trivial case */
  if (a_int1 == a_int2) {
    TBE_REL_SET_ADD(relset, TBE_REL_EQL);
    return relset;
  }

  /* get a reference of the node containing the smaller of the 2 intervals */
  if (!(nptr = tbe_net_get_ref(a_net, int1)))
    return relset;

  /* then we check if the larger interval exists */
  if (!tbe_net_get_ref(a_net, int2))
    return relset;

  /* now we look for the larger interval in the rel list of the first */
  if (!(rptr = tbe_net_get_rlist_ref(nptr->rlist, int2))) {
    /* interval2 not in the list, so return a full relset */
    TBE_REL_SET_FILL(relset);
    return relset;
  }
  else {
    /* found it. now we determine whether we need to find the inverse */
    if (int1 == a_int1)
      return rptr->relset;

    return tbe_rel_set_inverse(rptr->relset); 
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
    __tbe_net_node *nptr;

    /* retrieve i'th net node */
    tbe_list_get_index(a_net, i, (void *) &nptr);

    if (!nptr)
      continue;

    /* if the interval doesn't have any rels, just dump the interval */
    if (tbe_list_length(nptr->rlist) == 0) {
      fprintf(a_stream, "%03u\n", nptr->interval);
      continue;
    }

    /* now get this interval's rel list */
    for (j = 0; j < tbe_list_length(nptr->rlist); j++) {
      __tbe_net_rlist_node *rptr;

      /* print interval 1 */
      fprintf(a_stream, "%03u ", nptr->interval);

      /* get j'th rel node */
      tbe_list_get_index(nptr->rlist, j, (void *) &rptr);

      /* dump the relset of these nodes */
      if (rptr)
        tbe_rel_set_dump(rptr->relset, a_stream);

      /* print interval 2 */
      fprintf(a_stream, "%03u\n", rptr->interval);
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
  __tbe_net_node *nptr;

  if (!a_stream)
    return;

  for (i = 0; i < tbe_list_length(a_net); i++) {
    /* first interval */
    tbe_list_get_index(a_net, i, (void *) &nptr);
    int1 = nptr->interval;

    for (j = 0; j < tbe_list_length(a_net); j++) {
      /* second interval */
      tbe_list_get_index(a_net, j, (void *) &nptr);
      int2 = nptr->interval;

      /* print interval 1 */
      fprintf(a_stream, "%03u ", int1);

      /* the relset */
      tbe_rel_set_dump(tbe_net_rel(a_net, int1, int2), a_stream);

      /* print interval 2 */
      fprintf(a_stream, "%03u\n", int2);
    }
  }
}
