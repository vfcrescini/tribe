#include <stdlib.h>
#include <stdio.h>
#include <tribe/mem.h>
#include <tribe/interval.h>
#include <tribe/rel.h>
#include <tribe/network.h>
#include <tribe/rqueue.h>
#include <tribe/rlist.h>

/* Van Beek's skipping conditions */
#define TBE_NET_SKIP(X,Y) \
  ( \
    TBE_REL_SET_ISCLEAR(X) || \
    TBE_REL_SET_ISCLEAR(Y) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_BEF) && TBE_REL_SET_ISIN(Y,TBE_REL_BEI)) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_BEI) && TBE_REL_SET_ISIN(Y,TBE_REL_BEF)) || \
    (TBE_REL_SET_ISIN(X,TBE_REL_DUR) && TBE_REL_SET_ISIN(Y,TBE_REL_DUI)) \
  )

/* the network list inode */
typedef struct {
  unsigned int id;
  unsigned char type;
  tbe_interval interval;
  tbe_rlist rlist;
} __tbe_net_node;

/* the network */
typedef struct {
  tbe_list list;
  unsigned int int_cnt;
} __tbe_net;

/* structure for dumping the network */
typedef struct {
  __tbe_net *net;
  unsigned int id;
  unsigned char type;
  FILE *stream;
} __tbe_net_dump;

/* structure for propagation, no endpoints */
typedef struct {
  __tbe_net *net;
  tbe_rqueue queue;
  unsigned int id1;
  unsigned int id2;
  unsigned char type1;
  unsigned char type2;
  unsigned int rs;
} __tbe_net_prop1;

/* structure for propagation, with endpoints */
typedef struct {
  __tbe_net *net;
  unsigned int id;
  unsigned char type;
  tbe_interval interval;
} __tbe_net_prop2;

/* return TBE_OK if the intervals of the 2 net nodes are equal */
static int tbe_net_cmp1(const void *a_ptr1, const void *a_ptr2);

/* return TBE_OK if the endpoints of the intervals of the 2 nodes are equal */
static int tbe_net_cmp2(const void *a_ptr1, const void *a_ptr2);

/* return a reference to a node, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(__tbe_net *a_net,
                                       unsigned int a_id,
                                       unsigned char a_type);

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

/* add a new interval into the network, with type parameter */
static int tbe_net_add_int(tbe_net a_net,
                           unsigned int a_id,
                           unsigned char a_type);

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_nprop(__tbe_net *a_net,
                                 unsigned int a_id1,
                                 unsigned int a_id2,
                                 unsigned char a_type1,
                                 unsigned char a_type2,
                                 unsigned int a_rs);

/* add a new relation, with propagation */
static int tbe_net_add_rel_wprop(tbe_net a_net,
                                 unsigned int a_id1,
                                 unsigned int a_id2,
                                 unsigned char a_type1,
                                 unsigned char a_type2,
                                 unsigned int a_rs);

/* bind existing interval with the given endpoints, with type parameter */
static int tbe_net_add_ep(tbe_net a_net,
                          unsigned int a_id,
                          unsigned char a_type,
                          tbe_interval a_int);

/* returns the rel set between the given two intervals, with type parameter */
static unsigned int tbe_net_get_rel(tbe_net a_net,
                                    unsigned int a_id1,
                                    unsigned int a_id2,
                                    unsigned char a_type1,
                                    unsigned char a_type2);

/* return TBE_OK if the intervals of the 2 tbe_net_nodes are equal */
static int tbe_net_cmp1(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_net_node *pnode1;
  __tbe_net_node *pnode2;

  pnode1 = (__tbe_net_node *) a_ptr1;
  pnode2 = (__tbe_net_node *) a_ptr2;

  if (!pnode1 || !pnode2)
    return TBE_NULLPTR;

  return
    (pnode1->id == pnode2->id) && (pnode1->type == pnode2->type) ?
    TBE_OK : TBE_FAILURE;
}

