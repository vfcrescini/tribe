/*
 * This file is part of Tribe.
 *
 * Copyright (C) 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
 *
 * Tribe is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tribe is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tribe; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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

  if (a_ptr)
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
