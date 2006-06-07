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

#ifndef __TBE_INTERVAL_H
#define __TBE_INTERVAL_H

typedef struct {
  unsigned int ep_1;
  unsigned int ep_2;
  unsigned char ep_mask;
} tbe_interval;

/* interval type */
#define TBE_INTERVAL_INTRNL 0
#define TBE_INTERVAL_EXTRNL 1

/* masks for interval flags */
#define TBE_INTERVAL_EP_1   1
#define TBE_INTERVAL_EP_2   2
#define TBE_INTERVAL_EP_NUL 0
#define TBE_INTERVAL_EP_ALL (TBE_INTERVAL_EP_1 | TBE_INTERVAL_EP_2)

/* useful macros */
#define TBE_INTERVAL_EP_ISCLEAR(X) \
  (((X).ep_mask & TBE_INTERVAL_EP_ALL) == TBE_INTERVAL_EP_NUL)
#define TBE_INTERVAL_EP_ISFILL(X) \
  (((X).ep_mask & TBE_INTERVAL_EP_ALL) == TBE_INTERVAL_EP_ALL)
#define TBE_INTERVAL_EP_ISSET(X,Y) \
  (((X).ep_mask & (Y)) == (Y))

#define TBE_INTERVAL_ISVALID(X) \
  (TBE_INTERVAL_EP_ISFILL(X) ? ((X).ep_1 < (X).ep_2) : 1)
#define TBE_INTERVAL_ISEQUAL(X,Y) \
  ( \
    ((X).ep_1 == (Y).ep_1) && \
    ((X).ep_2 == (Y).ep_2) && \
    ((X).ep_mask == (Y).ep_mask) \
  )
#define TBE_INTERVAL_CLEAR(X) \
  (X).ep_1 = 0; (X).ep_2 = 0; (X).ep_mask = TBE_INTERVAL_EP_NUL
#define TBE_INTERVAL_INIT(X,Y1,Y2,Y3) \
  (X).ep_1 = (Y1); \
  (X).ep_2 = (Y2); \
  (X).ep_mask = (Y3)

#define TBE_INTERVAL_PREFIX(X) (((X) == TBE_INTERVAL_INTRNL) ? "I" : "E")

#endif
