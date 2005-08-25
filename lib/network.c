#include <stdlib.h>
#include <stdio.h>
#include <tribe/rel.h>
#include <tribe/mem.h>
#include <tribe/network.h>
#include <tribe/iqueue.h>
#include <tribe/rlist.h>

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

/* structure for propagation, no endpoints */
typedef struct {
  tbe_net net;
  tbe_iqueue queue;
  unsigned int int1;
  unsigned int int2;
  unsigned int rs;
} __tbe_net_prop1;

/* structure for propagation, with endpoints */
typedef struct {
  tbe_net net;
  unsigned int interval;
  unsigned int ep_1;
  unsigned int ep_2;
  unsigned char ep_mask;
} __tbe_net_prop2;

/* the network */
typedef struct {
  unsigned int interval;
  unsigned int ep_1;
  unsigned int ep_2;
  unsigned char ep_mask;
  tbe_rlist rlist;
} __tbe_net_node;

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(const void *a_ptr1, const void *a_ptr2);

/* return a reference to the node containing a_int, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(tbe_net a_net, unsigned int a_int);

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_node);

/* print relations between 2 nodes, as stored in the network */
static int tbe_net_trav_dump1(const void *a_node, void *a_stream);

/* print the relations between 2 nodes, as they are conceptually, level 1 */
static int tbe_net_trav_dump2_l1(const void *a_node, void *a_dump);

/* print the relations between 2 nodes, as they are conceptually, level 2 */
static int tbe_net_trav_dump2_l2(const void *a_node, void *a_dump);

/* propagate the relations by adding the required operations in the queue */
static int tbe_net_trav_prop1(const void *a_node, void *a_prop1);

/* propagate the relations derived from the endpoints */
static int tbe_net_trav_prop2(const void *a_node, void *a_prop2);

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_rs);

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp(const void *a_ptr1, const void *a_ptr2)
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
      tbe_rlist_destroy(&(nptr->rlist));

    TBE_MEM_FREE(nptr);
  }
}

/* print relations between 2 nodes, as stored in the network */
static int tbe_net_trav_dump1(const void *a_node, void *a_stream)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_node;
  FILE *sptr = (FILE *) a_stream;

  if (!nptr || !sptr)
    return TBE_NULLPTR;

  return tbe_rlist_dump(nptr->rlist, nptr->interval, sptr);
}

/* print the relations between 2 nodes, as they are conceptually, level 1*/
static int tbe_net_trav_dump2_l1(const void *a_node, void *a_dump)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_node;
  __tbe_net_dump *dptr = (__tbe_net_dump *) a_dump;

  if (!nptr || !dptr)
    return TBE_NULLPTR;

  dptr->interval = nptr->interval;

  return tbe_list_traverse(dptr->net, tbe_net_trav_dump2_l2, a_dump);
}

/* print the relations between 2 nodes, as they are conceptually, level 2 */
static int tbe_net_trav_dump2_l2(const void *a_node, void *a_dump)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_node;
  __tbe_net_dump *dptr = (__tbe_net_dump *) a_dump;
  unsigned int rs;

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