/* return TBE_OK if the endpoints of the intervals of the 2 nodes are equal */
static int tbe_net_cmp2(const void *a_ptr1, const void *a_ptr2)
{
  __tbe_net_node *pnode1;
  __tbe_net_node *pnode2;

  pnode1 = (__tbe_net_node *) a_ptr1;
  pnode2 = (__tbe_net_node *) a_ptr2;

  if (!pnode1 || !pnode2)
    return TBE_NULLPTR; 

  return
    TBE_INTERVAL_ISEQUAL(pnode1->interval, pnode2->interval) ?
    TBE_OK : TBE_FAILURE;
}

/* return a reference to a node, NULL if it doesn't exist */
static __tbe_net_node *tbe_net_get_ref(__tbe_net *a_net,
                                       unsigned int a_id,
                                       unsigned char a_type)
{
  __tbe_net *pnet;
  __tbe_net_node *pnode;
  __tbe_net_node node;
  int retval;

  if (!(pnet = (__tbe_net *) a_net))
    return NULL;

  node.id = a_id;
  node.type = a_type;

  /* there should be only one */
  retval = tbe_list_get_data_one(pnet->list,
                                 (void *) &node,
                                 tbe_net_cmp1,
                                 (void *) &pnode);

  return (retval == TBE_OK) ? pnode : NULL;
}

/* destroy contents of a tbe_net_node */
static void tbe_net_free(void *a_nptr)
{
  __tbe_net_node *pnode;

  pnode = (__tbe_net_node *) a_nptr;
  
  if (pnode) {
    if (pnode->rlist)
      tbe_rlist_destroy(&(pnode->rlist));

    TBE_MEM_FREE(pnode);
  }
}

/* print relations between 2 nodes, as stored in the network */
static int tbe_net_trav_dump1(const void *a_node, void *a_stream)
{
  __tbe_net_node *pnode;
  FILE *pstrm;

  pnode = (__tbe_net_node *) a_node;
  pstrm = (FILE *) a_stream;

  if (!pnode || !pstrm)
    return TBE_NULLPTR;

  return tbe_rlist_dump(pnode->rlist, pnode->id, pnode->type, pstrm);
}

/* print the relations between 2 nodes, as they are conceptually, level 1*/
static int tbe_net_trav_dump2_l1(const void *a_node, void *a_dump)
{
  __tbe_net_node *pnode;
  __tbe_net_dump *pdump;

  pnode = (__tbe_net_node *) a_node;
  pdump = (__tbe_net_dump *) a_dump;

  if (!pnode || !pdump)
    return TBE_NULLPTR;

  /* we have to re-traverse the list using the current interval */

  pdump->id = pnode->id;
  pdump->type = pnode->type;

  return tbe_list_traverse(pdump->net->list, tbe_net_trav_dump2_l2, a_dump);
}

/* print the relations between 2 nodes, as they are conceptually, level 2 */
static int tbe_net_trav_dump2_l2(const void *a_node, void *a_dump)
{
  __tbe_net_node *pnode;
  __tbe_net_dump *pdump;
  unsigned int rs;

  pnode = (__tbe_net_node *) a_node;
  pdump = (__tbe_net_dump *) a_dump;

  if (!pnode || !pdump)
    return TBE_NULLPTR;

  /* query the net for the relation between the 2 intervals */
  rs = tbe_net_get_rel((tbe_net) pdump->net,
                       pdump->id,
                       pnode->id,
                       pdump->type,
                       pnode->type);
  fprintf(pdump->stream,
          "%s%03u ",
          TBE_INTERVAL_PREFIX(pdump->type),
          pdump->id);

  tbe_rel_dump(rs, pdump->stream);

  fprintf(pdump->stream,
          "%s%03u\n",
          TBE_INTERVAL_PREFIX(pnode->type),
          pnode->id);

  return TBE_OK;
}

