#include <stdio.h>
#include "rel.h"
#include "network.h"

int main(int argc, char *argv[])
{
  unsigned int rs1;
  unsigned int rs2;

  tbe_net_list netlist;
  tbe_net_rel_list netrellist;

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

  tbe_net_rel_list_init(&netrellist);
  tbe_net_rel_list_add(&netrellist, 2, rs1);
  tbe_net_rel_list_add(&netrellist, 3, rs2);

  tbe_net_list_init(&netlist);
  tbe_net_list_add(&netlist, 1, &netrellist);

  tbe_net_list_dump(netlist, stdout);

  tbe_net_list_purge(&netlist);

  return 0;
}