/* propagate the relations by adding the required operations in the queue */
static int tbe_net_trav_prop1(const void *a_node, void *a_prop1)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_node;
  __tbe_net_prop1 *pptr = (__tbe_net_prop1 *) a_prop1;
  unsigned int rs1;
  unsigned int rs2;
  unsigned int rs3;
  int retval;

  if (!nptr || !pptr)
    return TBE_NULLPTR;

  if (!pptr->net || !pptr->queue)
    return TBE_INVALIDINPUT;

  /* ensure that the interval in this node is not int1 or int2, that is,
   * a k such that k != i and k != j */
  if (nptr->interval == pptr->int1 || nptr->interval == pptr->int2)
    return TBE_OK;

  /* find rs(k,j), given rs(k,i) and rs(i,j) */

  /* rs1 is the known relation between k and i */
  rs1 = tbe_net_rel(pptr->net, nptr->interval, pptr->int1);

  if (!TBE_NET_SKIP(rs1, pptr->rs)) {
    /* rs2 is the known relation between k and j */
    rs2 = tbe_net_rel(pptr->net, nptr->interval, pptr->int2);
    /* rs3 is the intersection of rs2 and the new rs derived from the table */
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

  /* rs1 is the known relation between j and k */
  rs1 = tbe_net_rel(pptr->net, pptr->int2, nptr->interval);

  if (!TBE_NET_SKIP(pptr->rs, rs1)) {
    /* rs2 is the know relation between i and k */
    rs2 = tbe_net_rel(pptr->net, pptr->int1, nptr->interval);
    /* rs3 is the intersection of rs2 and the new rs derived from the table */
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

/* propagate the relations derived from the endpoints */
static int tbe_net_trav_prop2(const void *a_node, void *a_prop2)
{
  __tbe_net_node *nptr = (__tbe_net_node *) a_node;
  __tbe_net_prop2 *pptr = (__tbe_net_prop2 *) a_prop2;
  unsigned int relset;
  int retval;

  if (!nptr || !pptr)
    return TBE_NULLPTR;

  /* check that this node is not the one we are comparing against */
  if (nptr->interval == pptr->interval)
    return TBE_OK;

  /* assuming that at least 1 endpoint in a_prop2 is defined, we then check the
   * endpoints of a_node. if none is given, then we can do nothing */
  if ((nptr->ep_mask & TBE_REL_EP_ALL) == TBE_REL_EP_NUL)
    return TBE_OK;

  /* see what relset we can deduce from the given endpoints */
  relset = tbe_rel_calc(pptr->ep_1,
                        pptr->ep_2,
                        nptr->ep_1,
                        nptr->ep_2,
                        pptr->ep_mask,
                        nptr->ep_mask);

  /* now we try to add this new relation to the network */
  retval = tbe_net_add_rel(pptr->net, pptr->interval, nptr->interval, relset);

  if (retval != TBE_OK)
    return retval; 

  return TBE_OK;
}

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_noprop(tbe_net a_net,
                                  unsigned int a_int1,
                                  unsigned int a_int2,
                                  unsigned int a_rs)
{
  __tbe_net_node *nptr;

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

  return tbe_rlist_add(nptr->rlist, a_int2, a_rs);
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
                      unsigned int *a_rs)
{
  unsigned int a;
  unsigned int b;

  if (!a_int1 || !a_int2 || !a_rs)
    return TBE_NULLPTR;

  a = *a_int1;
  b = *a_int2;

  *a_int1 = TBE_INT_MIN(a, b);
  *a_int2 = TBE_INT_MAX(a, b);

  if (a != *a_int1)
    *a_rs = tbe_rel_set_inverse(*a_rs);

  return TBE_OK;
}

/* add a new interval into the network */
int tbe_net_add_int(tbe_net a_net, unsigned int a_int)
{
  __tbe_net_node *nptr;
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
  if ((retval = tbe_rlist_create(&(nptr->rlist))) != TBE_OK) {
    TBE_MEM_FREE(nptr);
    return retval;
  }

  nptr->interval = a_int;
  nptr->ep_1 = 0;
  nptr->ep_2 = 0;
  nptr->ep_mask = TBE_REL_EP_NUL;

  if ((retval = tbe_list_add_tail(a_net, (void *) nptr)) != TBE_OK) {
    tbe_rlist_destroy(&(nptr->rlist));
    TBE_MEM_FREE(nptr);
  }

  return retval;
}

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_rel(tbe_net a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_rs)
{
  unsigned int rs1;
  unsigned int rs2;
  __tbe_net_prop1 p;
  int retval;

  if (!(p.net = a_net))
    return TBE_NULLPTR;

  /* check if the relset to be added contains all possible relations */
  if (TBE_REL_SET_ISFILL(a_rs))
    return TBE_OK;

  /* rs1 is the known relation between int1 and int2 */
  rs1 = tbe_net_rel(a_net, a_int1, a_int2);

  /* rs2 is the intersection of rs1 and the proposed relation */
  rs2 = TBE_REL_SET_INTERSECT(rs1, a_rs);

  /* see if the proposed relation clashes with what we already know */
  if (TBE_REL_SET_ISCLEAR(rs2))
    return TBE_INVALIDINPUT;

  /* see if the proposed relation is more specific than what we already know */
  if (rs1 == rs2)
    return TBE_OK;

  /* intialise and load the queue */
  if ((retval = tbe_iqueue_create(&(p.queue))) != TBE_OK)
    return retval;

  if ((retval = tbe_iqueue_enq(p.queue, a_int1, a_int2, rs2)) != TBE_OK) {
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
    retval = tbe_list_traverse(p.net, tbe_net_trav_prop1, &p);

    if (retval != TBE_OK)
      break;
  }

  tbe_iqueue_destroy(&(p.queue));

  return retval;
}

/* bind the given interval with the given endpoints */
int tbe_net_add_ep(tbe_net a_net, 
                   unsigned int a_int,
                   unsigned int a_ep_1,
                   unsigned int a_ep_2,
                   unsigned char a_ep_mask)
{
  int retval;
  __tbe_net_node *nptr;
  __tbe_net_prop2 p;

  if (!a_net)
    return TBE_NULLPTR;

  /* check if at least one endpoint is actually given */
  if ((a_ep_mask & TBE_REL_EP_ALL) == TBE_REL_EP_NUL)
    return TBE_OK;

  /* see if the given inerval actually exists */
  if (!(nptr = tbe_net_get_ref(a_net, a_int)))
    return TBE_INVALIDINPUT;

  /* now we go through the network to see if the definition of these endpoints
   * actually change anything else */

  p.net = a_net;
  p.interval = a_int;
  p.ep_1 = a_ep_1;
  p.ep_2 = a_ep_2;
  p.ep_mask = a_ep_mask;

  if ((retval = tbe_list_traverse(a_net, tbe_net_trav_prop2, &p)) != TBE_OK)
    return retval;

  /* if all went well, we add the endpoints to the network */
  nptr->ep_1 = a_ep_1;
  nptr->ep_2 = a_ep_2;
  nptr->ep_mask = a_ep_mask;

  return TBE_OK;
}

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_rel(tbe_net a_net,
                         unsigned int a_int1,
                         unsigned int a_int2)
{
  __tbe_net_node *nptr;
  unsigned int int1 = TBE_INT_MIN(a_int1, a_int2);
  unsigned int int2 = TBE_INT_MAX(a_int1, a_int2);
  unsigned int rs;

  TBE_REL_SET_CLEAR(rs);

  /* first, we eliminate the trivial case */
  if (a_int1 == a_int2) {
    TBE_REL_SET_ADD(rs, TBE_REL_EQL);
    return rs;
  }

  /* get a reference of the node containing the smaller of the 2 intervals */
  if (!(nptr = tbe_net_get_ref(a_net, int1)))
    return TBE_REL_SET_NUL;

  /* then we check if the larger interval exists */
  if (!tbe_net_get_ref(a_net, int2))
    return TBE_REL_SET_NUL;

  /* now we look for the larger interval in the rel list of the first */
  if (tbe_rlist_get(nptr->rlist, int2, &rs) != TBE_OK)
    return TBE_REL_SET_NUL;

  /* now determine whether we need to find the inverse */
  return (int1 == a_int1) ? rs : tbe_rel_set_inverse(rs);
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

  tbe_list_traverse(a_net, tbe_net_trav_dump2_l1, (void *) &dump);
}
