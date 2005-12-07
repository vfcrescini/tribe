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
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/smwrap.h>

/* a class for traversing the number list */
class vlad_smwrap_numberlist_trav : public vlad_numberlist_trav
{
  public :

    /* (re)init with values */
    void init(Api *a_api, bool a_truth);

    /* the function called by vlad_list::traverse() */
    int trav(unsigned int a_num);

  private :

    Api *m_api;
    bool m_truth;
} ;

/* (re)init with values */
void vlad_smwrap_numberlist_trav::init(Api *a_api, bool a_truth)
{
  m_api = a_api;
  m_truth = a_truth;
}

/* the function called by vlad_list::traverse() */
int vlad_smwrap_numberlist_trav::trav(unsigned int a_num)
{
  char name[VLAD_MAXLEN_NUM];

  if (m_api == NULL)
    return VLAD_UNINITIALISED;

  /* for each number, we get the name then add the atom */

  sprintf(name, "%d", a_num);
  m_api->add_body(m_api->get_atom(name), m_truth);

  return VLAD_OK;
}

vlad_smwrap::vlad_smwrap()
{
  m_smod = NULL;
  m_api = NULL;
  m_stage = 0;
}

vlad_smwrap::~vlad_smwrap()
{
  if (m_smod != NULL)
    VLAD_MEM_DELETE(m_smod);
  if (m_api != NULL)
    VLAD_MEM_DELETE(m_api);
}

