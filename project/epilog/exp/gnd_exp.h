/* 
 * gnd_exp.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_GND_EXP_H
#define __EPI_GND_EXP_H

#include <simplelist.h>
#include "gnd_atom.h"

#define gnd_exp_type simplelist_type

/* initialise list */
int gnd_exp_init(gnd_exp_type *exp);

/* gives the number of atoms in the gnd_exp */
int gnd_exp_length(gnd_exp_type exp, unsigned int *len);

/* return 0 if the atom is in the gnd_exp */
int gnd_exp_find(gnd_exp_type exp, gnd_atom_type atom);

/* gives a reference to the index'th atom in the gnd_exp */
int gnd_exp_get(gnd_exp_type exp, unsigned int index, gnd_atom_type **atom);

/* add an atom into the gnd_exp */
int gnd_exp_add(gnd_exp_type *exp, gnd_atom_type atom);

/* delete an atom from the gnd_exp */
int gnd_exp_del(gnd_exp_type *exp, gnd_atom_type atom);

/* delete all atoms from this gnd_exp */
int gnd_exp_purge(gnd_exp_type *exp);

#endif
