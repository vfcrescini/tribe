/*
 * consttab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <consttab.h>

constraint::constraint()
{
  initialised = false;
}

constraint::~constraint()
{
  if (exp != NULL)
    delete exp;
  if (cond != NULL)
    delete cond;
  if (ncond != NULL)
    delete ncond;
}

bool constraint::cmp(list_item *item)
{
  constraint *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<constraint *> (item)) == NULL)
    return false;

  /* ensure both are initialised. return true if both are uninitialised */

  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /* components can never be null */

  if (!exp->cmp(tmp->exp))
    return false;

  if (!cond->cmp(tmp->cond))
    return false;

  if (!ncond->cmp(tmp->ncond))
    return false;

  return true;
}

int constraint::init(expression *e, expression *c, expression *n)
{
  if (e == NULL || c == NULL || n == NULL)
    return VLAD_NULLPTR;

  if (exp != NULL)
    delete exp;
  if (cond != NULL)
    delete cond;
  if (ncond != NULL)
    delete ncond;

  exp = e;
  cond = c;
  ncond = n;
  initialised = true;

  return VLAD_OK;
}

int constraint::get(expression **e, expression **c, expression **n)
{
  if (e == NULL || c == NULL || n == NULL)
    return VLAD_NULLPTR;

  *e = exp;
  *c = cond;
  *n = ncond;

  return VLAD_OK;
}

consttab::consttab() : list(true)
{
}

consttab::~consttab()
{
  purge(true);
}

int consttab::add(expression *e, expression *c, expression *n)
{
  int retval;
  constraint *tmp;

  if ((tmp = VLAD_NEW(constraint())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(e, c, n)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return list::add((list_item *) tmp);
}

int consttab::get(unsigned int i, expression **e, expression **c, expression **n)
{
  int retval;
  constraint *tmp;

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  return tmp->get(e, c, n);
}
