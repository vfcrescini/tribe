/*
 * trans.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANS_H
#define __EPI_TRANS_H

#include <stringlist.h>
#include <expression.h>

typedef struct 
{
  char *name;
  stringlist_type varlist;
  expression_type precond;
  expression_type postcond;
} trans_type;

/* creates a newly-allocated trans; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int trans_create(trans_type **trans, 
                 char *name, 
		 stringlist_type varlist,
                 expression_type precond, 
                 expression_type postcond);

/* frees this trans and the name */
int trans_destroy(trans_type *trans);

/* returns 0 if the two are equivalent */
int trans_compare(trans_type t1, trans_type t2);

#endif
