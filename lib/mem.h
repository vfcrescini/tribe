#ifndef __TBE_MEM_H
#define __TBE_MEM_H

#ifdef __cplusplus
  #include <cstdlib>
#else
  #include <stdlib.h>
#endif

#include <tribe/tribe.h>

#ifdef TBE_DEBUG
  /* string prefixed to mem debug output */
  #define TBE_MEM_PREFIX_STR "TBE_MEM"
#endif

#ifdef TBE_DEBUG
  /* our own debug versions */
  TBE_EXTERN void *tbe_malloc(size_t a_size);
  TBE_EXTERN void *tbe_realloc(void *a_ptr, size_t a_size);
  TBE_EXTERN void tbe_free(void *a_ptr);
#endif

/* malloc(element type, number of elements) */
#ifdef TBE_DEBUG
  #define TBE_MEM_MALLOC(X,Y) ((X *) tbe_malloc(sizeof(X) * (Y)))
#else
  #define TBE_MEM_MALLOC(X,Y) ((X *) malloc(sizeof(X) * (Y)))
#endif

/* realloc(pointer, element type, number of elements) */
#ifdef TBE_DEBUG
  #define TBE_MEM_REALLOC(X,Y,Z) ((Y *) tbe_realloc(X, sizeof(Y) * (Z)))
#else
  #define TBE_MEM_REALLOC(X,Y,Z) ((Y *) realloc(X, sizeof(Y) * (Z)))
#endif
 
/* free(ptr) */
#ifdef TBE_DEBUG
  #define TBE_MEM_FREE(X) (tbe_free(X))
#else
  #define TBE_MEM_FREE(X) (free(X))
#endif

#endif
