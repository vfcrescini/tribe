/*
 * gnd_atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_GND_ATOM_H
#define __EPI_GND_ATOM_H

#include <ident.h>
#include "expression.h"

typedef struct 
{
  ident_type *subject;
  ident_type *access;
  ident_type *object;
} gnd_holds_type;

typedef struct
{
  ident_type *element;
  ident_type *group;
} gnd_memb_type;

typedef struct
{
  ident_type *group1;
  ident_type *group2;
} gnd_subst_type;

typedef struct
{
  unsigned short int type;
  unsigned short int truth;
  union {
    gnd_holds_type holds;
    gnd_memb_type memb; 
    gnd_subst_type subst;
  } atom;
} gnd_atom_type;

/* creates a pointer to an gnd_atom of type holds */
int gnd_atom_create_holds(gnd_atom_type *atom, 
                          ident_type *sub, 
                          ident_type *acc,
                          ident_type *obj,
                          unsigned short int truth);

/* creates a pointer to an gnd_atom of type memb */
int gnd_atom_create_memb(gnd_atom_type *atom,
                         ident_type *element,
                         ident_type *group,
                         unsigned short int truth);

/* creates a pointer to an gnd_atom of type subst */
int gnd_atom_create_subst(gnd_atom_type *atom,
                          ident_type *group1,
                          ident_type *group2,
                          unsigned short int truth);

/* return 0 if the gnd_atom is valid */
int gnd_atom_check(gnd_atom_type atom);

/* creates a copy of gnd_atom1 (no malloc) */
int gnd_atom_copy(gnd_atom_type *atom2, gnd_atom_type atom1);

/* creates a duplicate of gnd_atom1 (with malloc) */
int gnd_atom_dup(gnd_atom_type **atom2, gnd_atom_type atom1);

/* destroys gnd_atom structure */
void gnd_atom_destroy(gnd_atom_type *atom);

/* returns 0 if the contents of gnd_atom1 and gnd_atom2 are identical */
int gnd_atom_compare(gnd_atom_type atom1, gnd_atom_type atom2);

/* some convenience macro(s) */
#define EPI_GNDATOM_MALLOC (EPI_ADT_MALLOC(gnd_atom_type))

#endif