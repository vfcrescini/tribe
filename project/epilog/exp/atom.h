/*
 * atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_ATOM_H
#define __EPI_ATOM_H

#include "name.h"

typedef enum
{
  epi_true = 0,
  epi_false = 1
} truth_type;

typedef struct 
{
  name_type subject;
  name_type access;
  name_type object;
} holds_type;

typedef struct
{
  name_type element;
  name_type group;
} memb_type;

typedef struct
{
  name_type group1;
  name_type group2;
} subst_type;

typedef struct
{
  unsigned short int type;
  truth_type truth;
  union {
    holds_type holds;
    memb_type memb; 
    subst_type subst;
  } atom;
} atom_type;

/* creates a pointer to an atom of type const */
int atom_create_const(atom_type **atom, truth_type truth);

/* creates a pointer to an atom of type holds */
int atom_create_holds(atom_type **atom, 
                      name_type sub, 
                      name_type acc,
                      name_type obj,
                      truth_type truth);

/* creates a pointer to an atom of type memb */
int atom_create_memb(atom_type **atom,
                     name_type element,
                     name_type group,
                     truth_type truth);

/* creates a pointer to an atom of type subst */
int atom_create_subst(atom_type **atom,
                      name_type group1,
                      name_type group2,
                      truth_type truth);

/* return 0 if the atom is valid */
int atom_check(atom_type atom);

/* creates a copy of atom1 */
int atom_copy(atom_type **atom2, atom_type atom1);

/* destroys atom structure */
int atom_destroy(atom_type *atom);

/* returns 0 if the contents of atom1 and atom2 are identical */
int atom_compare(atom_type atom1, atom_type atom2);

/* first 3 bits indicate the type */
#define EPI_ATOM_CONST       1
#define EPI_ATOM_HOLDS       2
#define EPI_ATOM_MEMB        3
#define EPI_ATOM_SUBST       4

/* some convenience macros */
#define EPI_ATOM_IS_CONST(X) (((X).type & 7) == EPI_ATOM_CONST)
#define EPI_ATOM_IS_HOLDS(X) (((X).type & 7) == EPI_ATOM_HOLDS)
#define EPI_ATOM_IS_MEMB(X)  (((X).type & 7) == EPI_ATOM_MEMB)
#define EPI_ATOM_IS_SUBST(X) (((X).type & 7) == EPI_ATOM_SUBST)
#define EPI_ATOM_NEGATE(X)   ((X).truth = ((X).truth == epi_true) ? epi_false : epi_true)

#endif
