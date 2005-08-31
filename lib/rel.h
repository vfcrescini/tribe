#ifndef __TBE_REL_H
#define __TBE_REL_H

#include <stdio.h>
#include <tribe/tribe.h>
#include <tribe/interval.h>

/* a relation is two interval id's and a relset between them */
typedef struct {
  unsigned int id1;
  unsigned int id2;
  unsigned int rs;
} tbe_rel;

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
/* returns a rel set containing no relations */
#define TBE_REL_SET_NUL (0)
/* returns a rel set containing all relations */
#define TBE_REL_SET_ALL ((1 << (TBE_REL_FII + 1)) - 1)

/* more macros */

/* initialise relation with the given values */
#define TBE_REL_INIT(X,Y1,Y2,Y3) \
  (X).id1 = (Y1); \
  (X).id2 = (Y2); \
  (X).rs      = (Y3)
/* set values of relation to 0 */
#define TBE_REL_CLEAR(X) \
  (X).id1 = 0; \
  (X).id2 = 0; \
  TBE_REL_SET_CLEAR((X).rs)
/* add a relation to the relset of the relation structure */
#define TBE_REL_ADD(X,Y) \
  TBE_REL_SET_ADD((X).rs, (Y))
/* returns non-zero if the two relations are equal */
#define TBE_REL_ISEQL(X,Y) \
  ( \
    (X).id1 == (Y).id1 && \
    (X).id2 == (Y).id2 && \
    (X).rs == (Y).rs \
  )

/* create a new rel structure */
int tbe_rel_create(tbe_rel **a_rel);

/* destroy an existing rel structure */
void tbe_rel_destroy(tbe_rel *a_rel);

/* as above, but with a void ptr */
void tbe_rel_free(void *a_rel);

/* A rs1 B, B rs2 C --> A rs3 C, return rs3 */
unsigned int tbe_rel_trans(unsigned int a_rs1, unsigned int a_rs2);

/* returns a rel set that is the inverse of the given rel set */
unsigned int tbe_rel_inverse(unsigned int a_rs);

/* returns the relset between the 2 given intervals */
unsigned int tbe_rel_calc(tbe_interval a_int1, tbe_interval a_int2);

/* normalise the relation. a relation A rs B is normalised if A <= B */
int tbe_rel_normalise(tbe_rel *a_rel);

/* print all relations in rel set a_rs into stream a_stream */
int tbe_rel_dump(unsigned int a_rs, FILE *a_stream);

#endif
