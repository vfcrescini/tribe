/*
 * trans.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "trans.h"

/* creates a newly-allocated trans; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int trans_create(trans_type **trans,
                 char *name,
                 stringlist_type varlist,
                 expression_type precond,
                 expression_type postcond)
{
  if (trans == NULL || name == NULL)
    return -1;

  if ((*trans = (trans_type *) malloc(sizeof(trans_type))) == NULL)
    return -1;

  if (((*trans)->name = (char *) malloc(sizeof(char) * strlen(name))) == NULL) {
    free(*trans);
    return -1;
  }
  strcpy((*trans)->name, name);
  (*trans)->varlist = varlist;
  (*trans)->precond = precond;
  (*trans)->postcond = postcond;

  return 0;
}

/* frees this trans and the name */
int trans_destroy(trans_type *trans)
{
  if (trans == NULL)
    return -1;

  if (trans->name != NULL)
   free(trans->name);

  free(trans);
  trans = NULL;

  return 0;
}

/* returns 0 if the two are equivalent */
int trans_compare(trans_type t1, trans_type t2)
{
  return strcmp(t1.name, t2.name) == 0 && 
         stringlist_cmp(t1.varlist, t2.varlist) == 0 &&
         expression_cmp(t1.precond, t2.precond) == 0 &&
         expression_cmp(t1.postcond, t2.postcond) == 0;
}
