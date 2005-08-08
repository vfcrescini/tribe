#include <stdlib.h>
#include <stdio.h>

#include <tribe/rel.h>

/* relsets are unsigned ints. each rel is a bit mask */
#define TBE_REL_MSK_NUL (0)
#define TBE_REL_MSK_EQL (1 << TBE_REL_EQL)
#define TBE_REL_MSK_BEF (1 << TBE_REL_BEF)
#define TBE_REL_MSK_DUR (1 << TBE_REL_DUR)
#define TBE_REL_MSK_OVR (1 << TBE_REL_OVR)
#define TBE_REL_MSK_MET (1 << TBE_REL_MET)
#define TBE_REL_MSK_STA (1 << TBE_REL_STA)
#define TBE_REL_MSK_FIN (1 << TBE_REL_FIN)
#define TBE_REL_MSK_BEI (1 << TBE_REL_BEI)
#define TBE_REL_MSK_DUI (1 << TBE_REL_DUI)
#define TBE_REL_MSK_OVI (1 << TBE_REL_OVI)
#define TBE_REL_MSK_MEI (1 << TBE_REL_MEI)
#define TBE_REL_MSK_STI (1 << TBE_REL_STI)
#define TBE_REL_MSK_FII (1 << TBE_REL_FII)
#define TBE_REL_MSK_ALL ((1 << (TBE_REL_FII + 1)) - 1)

