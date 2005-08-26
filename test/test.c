#include <stdio.h>
#include <tribe/tribe.h>
#include <tribe/rel.h>
#include <tribe/network.h>

int main(int argc, char *argv[])
{
  tbe_net net;
  tbe_rel rel;
#if 0
  tbe_interval i;
  tbe_interval j;
#endif

#if 0
  int i;
  unsigned int rs1;
  unsigned int rs2;

  TBE_REL_SET_FILL(rs2);
  for (i = TBE_REL_EQL; i <= TBE_REL_FII; i++) {
    TBE_REL_SET_CLEAR(rs1);
    TBE_REL_SET_ADD(rs1, i);
    printf("%2d FILL=%d\n", i, TBE_REL_SET_ISFILL(tbe_rel_set_lookup(rs1, rs2)));
  }
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_int(net, 1);
  tbe_net_add_int(net, 2);
  tbe_net_add_int(net, 3);
  tbe_net_add_int(net, 4);

  TBE_REL_INIT(rel, 1, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_BEF);
  TBE_REL_ADD(rel, TBE_REL_MET);
  TBE_REL_ADD(rel, TBE_REL_MEI);
  TBE_REL_ADD(rel, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 2, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_OVR);
  TBE_REL_ADD(rel, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 3, 1, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_OVR);
  TBE_REL_ADD(rel, TBE_REL_STA);
  TBE_REL_ADD(rel, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 4, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 1, 2, TBE_REL_SET_ALL);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  tbe_net_dump2(net, stdout);

  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  printf("adding: %d\n", tbe_net_add_ep(net, 1, i));

  TBE_INTERVAL_INIT(i, 0,  4, TBE_INTERVAL_EP_ALL);
  printf("adding: %d\n", tbe_net_add_ep(net, 2, i));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

#if 1
  tbe_net_create(&net);
  tbe_net_add_int(net, 1);
  tbe_net_add_int(net, 2);
  tbe_net_add_int(net, 3);
  tbe_net_add_int(net, 4);

  TBE_REL_INIT(rel, 4, 1, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_STA);
  TBE_REL_ADD(rel, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 1, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_DUR);
  TBE_REL_ADD(rel, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 4, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_OVR);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 2, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_DUR);
  TBE_REL_ADD(rel, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 4, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_STA);
  TBE_REL_ADD(rel, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 1, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_FIN);
  TBE_REL_ADD(rel, TBE_REL_FII);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

#if 0
  printf("\nbefore: ");
  TBE_INTERVAL_INIT(i, 1,  10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 11, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nafter: ");
  TBE_INTERVAL_INIT(i, 11, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  10, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\noverlaps: ");
  TBE_INTERVAL_INIT(i, 1,  15, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\noverlapped-by: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  15, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nmeets: ");
  TBE_INTERVAL_INIT(i, 1,  10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nmet-by: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1,  10, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nduring: ");
  TBE_INTERVAL_INIT(i, 5, 15, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\ncontains: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 5, 15, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nstarts: ");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nstarted-by: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nfinishes: ");
  TBE_INTERVAL_INIT(i, 10, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nfinished-by: ");
  TBE_INTERVAL_INIT(i, 1, 20, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 10, 20, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\nequals: ");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_ALL);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\n");
  TBE_INTERVAL_INIT(i, 1, 10, TBE_INTERVAL_EP_ALL);
  TBE_INTERVAL_INIT(j, 1, 10, TBE_INTERVAL_EP_1);
  tbe_rel_set_dump(tbe_rel_calc(i, j), stdout);
  printf("\n");
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_int(net, 1);
  tbe_net_add_int(net, 2);
  tbe_net_add_int(net, 3);

  TBE_REL_INIT(rel, 1, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_BEF);
  TBE_REL_ADD(rel, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 2, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 3, 1, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

#if 0
  tbe_net_create(&net);
  tbe_net_add_int(net, 1);
  tbe_net_add_int(net, 2);
  tbe_net_add_int(net, 3);

  TBE_REL_INIT(rel, 1, 2, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  TBE_REL_INIT(rel, 2, 3, TBE_REL_SET_NUL);
  TBE_REL_ADD(rel, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(net, rel));

  tbe_net_dump2(net, stdout);
  tbe_net_destroy(&net);
#endif

  return 0;
}
