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

  tbe_print_rel_set(stdout, tbe_lookup_rel_set(rs1, rs2));

  printf("-----------------\n");

  TBE_REL_SET_CLR(rs1);
  TBE_REL_SET_CLR(rs2);

  TBE_REL_SET_ADD(rs1, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs1, TBE_REL_MET);

  TBE_REL_SET_ADD(rs2, TBE_REL_OVR);
  TBE_REL_SET_ADD(rs2, TBE_REL_STA);

  tbe_print_rel_set(stdout, tbe_lookup_rel_set(rs1, rs2));

#if 0
  tbe_print_rel_set(stdout, tbe_lookup_rel(TBE_REL_MET, TBE_REL_MEI));
#endif

  return 0;
}