/* propagate the relations by adding the required operations in the queue */
static int tbe_net_trav_prop1(const void *a_node, void *a_prop1)
{
  __tbe_net_node *pnode;
  __tbe_net_prop1 *pprop;
  unsigned int rs1;
  unsigned int rs2;
  unsigned int rs3;
  int retval;

  pnode = (__tbe_net_node *) a_node;
  pprop = (__tbe_net_prop1 *) a_prop1;

  if (!pnode || !pprop)
    return TBE_NULLPTR;

  if (!pprop->net || !pprop->queue)
    return TBE_INVALIDINPUT;

  /* ensure that the interval in this node is not int1 or int2, that is,
   * a k such that k != i and k != j */
  if ((pnode->id == pprop->id1) && (pnode->type == pprop->type1))
    return TBE_OK;
  if ((pnode->id == pprop->id2) && (pnode->type == pprop->type2))
    return TBE_OK;

  /* find rs(k,j), given rs(k,i) and rs(i,j) */

  /* rs1 is the known relation between k and i */
  rs1 = tbe_net_get_rel((tbe_net) pprop->net,
                        pnode->id,
                        pprop->id1,
                        pnode->type,
                        pprop->type1);

  if (!TBE_NET_SKIP(rs1, pprop->rs)) {
    /* rs2 is the known relation between k and j */
    rs2 = tbe_net_get_rel((tbe_net) pprop->net,
                          pnode->id,
                          pprop->id2,
                          pnode->type,
                          pprop->type2);

    /* rs3 is the intersection of rs2 and the new rs derived from the table */
    rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_trans(rs1, pprop->rs));

    /* if the intersection of "what is in the network" and "what we
     * have concluded is the empty set, something is wrong */
    if (TBE_REL_SET_ISCLEAR(rs3))
      return TBE_FAILURE;

    /* put this in the queue for later processing */
    if (rs2 != rs3) {
      retval = tbe_rqueue_enq(pprop->queue,
                              pnode->id,
                              pprop->id2,
                              pnode->type,
                              pprop->type2,
                              rs3);

      if (retval != TBE_OK)
        return retval;
    }
  }

  /* find rs(i,k), given rs(i,j), rs(j,k) */

  /* rs1 is the known relation between j and k */
  rs1 = tbe_net_get_rel((tbe_net) pprop->net,
                        pprop->id2,
                        pnode->id,
                        pprop->type2,
                        pnode->type);

  if (!TBE_NET_SKIP(pprop->rs, rs1)) {
    /* rs2 is the know relation between i and k */
    rs2 = tbe_net_get_rel((tbe_net) pprop->net,
                          pprop->id1,
                          pnode->id,
                          pprop->type1,
                          pnode->type);

    /* rs3 is the intersection of rs2 and the new rs derived from the table */
    rs3 = TBE_REL_SET_INTERSECT(rs2, tbe_rel_trans(pprop->rs, rs1));

    /* if the intersection of "what is in the network" and "what we have
     * concluded is the empty set, something is wrong */
    if (TBE_REL_SET_ISCLEAR(rs3))
      return TBE_FAILURE;

    /* put this in the queue for later processing */
    if (rs2 != rs3) {
      retval = tbe_rqueue_enq(pprop->queue,
                              pprop->id1,
                              pnode->id,
                              pprop->type1,
                              pnode->type,
                              rs3);

      if (retval != TBE_OK)
        return retval;
    }
  }

  return TBE_OK;
}

/* propagate the relations derived from the endpoints */
static int tbe_net_trav_prop2(const void *a_node, void *a_prop2)
{
  __tbe_net_node *pnode;
  __tbe_net_prop2 *pprop;
  int retval;

  pnode = (__tbe_net_node *) a_node;
  pprop = (__tbe_net_prop2 *) a_prop2;

  if (!pnode || !pprop)
    return TBE_NULLPTR;

  /* check that this node is not the one we are comparing against */
  if ((pnode->id == pprop->id) && (pnode->type == pprop->type))
    return TBE_OK;

  /* we calculate a new relation set based on the endpoints. then we try to add
   * this new relation into the network */
  retval = tbe_net_add_rel_wprop((tbe_net) pprop->net,
                                 pprop->id,
                                 pnode->id,
                                 pprop->type,
                                 pnode->type,
                                 tbe_rel_calc(pprop->interval,
                                              pnode->interval));

  return (retval != TBE_OK) ? retval : TBE_OK;
}

/* add a new interval into the network, with type parameter */
static int tbe_net_add_int(tbe_net a_net,
                           unsigned int a_id,
                           unsigned char a_type)

