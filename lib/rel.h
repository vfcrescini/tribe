#ifndef __TBE_REL_H
#define __TBE_REL_H

#include <tribe/tribe.h>

/* equal */
#define TBE_REL_EQL 0
/* before */
#define TBE_REL_BEF 1
/* during */
#define TBE_REL_DUR 2
/* overlaps */
#define TBE_REL_OVR 3
/* meets */
#define TBE_REL_MET 4
/* starts */
#define TBE_REL_STA 5
/* finishes */
#define TBE_REL_FIN 6
/* inverse before (after) */
#define TBE_REL_BEI 7
/* inverse during (contains) */
#define TBE_REL_DUI 8
/* inverse overlaps (overlapped by) */
#define TBE_REL_OVI 9
/* inverse meets (met by) */
#define TBE_REL_MEI 10
/* inverse starts (started by) */
#define TBE_REL_STI 11
/* inverse finishes (finished by) */
#define TBE_REL_FII 12

/* strings */
#define TBE_REL_STR_NUL "no-relation"
#define TBE_REL_STR_EQL "equals"
#define TBE_REL_STR_BEF "before"
#define TBE_REL_STR_DUR "during"
#define TBE_REL_STR_OVR "overlaps"
#define TBE_REL_STR_MET "meets"
#define TBE_REL_STR_STA "starts"
#define TBE_REL_STR_FIN "finishes"
#define TBE_REL_STR_BEI "after"
#define TBE_REL_STR_DUI "contains"
#define TBE_REL_STR_OVI "overlapped-by"
#define TBE_REL_STR_MEI "met-by"
#define TBE_REL_STR_STI "started-by"
#define TBE_REL_STR_FII "finished-by"

/* clear set X */
#define TBE_REL_SET_CLEAR(X) ((X) = 0)
/* fill set X with all possible relations */
#define TBE_REL_SET_FILL(X) ((X) = ((1 << (TBE_REL_FII + 1)) - 1))
/* add relation Y to set X */
#define TBE_REL_SET_ADD(X,Y) (X) = (X) | (1 << (Y))
/* remove relation Y from set X */
#define TBE_REL_SET_DEL(X,Y) (X) = (X) & ~(1 << (Y))
/* returns union of set X and set Y */
#define TBE_REL_SET_UNION(X,Y) ((X) | (Y))
/* returns intersection of set X and set Y */
#define TBE_REL_SET_INTERSECT(X,Y) ((X) & (Y))
/* returns non-zero if relation Y is in set X */
#define TBE_REL_SET_ISIN(X,Y) ((X) & (1 << (Y)))
/* returns non-zero if set X is empty */
#define TBE_REL_SET_ISCLEAR(X) ((X) == 0)
/* returns non-zero if set X contains all possible relations */
#define TBE_REL_SET_ISFILL(X) ((X) == (1 << (TBE_REL_FII + 1)) - 1)

/* masks for interval flags */
#define TBE_REL_I1S  1
#define TBE_REL_I1E  2
#define TBE_REL_I2S  4
#define TBE_REL_I2E  8
#define TBE_REL_INUL 0
#define TBE_REL_IALL (TBE_REL_I1S | TBE_REL_I1E | TBE_REL_I2S | TBE_REL_I2E)

/* A r1 B,  B r2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_lookup(unsigned int a_r1, unsigned int a_r2);
/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_set_lookup(unsigned int a_rs1, unsigned int a_rs2);
/* returns a rel set that is the inverse of the given rel set */
unsigned int tbe_rel_set_inverse(unsigned int a_rs);
/* returns the relset between the 2 given intervals. a_mask indicates which
 * endpoints are given, e.g. a_mask & TBE_REL_I1S != 0 means a_i1s is given */
unsigned int tbe_rel_calc(unsigned char a_mask,
                          unsigned int a_i1s,
                          unsigned int a_i1e,
                          unsigned int a_i2s,
                          unsigned int a_i2e);
/* print all relations in rel set a_rs into stream a_stream */
int tbe_rel_set_dump(unsigned int a_rs, FILE *a_stream);

#endif