#include <stdlib.h>
#include <stdio.h>

#include "rel.h"

/* relsets are unsigned ints. each rel is a bit mask */
#define TBE_REL_MSK_NUL (0)
#define TBE_REL_MSK_EQL (1 << TBE_REL_EQL)
#define TBE_REL_MSK_BEF (1 << TBE_REL_BEF)
#define TBE_REL_MSK_BEI (1 << TBE_REL_BEI)
#define TBE_REL_MSK_DUR (1 << TBE_REL_DUR)
#define TBE_REL_MSK_DUI (1 << TBE_REL_DUI)
#define TBE_REL_MSK_OVR (1 << TBE_REL_OVR)
#define TBE_REL_MSK_OVI (1 << TBE_REL_OVI)
#define TBE_REL_MSK_MET (1 << TBE_REL_MET)
#define TBE_REL_MSK_MEI (1 << TBE_REL_MEI)
#define TBE_REL_MSK_STA (1 << TBE_REL_STA)
#define TBE_REL_MSK_STI (1 << TBE_REL_STI)
#define TBE_REL_MSK_FIN (1 << TBE_REL_FIN)
#define TBE_REL_MSK_FII (1 << TBE_REL_FII)
#define TBE_REL_MSK_ALL ((1 << (TBE_REL_FII + 1)) - 1)

static int tbe_rel_table[13][13] = {
  /* equal */
  {
    TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STA,
    TBE_REL_MSK_STI,
    TBE_REL_MSK_FIN,
    TBE_REL_MSK_FII
  },
  /* before */
  {
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF
  },
  /* inverse before (after) */
  {
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI
  },
  /* during */
  {
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA
  },
  /* inverse during (contains) */
  {
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_MEI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_OVI,
    TBE_REL_MSK_DUI
  },
  /* overlaps */
  {
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_MEI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_DUI | TBE_REL_MSK_FII | TBE_REL_MSK_OVR,
    TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET
  },
  /* inverse overlap (overlapped by) */
  {
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_OVI | TBE_REL_MSK_BEI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI
  },
  /* meets */
  {
    TBE_REL_MSK_MET,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEF
  },
  /* inverse meets (met by) */
  {
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR | TBE_REL_MSK_FIN | TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_MEI
  },
  /* starts */
  {
    TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STA,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_MET | TBE_REL_MSK_OVR,
  },
  /* inverse starts (started by) */
  {
    TBE_REL_MSK_STI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_STI,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_DUI
  },
  /* finishes */
  {
    TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_FII,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL
  },
  /* inverse finishes (finished by) */
  {
    TBE_REL_MSK_FII,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVR | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_STI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_FII
  }
};

/* A r1 B,  B r2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_lookup(unsigned int a_r1, unsigned int a_r2)
{
  if (a_r1 > TBE_REL_FII || a_r2 > TBE_REL_FII)
    return TBE_REL_MSK_NUL;

  return tbe_rel_table[a_r1][a_r2];
}

/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_set_lookup(unsigned int a_rs1, unsigned int a_rs2)
{
  int i;
  int j;
  unsigned int retval;

  TBE_REL_SET_CLEAR(retval);

  if (a_rs1 > TBE_REL_MSK_ALL || a_rs2 > TBE_REL_MSK_ALL || !a_rs1 || !a_rs2)
    return retval;

  for (i = TBE_REL_EQL; i <= TBE_REL_FII; i++) {
    /* if this bit is not set, move on to the next one */
    if (!(a_rs1 & (1 << i)))
      continue;
    for (j = TBE_REL_EQL; j <= TBE_REL_FII; j++) {
      /* if this bit is not set, move on to the next one */
      if (!(a_rs2 & (1 << j)))
        continue;
      /* ok. so at this point both bit i and bit j are set */
      retval = TBE_REL_SET_UNION(retval, tbe_rel_lookup(i, j));
    }
  }

  return retval;
}

/* returns a rel set that is the inverse of the given rel set */
unsigned int tbe_rel_set_inverse(unsigned int a_rs)
{
  unsigned int rs;

  TBE_REL_SET_CLEAR(rs);

  /* if it's everthing, don't bother */
  if (TBE_REL_SET_ISFILL(a_rs))
    return a_rs;

  /* equals: always reflexive */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_EQL))
    TBE_REL_SET_ADD(rs, TBE_REL_EQL);

  /* before and after */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEF))
    TBE_REL_SET_ADD(rs, TBE_REL_BEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEI))
    TBE_REL_SET_ADD(rs, TBE_REL_BEF);

  /* during and contains */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUR))
    TBE_REL_SET_ADD(rs, TBE_REL_DUI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUI))
    TBE_REL_SET_ADD(rs, TBE_REL_DUR);

  /* overlaps and overlapped-by */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVR))
    TBE_REL_SET_ADD(rs, TBE_REL_OVI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVI))
    TBE_REL_SET_ADD(rs, TBE_REL_OVR);

  /* meets and met-by */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MET))
    TBE_REL_SET_ADD(rs, TBE_REL_MEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MEI))
    TBE_REL_SET_ADD(rs, TBE_REL_MET);

  /* starts and started-by */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STA))
    TBE_REL_SET_ADD(rs, TBE_REL_STI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STI))
    TBE_REL_SET_ADD(rs, TBE_REL_STA);

  /* finishes and finished-by */
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FIN))
    TBE_REL_SET_ADD(rs, TBE_REL_FII);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FII))
    TBE_REL_SET_ADD(rs, TBE_REL_FIN);

  return rs;
}

/* print all relations in rel set a_rs into stream a_stream */
int tbe_rel_set_dump(unsigned int a_rs, FILE *a_stream)
{
  if (a_rs > TBE_REL_MSK_ALL)
    return TBE_INVALIDINPUT;

  if (!a_stream)
    return TBE_NULLPTR;

  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_EQL))
    fprintf(a_stream, "%s ", TBE_STR_EQL);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEF))
    fprintf(a_stream, "%s ", TBE_STR_BEF);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEI))
    fprintf(a_stream, "%s ", TBE_STR_BEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUR))
    fprintf(a_stream, "%s ", TBE_STR_DUR);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUI))
    fprintf(a_stream, "%s ", TBE_STR_DUI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVR))
    fprintf(a_stream, "%s ", TBE_STR_OVR);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVI))
    fprintf(a_stream, "%s ", TBE_STR_OVI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MET))
    fprintf(a_stream, "%s ", TBE_STR_MET);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MEI))
    fprintf(a_stream, "%s ", TBE_STR_MEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STA))
    fprintf(a_stream, "%s ", TBE_STR_STA);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STI))
    fprintf(a_stream, "%s ", TBE_STR_STI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FIN))
    fprintf(a_stream, "%s ", TBE_STR_FIN);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FII))
    fprintf(a_stream, "%s ", TBE_STR_FII);

  return TBE_OK;
}
