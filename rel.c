#include <stdio.h>
#include "rel.h"

/* represent relation sets as unsigned ints, with each rels as bits */
#define TBE_REL_BNUL (0)
#define TBE_REL_BEQL (1 << TBE_REL_EQL)
#define TBE_REL_BBEF (1 << TBE_REL_BEF)
#define TBE_REL_BBEI (1 << TBE_REL_BEI)
#define TBE_REL_BDUR (1 << TBE_REL_DUR)
#define TBE_REL_BDUI (1 << TBE_REL_DUI)
#define TBE_REL_BOVR (1 << TBE_REL_OVR)
#define TBE_REL_BOVI (1 << TBE_REL_OVI)
#define TBE_REL_BMET (1 << TBE_REL_MET)
#define TBE_REL_BMEI (1 << TBE_REL_MEI)
#define TBE_REL_BSTA (1 << TBE_REL_STA)
#define TBE_REL_BSTI (1 << TBE_REL_STI)
#define TBE_REL_BFIN (1 << TBE_REL_FIN)
#define TBE_REL_BFII (1 << TBE_REL_FII)
#define TBE_REL_BALL ((1 << (TBE_REL_FII + 1)) - 1)

static int rel_table[13][13] = {
  /* equal */
  {
    TBE_REL_BEQL,
    TBE_REL_BBEF,
    TBE_REL_BBEI,
    TBE_REL_BDUR,
    TBE_REL_BDUI,
    TBE_REL_BOVR,
    TBE_REL_BOVI,
    TBE_REL_BMET,
    TBE_REL_BMEI,
    TBE_REL_BSTA,
    TBE_REL_BSTI,
    TBE_REL_BFIN,
    TBE_REL_BFII
  },
  /* before */
  {
    TBE_REL_BBEF,
    TBE_REL_BBEF,
    TBE_REL_BNUL,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BBEF,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BBEF,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF
  },
  /* inverse before (after) */
  {
    TBE_REL_BBEI,
    TBE_REL_BNUL,
    TBE_REL_BBEI,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BBEI,
    TBE_REL_BBEI
  },
  /* during */
  {
    TBE_REL_BDUR,
    TBE_REL_BBEF,
    TBE_REL_BBEI,
    TBE_REL_BDUR,
    TBE_REL_BNUL,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEF,
    TBE_REL_BBEI,
    TBE_REL_BDUR,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BDUR,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUR | TBE_REL_BSTA
  },
  /* inverse during (contains) */
  {
    TBE_REL_BDUI,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BMEI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BDUI | TBE_REL_BEQL,
    TBE_REL_BDUI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BDUI,
    TBE_REL_BDUI | TBE_REL_BSTI | TBE_REL_BOVI,
    TBE_REL_BDUI
  },
  /* overlaps */
  {
    TBE_REL_BOVR,
    TBE_REL_BBEF,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BMEI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET,
    TBE_REL_BOVR | TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BDUI | TBE_REL_BEQL,
    TBE_REL_BBEF,
    TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR,
    TBE_REL_BDUI | TBE_REL_BFII | TBE_REL_BOVR,
    TBE_REL_BDUR | TBE_REL_BSTA | TBE_REL_BOVR,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET
  },
  /* inverse overlap (overlapped by) */
  {
    TBE_REL_BOVI,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEI,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BDUI | TBE_REL_BEQL,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEI,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BOVI | TBE_REL_BBEI | TBE_REL_BMEI,
    TBE_REL_BOVI,
    TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BSTI
  },
  /* meets */
  {
    TBE_REL_BMET,
    TBE_REL_BBEF,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BBEF,
    TBE_REL_BOVR | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BFIN | TBE_REL_BFII | TBE_REL_BEQL,
    TBE_REL_BMET,
    TBE_REL_BMET,
    TBE_REL_BDUR | TBE_REL_BSTA | TBE_REL_BOVR,
    TBE_REL_BBEF
  },
  /* inverse meets (met by) */
  {
    TBE_REL_BMEI,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEI,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEI,
    TBE_REL_BSTA | TBE_REL_BSTI | TBE_REL_BEQL,
    TBE_REL_BBEI,
    TBE_REL_BDUR | TBE_REL_BFIN | TBE_REL_BOVI,
    TBE_REL_BBEI,
    TBE_REL_BMEI,
    TBE_REL_BMEI
  },
  /* starts */
  {
    TBE_REL_BSTA,
    TBE_REL_BBEF,
    TBE_REL_BBEI,
    TBE_REL_BDUR,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BBEF,
    TBE_REL_BMEI,
    TBE_REL_BSTA,
    TBE_REL_BSTA | TBE_REL_BSTI | TBE_REL_BEQL,
    TBE_REL_BDUR,
    TBE_REL_BBEF | TBE_REL_BMET | TBE_REL_BOVR,
  },
  /* inverse starts (started by) */
  {
    TBE_REL_BSTI,
    TBE_REL_BBEF | TBE_REL_BOVR | TBE_REL_BMET | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BBEI,
    TBE_REL_BOVI | TBE_REL_BDUR | TBE_REL_BFIN,
    TBE_REL_BDUI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BOVI,
    TBE_REL_BOVR | TBE_REL_BDUI | TBE_REL_BFII,
    TBE_REL_BMEI,
    TBE_REL_BSTA | TBE_REL_BSTI | TBE_REL_BEQL,
    TBE_REL_BSTI,
    TBE_REL_BOVI,
    TBE_REL_BDUI
  },
  /* finishes */
  {
    TBE_REL_BFIN,
    TBE_REL_BBEF,
    TBE_REL_BBEI,
    TBE_REL_BDUR,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI,
    TBE_REL_BMET,
    TBE_REL_BBEI,
    TBE_REL_BDUI,
    TBE_REL_BBEI | TBE_REL_BOVI | TBE_REL_BMEI,
    TBE_REL_BFII,
    TBE_REL_BFIN | TBE_REL_BFII | TBE_REL_BEQL
  },
  /* inverse finishes (finished by) */
  {
    TBE_REL_BFII,
    TBE_REL_BBEF,
    TBE_REL_BBEI | TBE_REL_BOVR | TBE_REL_BMEI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BOVR | TBE_REL_BDUR | TBE_REL_BSTA,
    TBE_REL_BDUI,
    TBE_REL_BOVR,
    TBE_REL_BOVI | TBE_REL_BDUI | TBE_REL_BSTI,
    TBE_REL_BMET,
    TBE_REL_BSTI | TBE_REL_BOVI | TBE_REL_BDUI,
    TBE_REL_BOVR,
    TBE_REL_BDUI,
    TBE_REL_BFIN | TBE_REL_BFII | TBE_REL_BEQL,
    TBE_REL_BFII
  }
};

