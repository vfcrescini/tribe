/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TBE_RQUEUE_H
#define __TBE_RQUEUE_H

#include <tribe/tribe.h>
#include <tribe/list.h>

typedef tbe_list tbe_rqueue;

/* create a new rqueue */
TBE_EXTERN int tbe_rqueue_create(tbe_rqueue *a_rqueue);

/* destroy the queue */
TBE_EXTERN void tbe_rqueue_destroy(tbe_rqueue *a_rqueue);

/* purge the queue */
TBE_EXTERN void tbe_rqueue_purge(tbe_rqueue a_rqueue);

/* add rel at the end of the queue (rels get normalised) */
TBE_EXTERN int tbe_rqueue_enq(tbe_rqueue a_rqueue,
                              unsigned int a_id1,
                              unsigned int a_id2,
                              unsigned char a_type1,
                              unsigned char a_type2,
                              unsigned int a_rs);

/* remove rel from the front of the queue (and copy to given ptrs) */
TBE_EXTERN int tbe_rqueue_deq(tbe_rqueue a_rqueue,
                              unsigned int *a_id1,
                              unsigned int *a_id2,
                              unsigned char *a_type1,
                              unsigned char *a_type2,
                              unsigned int *a_rs);

#endif