{
  __tbe_net *pnet;
  __tbe_net_node *pnode;
  int retval;

  if (!(pnet = (__tbe_net *) a_net))
    return TBE_NULLPTR;

  /* check if interval is not in list yet */
  if (tbe_net_get_ref(pnet, a_id, a_type))
    return TBE_DUPLICATE;

  /* allocate mem for new node */
  if (!(pnode = TBE_MEM_MALLOC(__tbe_net_node, 1)))
    return TBE_MALLOCFAILED;

  /* create a new empty rlist */
  if ((retval = tbe_rlist_create(&(pnode->rlist))) != TBE_OK) {
    TBE_MEM_FREE(pnode);
    return retval;
  }

  /* load the node with values! */
  pnode->id = a_id;
  pnode->type = a_type;
  TBE_INTERVAL_CLEAR(pnode->interval);

  /* attach the node at the tail of the list */
  if ((retval = tbe_list_add_tail(pnet->list, (void *) pnode)) != TBE_OK) {
    tbe_rlist_destroy(&(pnode->rlist));
    TBE_MEM_FREE(pnode);
  }

  return retval;
}

/* add a new relation to existing intervals, but no propagation */
static int tbe_net_add_rel_nprop(__tbe_net *a_net,
                                 unsigned int a_id1,
                                 unsigned int a_id2,
                                 unsigned char a_type1,
                                 unsigned char a_type2,
                                 unsigned int a_rs)
{
  __tbe_net *pnet;
  __tbe_net_node *pnode;

  if (!(pnet = (__tbe_net *) a_net))
    return TBE_NULLPTR;

  /* firstly, we check whether we are trying to add something trivial */
  if ((a_id1 == a_id2) && (a_type1 == a_type2))
    return TBE_OK;

  /* get a reference of the node containing the first interval */
  if (!(pnode = tbe_net_get_ref(pnet, a_id1, a_type1)))
    return TBE_INVALIDINPUT;

  /* check if the second interval exists */
  if (!tbe_net_get_ref(pnet, a_id2, a_type2))
    return TBE_INVALIDINPUT;

  return tbe_rlist_add(pnode->rlist, a_id2, a_type2, a_rs);
}

/* add a new relation, with propagation */
static int tbe_net_add_rel_wprop(tbe_net a_net,
                                 unsigned int a_id1,
                                 unsigned int a_id2,
                                 unsigned char a_type1,
                                 unsigned char a_type2,
                                 unsigned int a_rs)
{
  __tbe_net_prop1 prop;
  unsigned int rs1;
  unsigned int rs2;
  int retval;

  if (!(prop.net = (__tbe_net *) a_net))
    return TBE_NULLPTR;

  /* check if relset if empty */
  if (TBE_REL_SET_ISCLEAR(a_rs))
    return TBE_INVALIDINPUT;

  /* check if the relset to be added contains all possible relations */
  if (TBE_REL_SET_ISFILL(a_rs))
    return TBE_OK;

  /* rs1 is the known relation between int1 and int2 */
  rs1 = tbe_net_get_rel(a_net, a_id1, a_id2, a_type1, a_type2);

  /* rs2 is the intersection of rs1 and the proposed relation */
  rs2 = TBE_REL_SET_INTERSECT(rs1, a_rs);

  /* see if the proposed relation clashes with what we already know */
  if (TBE_REL_SET_ISCLEAR(rs2))
    return TBE_INVALIDINPUT;

  /* see if the proposed relation is more specific than what we already know */
  if (rs1 == rs2)
    return TBE_OK;

  /* intialise and load the queue */
  if ((retval = tbe_rqueue_create(&(prop.queue))) != TBE_OK)
    return retval;

  retval = tbe_rqueue_enq(prop.queue,
                          a_id1,
                          a_id2,
                          a_type1,
                          a_type2,
                          rs2);
  if (retval != TBE_OK) {
    tbe_rqueue_destroy(&(prop.queue));
    return retval;
  }

  while (tbe_list_length(prop.queue) && retval == TBE_OK) {
    /* get relation from queue */
    retval = tbe_rqueue_deq(prop.queue,
                            &(prop.id1),
                            &(prop.id2),
                            &(prop.type1),
                            &(prop.type2),
                            &(prop.rs));
    if (retval != TBE_OK)
      break;

    /* add blindly, disregarding existing relations */
    retval = tbe_net_add_rel_nprop(prop.net,
                                   prop.id1,
                                   prop.id2,
                                   prop.type1,
                                   prop.type2,
                                   prop.rs);
    if (retval != TBE_OK)
      break;

    /* traverse the list, propagate the effects of this new relation */
    retval = tbe_list_traverse((prop.net)->list, tbe_net_trav_prop1, &prop);
    if (retval != TBE_OK)
      break;
  }

  tbe_rqueue_destroy(&(prop.queue));

  return retval;
}

