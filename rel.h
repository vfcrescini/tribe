#ifndef __TBE_REL_H
#define __TBE_REL_H

#include "tribe.h"

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

/* clear set X */
#define TBE_REL_SET_CLR(X) ((X) = 0)
/* add relation Y to set X */
#define TBE_REL_SET_ADD(X,Y) (X) = (X) | (1 << (Y))
/* remove relation Y from set X */
#define TBE_REL_SET_DEL(X,Y) (X) = (X) & ~(1 << (Y))
/* returns union of set X and set Y */
#define TBE_REL_SET_UNI(X,Y) ((X) | (Y))
/* returns intersection of set X and set Y */
#define TBE_REL_SET_INT(X,Y) ((X) & (Y))
/* returns non-zero if relation Y is in set X */
#define TBE_REL_SET_IN(X,Y) ((X) & (1 << (Y)))

/* strings */
#define TBE_STR_NUL "no relation"
#define TBE_STR_EQL "equals"
#define TBE_STR_BEF "before"
#define TBE_STR_BEI "after"
#define TBE_STR_DUR "during"
#define TBE_STR_DUI "contains"
#define TBE_STR_OVR "overlaps"
#define TBE_STR_OVI "overlapped by"
#define TBE_STR_MET "meets"
#define TBE_STR_MEI "met by"
#define TBE_STR_STA "starts"
#define TBE_STR_STI "started by"
#define TBE_STR_FIN "finishes"
#define TBE_STR_FII "finished by"

/* print all relations in rel set a_rs into stream a_stream */
int tbe_print_rel_set(FILE *a_stream, unsigned int a_rs);
/* A r1 B,  B r2 C --> A rs3 C, return rs3 */
unsigned int tbe_lookup_rel(unsigned int a_r1, unsigned int a_r2);
/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_lookup_rel_set(unsigned int a_rs1, unsigned int a_rs2);

#endif
