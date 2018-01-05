#ifndef __TBE_TRIBE_H
#define __TBE_TRIBE_H

#include <tribe/config.h>

/* error codes */

#define TBE_OK                   0
/* general failure error code */
#define TBE_FAILURE             -1
/* unexpected null pointer */
#define TBE_NULLPTR             -2
/* call to malloc failed */
#define TBE_MALLOCFAILED        -3
/* index given is out of bounds */
#define TBE_OUTOFBOUNDS         -4
/* find functions: search failed */
#define TBE_NOTFOUND            -5
/* add functions: element already exists in an all-unique list */
#define TBE_DUPLICATE           -6
/* there is something wrong with the input parameters */
#define TBE_INVALIDINPUT        -7
/* function failed because the object was not initialised */
#define TBE_UNINITIALISED       -8
/* cannot open file */
#define TBE_OPENFAILED          -9
/* operation not permitted at this time */
#define TBE_INVALIDOP           -10

/* some macros */

/* returns the minimum of the two values */
#define TBE_INT_MIN(X,Y) ((X) <= (Y) ? (X) : (Y))
/* returns the maximum of the two values */
#define TBE_INT_MAX(X,Y) ((X) >= (Y) ? (X) : (Y))

#endif