/* bind existing interval with the given endpoints, with type parameter */
static int tbe_net_add_ep(tbe_net a_net,
                          unsigned int a_id,
                          unsigned char a_type,
                          tbe_interval a_int)
{
  int retval;
  __tbe_net *pnet;
  __tbe_net_node *pnode;
  __tbe_net_prop2 prop;

  if (!(pnet = (__tbe_net *) a_net))
    return TBE_NULLPTR;

  /* see if the given interval actually exists */
  if (!(pnode = tbe_net_get_ref(pnet, a_id, a_type)))
    return TBE_INVALIDINPUT;

  /* check if at least one endpoint is actually given */
  if (TBE_INTERVAL_EP_ISCLEAR(a_int))
    return TBE_OK;

  /* now we go through the network to see if the definition of these endpoints
   * actually change anything else */

  prop.net = pnet;
  prop.id = a_id;
  prop.type = a_type;
  prop.interval = a_int;

  retval = tbe_list_traverse(pnet->list, tbe_net_trav_prop2, &prop);

  if (retval != TBE_OK)
    return retval;

  /* if all went well, we copy the endpoints to the node */
  pnode->interval = a_int;

  return TBE_OK;
}

/* returns the rel set between the given two intervals, with type parameter */
static unsigned int tbe_net_get_rel(tbe_net a_net,
                                    unsigned int a_id1,
                                    unsigned int a_id2,
                                    unsigned char a_type1,
                                    unsigned char a_type2)
{
  __tbe_net *pnet;
  __tbe_net_node *pnode;
  unsigned char swapped;
  unsigned int rs;

  if (!(pnet = (__tbe_net *) a_net))
    return TBE_REL_SET_NUL;

  TBE_REL_SET_CLEAR(rs);

  /* eliminate the trivial case */
  if ((a_id1 == a_id2) && (a_type1 == a_type2)) {
    TBE_REL_SET_ADD(rs, TBE_REL_EQL);
    return rs;
  }

  /* see if we need to normalise */
  swapped = (tbe_rel_is_normalised(a_id1, a_id2, a_type1, a_type2) != TBE_OK);

  if (swapped)
    tbe_rel_normalise(&a_id1, &a_id2, &a_type1, &a_type2, NULL);

  /* get a reference of the node containing the smaller of the 2 intervals */
  if (!(pnode = tbe_net_get_ref(pnet, a_id1, a_type1)))
    return TBE_REL_SET_NUL;

  /* then we check if the larger interval exists */
  if (!tbe_net_get_ref(pnet, a_id2, a_type2))
    return TBE_REL_SET_NUL;

  /* now we look for the larger interval in the rel list of the first */
  if (tbe_rlist_get(pnode->rlist, a_id2, a_type2, &rs) != TBE_OK)
    return TBE_REL_SET_ALL;

  return (swapped) ? tbe_rel_inverse(rs) : rs;
}

/* create a new network */
int tbe_net_create(tbe_net *a_net)
{
  __tbe_net *pnet;
  int retval;

  /* allocate mem for the net structure, then create a net list */

  if (!a_net)
    return TBE_NULLPTR;

  if (!(pnet = TBE_MEM_MALLOC(__tbe_net, 1)))
    return TBE_MALLOCFAILED;

  if ((retval = tbe_list_create(&(pnet->list))) != TBE_OK) {
    TBE_MEM_FREE(pnet);
    return retval;
  }
  
  pnet->int_cnt = 0;
  *a_net = (tbe_net) pnet;

  return TBE_OK;
}

