#include <stdio.h>
#include <tribe/tribe.h>
#include <tribe/rel.h>
#include <tribe/network.h>

int main(int argc, char *argv[])
{
  unsigned int rs;
  tbe_net net;
#if 0
  tbe_net_init(&net);
  tbe_net_add_int(&net, 1);
  tbe_net_add_int(&net, 2);
  tbe_net_add_int(&net, 3);
  tbe_net_add_int(&net, 4);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);
  TBE_REL_SET_ADD(rs, TBE_REL_MEI);
  TBE_REL_SET_ADD(rs, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(&net, 2, 3, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(&net, 3, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(&net, 4, 2, rs));

  TBE_REL_SET_FILL(rs);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_purge(&net);
#endif

  tbe_net_init(&net);
  tbe_net_add_int(&net, 1);
  tbe_net_add_int(&net, 2);
  tbe_net_add_int(&net, 3);
  tbe_net_add_int(&net, 4);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(&net, 4, 1, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_OVR);

  printf("adding: %d\n", tbe_net_add_rel(&net, 4, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(&net, 2, 3, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_STA);
  TBE_REL_SET_ADD(rs, TBE_REL_MET);

  printf("adding: %d\n", tbe_net_add_rel(&net, 4, 3, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_FIN);
  TBE_REL_SET_ADD(rs, TBE_REL_FII);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 3, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_purge(&net);


#if 0
  tbe_net_init(&net);
  tbe_net_add_int(&net, 1);
  tbe_net_add_int(&net, 2);
  tbe_net_add_int(&net, 3);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs, TBE_REL_BEI);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_rel(&net, 2, 3, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_BEF);

  printf("adding: %d\n", tbe_net_add_rel(&net, 3, 1, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_purge(&net);
#endif
#if 0
  tbe_net_init(&net);
  tbe_net_add_int(&net, 1);
  tbe_net_add_int(&net, 2);
  tbe_net_add_int(&net, 3);

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  TBE_REL_SET_CLEAR(rs);
  TBE_REL_SET_ADD(rs, TBE_REL_DUI);

  printf("adding: %d\n", tbe_net_add_rel(&net, 2, 3, rs));


  tbe_net_dump2(net, stdout);
  tbe_net_purge(&net);
#endif

  return 0;
}
