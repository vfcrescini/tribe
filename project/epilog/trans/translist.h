/*
 * translist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSLIST_H
#define __EPI_TRANSLIST_H

#include <simplelist.h>
#include "transref.h"

#define translist_type simplelist_type

/* initialise list */
void translist_init(translist_type *list);

/* gives the length of the list */
unsigned int translist_length(translist_type list);

/* add into list given the transref_type structure */
int translist_add(translist_type *list, transref_type trans);

/* gives ar reference to the index'th transref */
int translist_get(translist_type list,
                  unsigned int index,
                  transref_type **trans);

/* empty the list along with its members */
void translist_purge(translist_type *list);

#endif