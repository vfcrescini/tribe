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

/* structure for dumping the network */
typedef struct {
  tbe_net net;
  unsigned int interval;
  FILE *stream;
} __tbe_net_dump;

/* structure for propagation */
typedef struct {
  tbe_net net;
  tbe_iqueue queue;
  unsigned int int1;
  unsigned int int2;
  unsigned int rs;
} __tbe_net_prop;

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

/* print relations between 2 nodes */
static int tbe_net_rlist_trav_dump(const void *a_node, void *a_dump);

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_rlist_node *tbe_net_get_rlist_ref(__tbe_net_rlist a_rlist,
                                                   unsigned int a_int);

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(void *a_ptr1, void *a_ptr2);

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(tbe_net a_net, unsigned int a_int);

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node);

/* print relations between 2 nodes, as stored in the network */
static int tbe_net_trav_dump1(const void *a_node, void *a_stream);

/* print the relations between 2 nodes, as they are conceptually */
static int tbe_net_trav_dump2(const void *a_node, void *a_dump);

/* print the relations between 2 nodes, as they are conceptually */
static int tbe_net_trav_dump3(const void *a_node, void *a_dump);

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_relset);

/* propagate the relations by adding the required operations in the queue */
static int tbe_net_propagate(const void *a_node, void *a_prop);

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

