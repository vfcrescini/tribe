/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <tribe/tribe.h>
#include <tribe/rel.h>
#include <tribe/clist.h>
#include <tribe/network.h>

int dump1(unsigned int a_id, void *a_stream)
{
  fprintf((FILE *) a_stream, "%d\n", a_id);
  return 0;
}

int dump2(unsigned int a_tuple[], unsigned int a_size, void *a_parm)
{
  unsigned int i;

  for (i = 0; i < a_size; i++)
    fprintf((FILE *) a_parm, "%d ", a_tuple[i]);

  fprintf((FILE *) a_parm, "\n");

  return TBE_OK;
}

int main(int argc, char *argv[])
{
  tbe_net net;
  unsigned int rs;
#if 1
  tbe_clist clist;
  unsigned int array[2];
#endif
#if 0
  tbe_interval i;
  tbe_interval j;
#endif

#if 0
  int r;
  unsigned int rs1;
  unsigned int rs2;

  TBE_REL_SET_FILL(rs2);
  for (r = TBE_REL_EQL; r <= TBE_REL_FII; r++) {
    TBE_REL_SET_CLEAR(rs1);
    TBE_REL_SET_ADD(rs1, r);
    printf("%2d FILL=%d\n", r, TBE_REL_SET_ISFILL(tbe_rel_trans(rs1, rs2)));
  }
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_interval(net, 0);
  tbe_net_add_interval(net, 1);
  tbe_net_add_interval(net, 2);
  tbe_net_add_interval(net, 3);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);
  TBE_REL_SET_ADD(rs, TBE_REL_MEI);
  TBE_REL_SET_ADD(rs, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_relation(net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_relation(net, 2, 0, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_relation(net, 3, 1, rs));

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 1, TBE_REL_SET_ALL));

  tbe_net_dump2(net, stdout);

  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  printf("adding: %d\n", tbe_net_add_endpoints(net, 0, i));

  TBE_INTERVAL_INIT(i, 0,  4, TBE_INTERVAL_EP_ALL);
  printf("adding: %d\n", tbe_net_add_endpoints(net, 1, i));

  tbe_net_dump2(net, stdout);

  printf("interval 1 to [3,15]: ");
  TBE_INTERVAL_INIT(i, 3, 15, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_net_get_relation2(net, 1, i), stdout);
  printf("\n");

  printf("interval 1 to [0,5]: ");
  TBE_INTERVAL_INIT(j, 0, 5, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_net_get_relation2(net, 1, j), stdout);
  printf("\n");

  tbe_net_dump2(net, stdout);

  tbe_net_destroy(&net);
#endif

#if 1
  tbe_net_create(&net);
  tbe_net_add_interval(net, 0);
  tbe_net_add_interval(net, 1);
  tbe_net_add_interval(net, 2);
  tbe_net_add_interval(net, 3);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_relation(net, 3, 0, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);

  printf("adding: %d\n", tbe_net_add_relation(net, 3, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_relation(net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_relation(net, 3, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_FIN);
  TBE_REL_SET_ADD(rs, TBE_REL_FII);

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);
  TBE_REL_SET_ADD(rs, TBE_REL_FIN);
  TBE_REL_SET_ADD(rs, TBE_REL_FII);

  tbe_net_get_intervals(net, 0, rs, dump1, (void *) stdout);

  /* 6 variables:
   *   0. the literal interval 0
   *   1. the literal interval 2
   *   2. & 3. all intervals whose relation is met | sta | bef | ovr
   *   4. all intervals that is fin | fin | eql by interval 0 (variable 0)
   *   5. any interval
   */

  tbe_clist_create(&clist);

  array[0] = 0;
  array[1] = 2;

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);

  tbe_clist_add(clist, 2, 3, rs);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_FIN);
  TBE_REL_SET_ADD(rs, TBE_REL_FII);
  TBE_REL_SET_ADD(rs, TBE_REL_EQL);

  tbe_clist_add(clist, 4, 0, rs);

  tbe_net_get_tuples(net, 4, 2, array, clist, dump2, (void *) stdout);

  tbe_clist_destroy(&clist);

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

#if 0
  printf("\nbefore: ");
  TBE_INTERVAL_INIT(i, 1,  10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 11, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nafter: ");
  TBE_INTERVAL_INIT(i, 11, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  10, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\noverlaps: ");
  TBE_INTERVAL_INIT(i, 1,  15, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\noverlapped-by: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  15, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nmeets: ");
  TBE_INTERVAL_INIT(i, 1,  10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nmet-by: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  10, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nduring: ");
  TBE_INTERVAL_INIT(i, 5, 15, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\ncontains: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 5, 15, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nstarts: ");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nstarted-by: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nfinishes: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nfinished-by: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\nequals: ");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_ALL);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\n");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_1);
  tbe_rel_dump(tbe_rel_calc(i, j), stdout);
  printf("\n");
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_interval(net, 0);
  tbe_net_add_interval(net, 1);
  tbe_net_add_interval(net, 2);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_relation(net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_relation(net, 2, 0, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_interval(net, 0);
  tbe_net_add_interval(net, 1);
  tbe_net_add_interval(net, 2);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_relation(net, 0, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_relation(net, 1, 2, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

  return 0;
}