/* destroy the given net */
void tbe_net_destroy(tbe_net *a_net)
{
  __tbe_net *pnet;

  /* first destroy the net list, then destroy the structure */

  if ((pnet = (__tbe_net *) *a_net)) {
    tbe_list_destroy(&(pnet->list), tbe_net_free);
    TBE_MEM_FREE(pnet);
  }
}

/* add a new interval into the network */
int tbe_net_add_interval(tbe_net a_net, unsigned int a_id)
{
  return tbe_net_add_int(a_net, a_id, TBE_INTERVAL_EXTRNL);
}

/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_relation(tbe_net a_net,
                         unsigned int a_id1,
                         unsigned int a_id2,
                         unsigned int a_rs)
{
  return tbe_net_add_rel_wprop(a_net,
                               a_id1,
                               a_id2,
                               TBE_INTERVAL_EXTRNL,
                               TBE_INTERVAL_EXTRNL,
                               a_rs);
}

/* bind the given (existing) interval with the given endpoints */
int tbe_net_add_endpoints(tbe_net a_net,
                          unsigned int a_id,
                          tbe_interval a_int)
{
  return tbe_net_add_ep(a_net, a_id, TBE_INTERVAL_EXTRNL, a_int);
}

/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_get_relation1(tbe_net a_net,
                                   unsigned int a_id1,
                                   unsigned int a_id2)
{
  return tbe_net_get_rel(a_net,
                         a_id1,
                         a_id2,
                         TBE_INTERVAL_EXTRNL,
                         TBE_INTERVAL_EXTRNL);
}

/* returns the rel set between the given interval and endpoints */
unsigned int tbe_net_get_relation2(tbe_net a_net,
                                   unsigned int a_id,
                                   tbe_interval a_interval)
{
  __tbe_net *pnet;
  __tbe_net_node *pnode;
  __tbe_net_node node;
  unsigned int rs;
  int retval;

  if (!(pnet = (__tbe_net *) a_net))
    return TBE_REL_SET_NUL;

  if (!TBE_INTERVAL_ISVALID(a_interval))
    return TBE_REL_SET_NUL;

  /* try to find an interval with the given endpoints. any match will do */

  node.interval = a_interval;

  retval = tbe_list_get_data_one(pnet->list,
                                 (void *) &node,
                                 tbe_net_cmp2,
                                 (void *) &pnode);

  switch (retval) {
    case TBE_NOTFOUND :
      /* no interval that contains the given endpoints exists, so we add one */
      retval = tbe_net_add_int(a_net, pnet->int_cnt, TBE_INTERVAL_INTRNL);

      if (retval != TBE_OK)
        return TBE_REL_SET_NUL;

      /* assign the given endpoints to the new interval */
      retval = tbe_net_add_ep(a_net,
                              pnet->int_cnt,
                              TBE_INTERVAL_INTRNL,
                              a_interval);

      if (retval != TBE_OK)
        return TBE_REL_SET_NUL;

      /* now get the relation between the given interval and the new one */
      rs = tbe_net_get_rel(a_net,
                           a_id,
                           pnet->int_cnt,
                           TBE_INTERVAL_EXTRNL,
                           TBE_INTERVAL_INTRNL);

      /* don't forget to increment the internal interval counter */
      pnet->int_cnt = pnet->int_cnt + 1;

      return rs;
    case TBE_OK :
      /* an interval whose endpoints matches the given exists */
      rs = tbe_net_get_rel(a_net,
                           a_id,
                           pnode->id,
                           TBE_INTERVAL_EXTRNL,
                           pnode->type);

      return rs;
  }

  return TBE_REL_SET_NUL;
}

/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream)
{
  __tbe_net *pnet;

  if (!(pnet = (__tbe_net *) a_net) || !a_stream)
    return;

  tbe_list_traverse(pnet->list, tbe_net_trav_dump1, (void *) a_stream);
}

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream)
{
  __tbe_net *pnet;
  __tbe_net_dump dump;

  if (!(pnet = (__tbe_net *) a_net) || !a_stream)
    return;

  dump.net = pnet;
  dump.stream = a_stream;

  tbe_list_traverse(pnet->list, tbe_net_trav_dump2_l1, (void *) &dump);
}
