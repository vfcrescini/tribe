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

#ifndef __TBE_CLIST_H
#define __TBE_CLIST_H

#include <tribe/tribe.h>

typedef void *tbe_clist;

/* create a new clist */
int tbe_clist_create(tbe_clist *a_clist);

/* destroy the clist */
void tbe_clist_destroy(tbe_clist *a_clist);

/* purge the clist */
void tbe_clist_purge(tbe_clist a_clist);

/* add constraint */
int tbe_clist_add(tbe_clist a_clist,
                  unsigned int a_v1,
                  unsigned int a_v2,
                  unsigned int a_rs);

/* check that all variables in the list are under the given maximum */
int tbe_clist_verify(tbe_clist a_clist, unsigned int a_max);

/* return TBE_OK if the given tuple of the given size satisfies the given
 * clist, TBE_FAILURE if it doesn't, but no other error occurs. calls a_fn()
 * with the given parameter to get the relset of 2 intervals. each variable in
 * the clist is an index of the tuple. important: assumes a_tuple is of size
 * a_size. */
int tbe_clist_validate(tbe_clist a_clist,
                       unsigned int *a_tuple,
                       unsigned int a_size,
                       unsigned int (*a_fn)(unsigned int, unsigned int, void *),
                       void *a_parm);

#endif