/* print all relations in rel set a_rs into stream a_stream */
int tbe_print_rel_set(FILE *a_stream, unsigned int a_rs)
{
  if (a_rs > TBE_REL_BALL)
    return TBE_INVALIDINPUT;

  if (!a_stream)
    return TBE_NULLPTR;

  if (TBE_REL_SET_IN(a_rs, TBE_REL_EQL))
    fprintf(a_stream, "%s\n", TBE_STR_EQL);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_BEF))
    fprintf(a_stream, "%s\n", TBE_STR_BEF);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_BEI))
    fprintf(a_stream, "%s\n", TBE_STR_BEI);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_DUR))
    fprintf(a_stream, "%s\n", TBE_STR_DUR);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_DUI))
    fprintf(a_stream, "%s\n", TBE_STR_DUI);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_OVR))
    fprintf(a_stream, "%s\n", TBE_STR_OVR);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_OVI))
    fprintf(a_stream, "%s\n", TBE_STR_OVI);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_MET))
    fprintf(a_stream, "%s\n", TBE_STR_MET);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_MEI))
    fprintf(a_stream, "%s\n", TBE_STR_MEI);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_STA))
    fprintf(a_stream, "%s\n", TBE_STR_STA);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_STI))
    fprintf(a_stream, "%s\n", TBE_STR_STI);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_FIN))
    fprintf(a_stream, "%s\n", TBE_STR_FIN);
  if (TBE_REL_SET_IN(a_rs, TBE_REL_FII))
    fprintf(a_stream, "%s\n", TBE_STR_FII);

  return TBE_OK;
}

/* A r1 B,  B r2 C --> A rs3 C, return rs3 */
unsigned int tbe_lookup_rel(unsigned int a_r1, unsigned int a_r2)
{
  if (a_r1 > TBE_REL_FII || a_r2 > TBE_REL_FII)
    return TBE_REL_BNUL;

  return rel_table[a_r1][a_r2];
}

/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_lookup_rel_set(unsigned int a_rs1, unsigned int a_rs2)
{
  int i;
  int j;
  unsigned int retval;

  TBE_REL_SET_CLR(retval);

  if (a_rs1 > TBE_REL_BALL || a_rs2 > TBE_REL_BALL || !a_rs1 || !a_rs2)
    return retval;

  for (i = TBE_REL_EQL; i <= TBE_REL_FII; i++) {
    /* if this bit is not set, move on to the next one */
    if ((a_rs1 & (1 << i)) == TBE_REL_BNUL)
      continue;
    for (j = TBE_REL_EQL; j <= TBE_REL_FII; j++) {
      /* if this bit is not set, move on to the next one */
      if ((a_rs2 & (1 << j)) == TBE_REL_BNUL)
        continue;
      /* ok. so at this point both bit i and bit j are set */
      retval = TBE_REL_SET_UNI(retval, tbe_lookup_rel(i, j));
    }
  }

  return retval;
}
