#include <stdio.h>
#include "rel.h"
#include "network.h"

int main(int argc, char *argv[])
{
  unsigned int rs;
  tbe_net net;

  tbe_net_init(&net);
  tbe_net_add_int(&net, 1);
  tbe_net_add_int(&net, 2);
  tbe_net_add_int(&net, 3);

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

  TBE_REL_SET_FILL(rs);

  printf("adding: %d\n", tbe_net_add_rel(&net, 1, 2, rs));

  tbe_net_dump2(net, stdout);
  tbe_net_purge(&net);

  return 0;
}