/* (re)initialise */
int vlad_smwrap::init()
{
  Atom *atom;

  if (m_stage > 0) {
    if (m_smod != NULL)
      VLAD_MEM_DELETE(m_smod);
    if (m_api != NULL)
      VLAD_MEM_DELETE(m_api);
  }

  if ((m_smod = VLAD_MEM_NEW(Smodels())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_api = VLAD_MEM_NEW(Api(&(m_smod->program)))) == NULL)
    return VLAD_MALLOCFAILED;

  /* so we don't forget */
  m_api->remember();

  /* we have to add the two constant atoms */
  if ((atom = m_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  m_api->set_name(atom, VLAD_STR_TRUE);

  if ((atom = m_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  m_api->set_name(atom, VLAD_STR_FALSE);

  m_stage = 1;

  return VLAD_OK;
}

/* after this no more calls to add_atom() are allowed */
int vlad_smwrap::close_atom()
{
  if (m_stage != 1)
    return VLAD_INVALIDOP;

  /* now we add the rule that the atom "true" is always true */

  m_api->begin_rule(BASICRULE);
  m_api->add_head(m_api->get_atom(VLAD_STR_TRUE));
  m_api->end_rule();

  m_stage = 2;

  return VLAD_OK;
}

/* after this no more calls to add_rule_*() are allowed */
int vlad_smwrap::close_rule()
{
  if (m_stage != 2)
    return VLAD_INVALIDOP;

  m_api->done();

#if 0
  m_smod->program.print();
#endif

  m_smod->init();

  /* assert true and not false */
  m_api->set_compute(m_api->get_atom(VLAD_STR_TRUE), true);
  m_api->set_compute(m_api->get_atom(VLAD_STR_FALSE), false);

  if (!m_smod->model())
    return VLAD_NOMODEL;

#if 0
    m_smod->printAnswer();
#endif

  m_stage = 3;

  return VLAD_OK;
}

/* register an atom */
int vlad_smwrap::add_atom(unsigned int a_atom)
{
  Atom *atom;
  char name[VLAD_MAXLEN_NUM];

  if (m_stage != 1)
    return VLAD_INVALIDOP;

  if ((atom = m_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  sprintf(name, "%d", a_atom);
  m_api->set_name(atom, name);

  return VLAD_OK;
}

/* add an axiom (always true or always false) */
int vlad_smwrap::add_axiom(bool a_tr, unsigned int a_count, ...)
{
  unsigned int i;
  char name[VLAD_MAXLEN_NUM];
  va_list ap;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* of course if the list is empty, we do nothing */
  if (a_count == 0)
    return VLAD_OK;

  if (a_tr) {
    va_start(ap, a_count);

    /* if positive, we simply add each axiom */
    for (i = 0; i < a_count; i++) {
      sprintf(name, "%d", va_arg(ap, unsigned int));

      m_api->begin_rule(BASICRULE);
      m_api->add_head(m_api->get_atom(name));
      m_api->add_body(m_api->get_atom(VLAD_STR_TRUE), true);
      m_api->end_rule();
    }

    va_end(ap);
  }
  else {
    /* false, meaning each atom in the list cannot all be true */
    m_api->begin_rule(BASICRULE);
    m_api->add_head(m_api->get_atom(VLAD_STR_FALSE));

    va_start(ap, a_count);

    for (i = 0; i < a_count; i++) {
      sprintf(name, "%d", va_arg(ap, unsigned int));
      m_api->add_body(m_api->get_atom(name), true);
    }

    va_end(ap);

    m_api->end_rule();
  }

  return VLAD_OK;
}

/* add rule: variable argument list */
int vlad_smwrap::add_rule(unsigned int a_pcount,
                          unsigned int a_ncount,
                          unsigned int a_head,
                          ...)
{
  unsigned int i;
  char name[VLAD_MAXLEN_NUM];
  va_list ap;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* the special case when there is no body: head becomes an axiom */
  if (a_pcount == 0 && a_ncount == 0)
    return add_axiom(true, 1, a_head);

  m_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(name, "%d", a_head);
  m_api->add_head(m_api->get_atom(name));

  va_start(ap, a_head);

  /* now for the positive body */
  for (i = 0; i < a_pcount; i++) {
    sprintf(name, "%d", va_arg(ap, unsigned int));
    m_api->add_body(m_api->get_atom(name), true);
  }

  /* then the negative body */
  for (i = 0; i < a_ncount; i++) {
    sprintf(name, "%d", va_arg(ap, unsigned int));
    m_api->add_body(m_api->get_atom(name), false);
  }

  va_end(ap);

  m_api->end_rule();

  return VLAD_OK;
}

/* add rule: numberlist */
int vlad_smwrap::add_rule(unsigned int a_head,
                          vlad_numberlist *a_pbody,
                          vlad_numberlist *a_nbody)
{
  int retval;
  char name[VLAD_MAXLEN_NUM];
  vlad_smwrap_numberlist_trav nltrav;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* the special case when there is no body: head becomes an axiom */
  if (VLAD_LIST_LENGTH(a_pbody) == 0 && VLAD_LIST_LENGTH(a_nbody) == 0)
    return add_axiom(true, 1, a_head);

  m_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(name, "%d", a_head);
  m_api->add_head(m_api->get_atom(name));

  /* now for the positive body */
  if (a_pbody) {
    nltrav.init(m_api, true);

    if ((retval = a_pbody->traverse(&nltrav)) != VLAD_OK)
      return retval;
  }
  
  /* then the negative body */
  if (a_nbody) {
    nltrav.init(m_api, false);

    if ((retval = a_nbody->traverse(&nltrav)) != VLAD_OK)
      return retval;
  }

  m_api->end_rule();

  return VLAD_OK;
}

/* give T or F depending on whether atom is in ALL models or not */
int vlad_smwrap::ask(unsigned int a_atom, bool *a_res)
{
  char name[VLAD_MAXLEN_NUM];

  if (m_stage != 3)
    return VLAD_INVALIDOP;

  if (a_res == NULL)
    return VLAD_NULLPTR;

  sprintf(name, "%d", a_atom);

  /* clear previous model computations */
  m_smod->revert();

  /* now, we try to find models for which the atom being tested does NOT hold.
   * if no models are found we can safely conclude that the atom is present
   * in all models. any models we find are those where the atom does not
   * hold */

  m_api->set_compute(m_api->get_atom(name), false);
  *a_res = !m_smod->model();

  /* reset compute */
  m_api->reset_compute(m_api->get_atom(name), false);

  return VLAD_OK;
}