/* print relations between 2 nodes */
static int tbe_net_rlist_trav_dump(const void *a_node, void *a_dump)
{
  __tbe_net_rlist_node *rptr;
  __tbe_net_dump *dptr;

  rptr = (__tbe_net_rlist_node *) a_node;
  dptr = (__tbe_net_dump *) a_dump;

  if (!rptr || !dptr)
    return TBE_NULLPTR;

  if (!dptr->stream)
     return TBE_INVALIDINPUT;

  fprintf(dptr->stream, "%03u ", dptr->interval);
  tbe_rel_set_dump(rptr->relset, dptr->stream);
  fprintf(dptr->stream, "%03u\n", rptr->interval);
    
  return TBE_OK;
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

/* print relations between 2 nodes, as stored in the network */
static int tbe_net_trav_dump1(const void *a_node, void *a_stream)
{
  __tbe_net_node *nptr;
  __tbe_net_dump dump;
  FILE *sptr;

  if (!(nptr = (__tbe_net_node *) a_node) || !(sptr = (FILE *) a_stream))
    return TBE_NULLPTR;

  /* check if the rlist is empty */
  if (tbe_list_length(nptr->rlist) == 0) {
    fprintf(sptr, "%03u\n", nptr->interval);
    return TBE_OK;
  }

  dump.interval = nptr->interval;
  dump.stream = sptr;

  return tbe_list_traverse(nptr->rlist, tbe_net_rlist_trav_dump, &dump);
}

/* print the relations between 2 nodes, as they are conceptually */
static int tbe_net_trav_dump2(const void *a_node, void *a_dump)
{
  __tbe_net_node *nptr;
  __tbe_net_dump *dptr;
  unsigned int rs;

  nptr = (__tbe_net_node *) a_node;
  dptr = (__tbe_net_dump *) a_dump;

  if (!nptr || !dptr)
    return TBE_NULLPTR;

  if (!dptr->net || !dptr->stream)
    return TBE_INVALIDINPUT;

  /* query the net for the relation between the 2 intervals */
  rs = tbe_net_rel(dptr->net, dptr->interval, nptr->interval);
  
  fprintf(dptr->stream, "%03u ", dptr->interval);
  tbe_rel_set_dump(rs, dptr->stream);
  fprintf(dptr->stream, "%03u\n", nptr->interval);

  return TBE_OK;
}

/* print the relations between 2 nodes, as they are conceptually */
static int tbe_net_trav_dump3(const void *a_node, void *a_dump)
{
  __tbe_net_node *nptr;
  __tbe_net_dump *dptr;

  nptr = (__tbe_net_node *) a_node;
  dptr = (__tbe_net_dump *) a_dump;
 
  if (!nptr || !dptr)
    return TBE_NULLPTR;

  dptr->interval = nptr->interval;

  return tbe_list_traverse(dptr->net, tbe_net_trav_dump2, a_dump);
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

    /* blindly replace the relset with the new one */
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

/* propagate the relations by adding the required operations in the queue */
static int tbe_net_propagate(const void *a_node, void *a_prop)
{
  __tbe_net_node *nptr;
  __tbe_net_prop *pptr;
  unsigned int rs1;
  unsigned int rs2;
  unsigned int rs3;
  int retval;

  nptr = (__tbe_net_node *) a_node;
  pptr = (__tbe_net_prop *) a_prop;

  if (!nptr || !pptr)
    return TBE_NULLPTR;

  if (!pptr->net || !pptr->queue)
    return TBE_INVALIDINPUT;

  /* ensure that the interval in this node is not int1 or int2, that is,
   * a k such that k != i and k != j */
  if (nptr->interval == pptr->int1 || nptr->interval == pptr->int2)
    return TBE_OK;

  /* find rs(k,j), given rs(k,i) and rs(i,j) */

  rs1 = tbe_net_rel(pptr->net, nptr->interval, pptr->int1);

  if (!TBE_NET_SKIP(rs1, pptr->rs)) {
    rs2 = tbe_net_rel(pptr->net, nptr->interval, pptr->int2);
    rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(rs1, pptr->rs));

    /* if the intersection of "what is in the network" and "what we
     * have concluded is the empty set, something is wrong */
    if (TBE_REL_SET_ISCLEAR(rs3))
      return TBE_FAILURE;

    /* put this in the queue for later processing */
    if (rs2 != rs3) {
      retval = tbe_iqueue_enq(pptr->queue, nptr->interval, pptr->int2, rs3);

      if (retval != TBE_OK)
        return retval;
    }
  }

  /* find rs(i,k), given rs(i,j), rs(j,k) */

  rs1 = tbe_net_rel(pptr->net, pptr->int2, nptr->interval);

  if (!TBE_NET_SKIP(pptr->rs, rs1)) {
    rs2 = tbe_net_rel(pptr->net, pptr->int1, nptr->interval);
    rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_set_lookup(pptr->rs, rs1));

    /* if the intersection of "what is in the network" and "what we have
     * concluded is the empty set, something is wrong */
    if (TBE_REL_SET_ISCLEAR(rs3))
      return TBE_FAILURE;

    /* put this in the queue for later processing */
    if (rs2 != rs3) {
      retval = tbe_iqueue_enq(pptr->queue, pptr->int1, nptr->interval, rs3);

      if (retval != TBE_OK)
        return retval;
    }
  }

  return TBE_OK;
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
  unsigned int rs;
  __tbe_net_prop p;
  int retval;

  if (!(p.net = a_net))
    return TBE_NULLPTR;

  /* check if the relset to be added contains all possible relations */
  if (TBE_REL_SET_ISFILL(a_relset))
    return TBE_OK;

  /* get intersection of what is already in the network and what is given */
  rs = TBE_REL_SET_INTERSECT(tbe_net_rel(a_net, a_int1, a_int2), a_relset);

  if (TBE_REL_SET_ISCLEAR(rs))
    return TBE_INVALIDINPUT;

  /* intialise and load the queue */
  if ((retval = tbe_iqueue_create(&(p.queue))) != TBE_OK)
    return retval;

  if ((retval = tbe_iqueue_enq(p.queue, a_int1, a_int2, rs)) != TBE_OK) {
    tbe_iqueue_destroy(&(p.queue));
    return retval;
  }

  retval = TBE_OK;

  while (tbe_list_length(p.queue) && retval == TBE_OK) {
    /* get relation from queue */
    retval = tbe_iqueue_deq(p.queue, &(p.int1), &(p.int2), &(p.rs));

    if (retval != TBE_OK)
      break;

    /* normalise then add this new relation to the network */
    tbe_net_normalise(&(p.int1), &(p.int2), &(p.rs));
    retval = tbe_net_add_rel_noprop(p.net, p.int1, p.int2, p.rs);

    if (retval != TBE_OK)
      break;

    /* traverse the list, propagate the effects of this new relation */
    retval = tbe_list_traverse(p.net, tbe_net_propagate, &p);

    if (retval != TBE_OK)
      break;
  }

  tbe_iqueue_destroy(&(p.queue));

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

  /* found it. now we determine whether we need to find the inverse */
  return (int1 == a_int1) ? rptr->relset : tbe_rel_set_inverse(rptr->relset);
}

/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream)
{
  if (!a_stream) 
    return;

  tbe_list_traverse(a_net, tbe_net_trav_dump1, (void *) a_stream);
}

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream)
{
  __tbe_net_dump dump;

  if (!a_stream)
    return;

  dump.net = a_net;
  dump.stream = a_stream;

  tbe_list_traverse(a_net, tbe_net_trav_dump3, (void *) &dump);
}
