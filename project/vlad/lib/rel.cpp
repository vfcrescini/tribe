/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
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

#include <cstdlib>
#include <cstdio>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/rel.h>

vlad_rel::vlad_rel()
{
  m_int1 = NULL;
  m_int2 = NULL;
  m_init = false;
  TBE_REL_SET_CLEAR(m_rs);
}

vlad_rel::~vlad_rel()
{
  if (m_int1 != NULL)
    VLAD_MEM_FREE(m_int1);
  if (m_int2 != NULL)
    VLAD_MEM_FREE(m_int2);
}

/* returns true if rels are equal */
bool vlad_rel::cmp(vlad_list_item *a_item)
{
  vlad_rel *rel = NULL;

  if (a_item == NULL)
    return false;

  if ((rel = dynamic_cast<vlad_rel *>(a_item)) == NULL)
    return false;

  /* if both are uninitialised, return true. if only one, return false */
  if (!m_init)
    return !rel->m_init;
  if (!rel->m_init)
    return false;

  /* we don't care if the relsets are equal or not */
  return !strcmp(m_int1, rel->m_int1) && !strcmp(m_int2, rel->m_int2);
}

/* (re)initialise the relation */
int vlad_rel::init(const char *a_int1, const char *a_int2, unsigned int a_rs)
{
  int retval = VLAD_OK;
  char *int1 = NULL;
  char *int2 = NULL;

  if (a_int1 == NULL || a_int2 == NULL)
    return VLAD_NULLPTR;

  if (a_rs > TBE_REL_FII)
    return VLAD_INVALIDINPUT;

  /* allocate mem */
  if ((retval == VLAD_OK) && (int1 = VLAD_MEM_STR_MALLOC(a_int1)) == NULL)
    retval = VLAD_MALLOCFAILED;
  if ((retval == VLAD_OK) && (int2 = VLAD_MEM_STR_MALLOC(a_int2)) == NULL)
    retval = VLAD_MALLOCFAILED;

  /* if we failed, abort */
  if (retval != VLAD_OK) {
    if (int1 != NULL)
      VLAD_MEM_FREE(int1);
    if (int2 != NULL)
      VLAD_MEM_FREE(int2);

    return retval;
  }

  /* cleanup if necessary */
  if (m_int1 != NULL)
    VLAD_MEM_FREE(m_int1);
  if (m_int2 != NULL)
    VLAD_MEM_FREE(m_int2);

  /* copy strings */
  strcpy(int1, a_int1);
  strcpy(int2, a_int2);

  /* assign */
  m_int1 = int1;
  m_int2 = int2;
  m_rs = a_rs;
  m_init = true;
    
  return VLAD_OK;
}

/* gives the values */
int vlad_rel::get(char **a_int1, char **a_int2, unsigned int *a_rs)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_rs == NULL)
    return VLAD_NULLPTR;

  /* these will just give a reference, not a copy */

  if (a_int1 != NULL)
    *a_int1 = m_int1;
  if (a_int2 != NULL)
    *a_int2 = m_int2;

  *a_rs = m_rs;

  return VLAD_OK;
}

/* adds the given relation to the relset */
int vlad_rel::add(unsigned int a_rel)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_rel > TBE_REL_FII)
    return VLAD_INVALIDINPUT;

  TBE_REL_SET_ADD(m_rs, a_rel);

  return VLAD_OK;
}


/* creates a new instance of this relation */
int vlad_rel::copy(vlad_rel **a_rel)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_rel == NULL)
    return VLAD_NULLPTR;

  if ((*a_rel = VLAD_MEM_NEW(vlad_rel())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = (*a_rel)->init(m_int1, m_int2, m_rs);

  if (retval != VLAD_OK)
    VLAD_MEM_DELETE(*a_rel);

  return retval;
}

/* replaces vars in a_vlist with idents in a_ilist. gives a new rel */
int vlad_rel::replace(vlad_varlist *a_vlist,
                      vlad_stringlist *a_ilist,
                      vlad_rel **a_rel)
{
  int retval;
  vlad_rel *rel;
  char *name[2];
  unsigned int index;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_rel == NULL)
    return VLAD_NULLPTR;

  /* if both lists are null or if the lengths are zero, return the rel */
  if (VLAD_LIST_LENGTH(a_vlist) == 0 && VLAD_LIST_LENGTH(a_ilist) == 0)
    return copy(a_rel);

  /* make sure the lists are non-NULL and their lengths are equal */
  if (a_vlist == NULL || a_ilist == NULL || a_vlist->length() != a_ilist->length())
    return VLAD_INVALIDINPUT;

  /* all good, so create a new rel */
  if ((rel = VLAD_MEM_NEW(vlad_rel())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  /* blindly replace variables with intervals */

  if (retval == VLAD_OK) {
    retval = a_vlist->get(m_int1, &index);
    switch(retval) {
      case VLAD_NOTFOUND :
        name[0] = m_int1;
        retval = VLAD_OK;
        break;
      case VLAD_OK :
        a_ilist->get(index, &(name[0]));
        break;
    }
  }

  if (retval == VLAD_OK) {
    retval = a_vlist->get(m_int2, &index);
    switch(retval) {
      case VLAD_NOTFOUND :
        name[1] = m_int2;
        retval = VLAD_OK;
        break;
      case VLAD_OK :
        a_ilist->get(index, &(name[1]));
        break;
    }
  }

  if (retval == VLAD_OK)
    retval = rel->init(name[0], name[1], m_rs);

  /* cleanup */
  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(rel);
    return retval;
  }

  *a_rel = rel;

  return VLAD_OK;
}

/* gives a list of vars occurring in the rel. assumes list is init'ed */
int vlad_rel::varlist(vlad_varlist *a_list)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_list == NULL)
    return VLAD_NULLPTR;

  if (vlad_identifier::validate_var_ident(m_int1) == VLAD_OK)
    if ((retval = a_list->add(m_int1)) != VLAD_OK)
      if (retval != VLAD_DUPLICATE)
        return retval;

  if (vlad_identifier::validate_var_ident(m_int2) == VLAD_OK)
    if ((retval = a_list->add(m_int2)) != VLAD_OK)
      if (retval != VLAD_DUPLICATE)
        return retval;

  return VLAD_OK;
}

/* returns true if none of the interval identifiers are variables */
bool vlad_rel::is_ground()
{
  /* uninitialised rel is ground */
  if (!m_init)
    return true;

  if (vlad_identifier::validate_var_ident(m_int1) == VLAD_OK)
    return false;
  if (vlad_identifier::validate_var_ident(m_int2) == VLAD_OK)
    return false;

  return true;
}