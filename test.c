#include <stdio.h>
#include "rel.h"
#include "network.h"

int main(int argc, char *argv[])
{
  unsigned int rs1;
  unsigned int rs2;

  tbe_net net;

  TBE_REL_SET_CLR(rs1);
  TBE_REL_SET_CLR(rs2);

  TBE_REL_SET_ADD(rs1, TBE_REL_OVI);
  TBE_REL_SET_ADD(rs1, TBE_REL_MEI);

  TBE_REL_SET_ADD(rs2, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs2, TBE_REL_BEI);
  TBE_REL_SET_ADD(rs2, TBE_REL_MET);
  TBE_REL_SET_ADD(rs2, TBE_REL_MEI);

  tbe_rel_set_dump(tbe_rel_set_lookup(rs1, rs2), stdout);

  printf("\n");

  TBE_REL_SET_CLR(rs1);
  TBE_REL_SET_CLR(rs2);

  TBE_REL_SET_ADD(rs1, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs1, TBE_REL_MET);

  TBE_REL_SET_ADD(rs2, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs2, TBE_REL_STA);

  tbe_rel_set_dump(tbe_rel_set_lookup(rs1, rs2), stdout);

  printf("\n\n");

  TBE_REL_SET_CLR(rs1);
  TBE_REL_SET_ADD(rs1, TBE_REL_BEF);
  TBE_REL_SET_ADD(rs1, TBE_REL_BEI);

  TBE_REL_SET_CLR(rs2);
  TBE_REL_SET_ADD(rs2, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs2, TBE_REL_OVI);
  TBE_REL_SET_ADD(rs2, TBE_REL_MET);
  TBE_REL_SET_ADD(rs2, TBE_REL_MEI);

  tbe_net_init(&net);
  tbe_net_int_add(&net, 1);
  tbe_net_int_add(&net, 2);
  tbe_net_int_add(&net, 3);
  tbe_net_rel_add(&net, 2, 1, rs1);
  tbe_net_rel_add(&net, 2, 3, rs2);

  tbe_net_dump(net, stdout);

  tbe_net_purge(&net);

  return 0;
}