static int tbe_rel_table[13][13] = {
  /* equal */
  {
    TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_STA,
    TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STI,
    TBE_REL_MSK_FII
  },
  /* before */
  {
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF
  },
  /* during */
  {
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUR | TBE_REL_MSK_STA
  },
  /* overlaps */
  {
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_MEI | TBE_REL_MSK_STI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_DUI | TBE_REL_MSK_FII | TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET
  },
  /* meets */
  {
    TBE_REL_MSK_MET,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_OVR,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_BEF
  },
  /* starts */
  {
    TBE_REL_MSK_STA,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_STA,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEF | TBE_REL_MSK_MET | TBE_REL_MSK_OVR,
  },
  /* finishes */
  {
    TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_DUR,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_FII,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL
  },
  /* inverse before (after) */
  {
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_ALL,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI
  },
  /* inverse during (contains) */
  {
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_MEI | TBE_REL_MSK_STI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_DUI
  },
  /* inverse overlap (overlapped by) */
  {
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_OVR | TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_STA | TBE_REL_MSK_FIN | TBE_REL_MSK_DUI | TBE_REL_MSK_STI | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_BEI | TBE_REL_MSK_MEI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI
  },
  /* inverse meets (met by) */
  {
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_DUR | TBE_REL_MSK_FIN | TBE_REL_MSK_OVI,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_MEI
  },
  /* inverse starts (started by) */
  {
    TBE_REL_MSK_STI,
    TBE_REL_MSK_BEF | TBE_REL_MSK_OVR | TBE_REL_MSK_MET | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUR | TBE_REL_MSK_FIN,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUI | TBE_REL_MSK_FII,
    TBE_REL_MSK_STA | TBE_REL_MSK_STI | TBE_REL_MSK_EQL,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_BEI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVI,
    TBE_REL_MSK_MEI,
    TBE_REL_MSK_STI,
    TBE_REL_MSK_DUI
  },
  /* inverse finishes (finished by) */
  {
    TBE_REL_MSK_FII,
    TBE_REL_MSK_BEF,
    TBE_REL_MSK_OVR | TBE_REL_MSK_DUR | TBE_REL_MSK_STA,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_MET,
    TBE_REL_MSK_OVR,
    TBE_REL_MSK_FIN | TBE_REL_MSK_FII | TBE_REL_MSK_EQL,
    TBE_REL_MSK_BEI | TBE_REL_MSK_OVR | TBE_REL_MSK_MEI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_DUI,
    TBE_REL_MSK_OVI | TBE_REL_MSK_DUI | TBE_REL_MSK_STI,
    TBE_REL_MSK_STI | TBE_REL_MSK_OVI | TBE_REL_MSK_DUI,
    TBE_REL_MSK_DUI,
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
  unsigned int m1 = ((1 << TBE_REL_FIN) - 1) << 1;
  unsigned int m2 = ((1 << TBE_REL_FIN) - 1) << (TBE_REL_FIN + 1);
  unsigned int m3 = ~(((1 << TBE_REL_FII) - 1) << 1);

  return
    ((a_rs & m1) << TBE_REL_FIN) | ((a_rs & m2) >> TBE_REL_FIN) | (a_rs & m3);
}

/* gives the relation that exists between the 2 given intervals */
int tbe_rel_calc(unsigned int a_i1s,
                 unsigned int a_i1e,
                 unsigned int a_i2s,
                 unsigned int a_i2e,
                 unsigned int *a_rel)
{
  if (!a_rel)
    return TBE_NULLPTR;

  /* check if the intervals are valid */
  if (a_i1s >= a_i1e || a_i2s >= a_i2e)
    return TBE_INVALIDINPUT;

  /*
   * eql  a_i1s = a_i2s, a_i1e = a_i2e
   * bef  a_i1e < a_i2s
   * bei  a_i1s > a_i2e
   * dur  a_i1s > a_i2s, a_i1e < a_i2e
   * dui  a_i1s < a_i2s, a_i1e > a_i2e
   * ovr  a_i1s < a_i2s, a_i1e < a_i2e, a_i1e > a_i2s
   * ovi  a_i1s > a_i2s, a_i1e > a_i2e, a_i1s < a_i2e
   * met  a_i1e = a_i2s
   * mei  a_i1s = a_i2e
   * sta  a_i1s = a_i2s, a_i1e < a_i2e
   * sti  a_i1s = a_i2s, a_ie > a_i2e
   * fin  a_i1s > a_i2s, a_i1e = a_i2e
   * fii  a_i1s < a_i2s, a_i1e = a_i2e
   */

  if (a_i1s == a_i2s) {
    if (a_i1e == a_i2e)
      *a_rel = TBE_REL_EQL;
    else if (a_i1e < a_i2e)
      *a_rel = TBE_REL_STA;
    else
      *a_rel = TBE_REL_STI;
  }
  else if (a_i1s < a_i2s) {
    if (a_i1e == a_i2e)
      *a_rel = TBE_REL_FII;
    else if (a_i1e < a_i2e) {
      if (a_i1e == a_i2s)
        *a_rel = TBE_REL_MET;
      else if (a_i1e < a_i2s)
        *a_rel = TBE_REL_BEF;
      else
        *a_rel = TBE_REL_OVR;
    }
    else
      *a_rel = TBE_REL_DUI;
  }
  else {
    if (a_i1e == a_i2e)
      *a_rel = TBE_REL_FIN;
    else if (a_i1e < a_i2e)
      *a_rel = TBE_REL_DUR;
    else {
      if (a_i1s == a_i2e)
        *a_rel = TBE_REL_MEI;
      else if (a_i1s < a_i2e)
        *a_rel = TBE_REL_OVI;
      else
        *a_rel = TBE_REL_BEI;
    }
  }

  return TBE_OK;
}

/* print all relations in rel set a_rs into stream a_stream */
int tbe_rel_set_dump(unsigned int a_rs, FILE *a_stream)
{
  if (a_rs > TBE_REL_MSK_ALL)
    return TBE_INVALIDINPUT;

  if (!a_stream)
    return TBE_NULLPTR;

  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_EQL))
    fprintf(a_stream, "%s ", TBE_REL_STR_EQL);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEF))
    fprintf(a_stream, "%s ", TBE_REL_STR_BEF);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUR))
    fprintf(a_stream, "%s ", TBE_REL_STR_DUR);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVR))
    fprintf(a_stream, "%s ", TBE_REL_STR_OVR);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MET))
    fprintf(a_stream, "%s ", TBE_REL_STR_MET);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STA))
    fprintf(a_stream, "%s ", TBE_REL_STR_STA);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FIN))
    fprintf(a_stream, "%s ", TBE_REL_STR_FIN);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_BEI))
    fprintf(a_stream, "%s ", TBE_REL_STR_BEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_DUI))
    fprintf(a_stream, "%s ", TBE_REL_STR_DUI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_OVI))
    fprintf(a_stream, "%s ", TBE_REL_STR_OVI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_MEI))
    fprintf(a_stream, "%s ", TBE_REL_STR_MEI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_STI))
    fprintf(a_stream, "%s ", TBE_REL_STR_STI);
  if (TBE_REL_SET_ISIN(a_rs, TBE_REL_FII))
    fprintf(a_stream, "%s ", TBE_REL_STR_FII);

  return TBE_OK;
}
