/* 
 * ident.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "ident.h"

/* create a new ident structure. allocate space for name */
int ident_create(ident_type **ident, char *name, unsigned short type)
{
  if (ident == NULL || name == NULL)
    return EPI_NULLPTR;

  if ((*ident = EPI_ADT_MALLOC(ident_type)) == NULL)
    return EPI_MALLOCFAILED;

  if (((*ident)->name = EPI_STRING_MALLOC(name)) == NULL) {
    free(*ident);
    return EPI_MALLOCFAILED;
  }

  strcpy((*ident)->name, name);
  (*ident)->type = type;

  return EPI_OK;
}

/* free ident struct, including allocated space for name */
void ident_destroy(ident_type *ident)
{
  if (ident != NULL) {
    if (ident->name != NULL)
      free(ident->name);

    free(ident);
  }
}

/* returns 0 if the contents of ident1 and ident2 are identical */
int ident_compare(ident_type ident1, ident_type ident2)
{
  if (ident1.type != ident2.type)
    return EPI_FAILURE;

  return (strcmp(ident1.name, ident2.name) ? EPI_FAILURE : EPI_OK);
}