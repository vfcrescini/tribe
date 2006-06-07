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
