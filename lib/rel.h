#ifndef __TBE_REL_H
#define __TBE_REL_H

#include <tribe/tribe.h>

/* equal */
#define TBE_REL_EQL 0
/* before */
#define TBE_REL_BEF 1
/* inverse before (after) */
#define TBE_REL_BEI 2
/* during */
#define TBE_REL_DUR 3
/* inverse during (contains) */
#define TBE_REL_DUI 4
/* overlaps */
#define TBE_REL_OVR 5
/* inverse overlaps (overlapped by) */
#define TBE_REL_OVI 6
/* meets */
#define TBE_REL_MET 7
/* inverse meets (met by) */
#define TBE_REL_MEI 8
/* starts */
#define TBE_REL_STA 9
/* inverse starts (started by) */
#define TBE_REL_STI 10
/* finishes */
#define TBE_REL_FIN 11
/* inverse finishes (finished by) */
#define TBE_REL_FII 12

/* strings */
#define TBE_STR_NUL "no-relation"
#define TBE_STR_EQL "equals"
#define TBE_STR_BEF "before"
#define TBE_STR_BEI "after"
#define TBE_STR_DUR "during"
#define TBE_STR_DUI "contains"
#define TBE_STR_OVR "overlaps"
#define TBE_STR_OVI "overlapped-by"
#define TBE_STR_MET "meets"
#define TBE_STR_MEI "met-by"
#define TBE_STR_STA "starts"
#define TBE_STR_STI "started-by"
#define TBE_STR_FIN "finishes"
#define TBE_STR_FII "finished-by"

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

/* A r1 B,  B r2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_lookup(unsigned int a_r1, unsigned int a_r2);
/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_set_lookup(unsigned int a_rs1, unsigned int a_rs2);
/* returns a rel set that is the inverse of the given rel set */
unsigned int tbe_rel_set_inverse(unsigned int a_rs);
/* print all relations in rel set a_rs into stream a_stream */
int tbe_rel_set_dump(unsigned int a_rs, FILE *a_stream);

#endif
