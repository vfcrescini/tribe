#include <stdio.h>
#include "rel.h"

int main(int argc, char *argv[])
{
  unsigned int rs1;
  unsigned int rs2;

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

  printf("\n");

  return 0;
}
