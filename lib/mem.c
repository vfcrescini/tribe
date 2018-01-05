#include <stdlib.h>
#include <stdio.h>
#include <tribe/mem.h>

#ifdef TBE_DEBUG
void *tbe_malloc(size_t a_size)
{
  void *ptr = NULL;

  ptr = malloc(a_size);

  fprintf(stderr, "%s M %p\n", TBE_MEM_PREFIX_STR, ptr);

  return ptr;
}

void *tbe_realloc(void *a_ptr, size_t a_size)
{
  void *ptr = NULL;

  /* the exact behaviour of realloc is different on different systems when
   * given a NULL ptr or 0 size or both. since free(NULL) is legal, we simply
   * treat all calls to realloc as a call to "free" then "malloc" */

  fprintf(stderr, "%s F %p\n", TBE_MEM_PREFIX_STR, a_ptr);

  ptr = realloc(a_ptr, a_size);
  
  fprintf(stderr, "%s M %p\n", TBE_MEM_PREFIX_STR, ptr);
  
  return ptr;
}

void tbe_free(void *a_ptr)
{
  fprintf(stderr, "%s F %p\n", TBE_MEM_PREFIX_STR, a_ptr);

  free(a_ptr);
}
#endif
