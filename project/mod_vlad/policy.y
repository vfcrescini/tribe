/*
 * policy.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

/* externally available functions */
void policy_set_kb(void *a_kb);

/* function from lexer */
int policylex();

/* some static functions */
static int policyerror(char *a_msg);

/* some static vars */
static void *kbase = NULL;
static int errcode = VLAD_FAILURE;

#ifdef YYBYACC
int policyparse();
#endif
%}

%union {
  unsigned int tok;
  char ident[128];
  void *ptr;
}

%token <tok> MODVLAD_SYM_EOF
%token <tok> MODVLAD_SYM_ERROR
%token <tok> MODVLAD_SYM_OPEN_PARENT
%token <tok> MODVLAD_SYM_CLOSE_PARENT
%token <tok> MODVLAD_SYM_COMMA
%token <tok> MODVLAD_SYM_SEMICOLON
%token <tok> MODVLAD_SYM_AND
%token <tok> MODVLAD_SYM_NOT
%token <tok> MODVLAD_SYM_HOLDS
%token <tok> MODVLAD_SYM_MEMB
%token <tok> MODVLAD_SYM_SUBST
%token <tok> MODVLAD_SYM_INITIALLY
%token <tok> MODVLAD_SYM_IMPLIED
%token <tok> MODVLAD_SYM_BY
%token <tok> MODVLAD_SYM_WITH
%token <tok> MODVLAD_SYM_ABSENCE
%token <tok> MODVLAD_SYM_ALWAYS
%token <tok> MODVLAD_SYM_CAUSES
%token <tok> MODVLAD_SYM_IF
%token <tok> MODVLAD_SYM_AFTER
%token <tok> MODVLAD_SYM_GROUP
%token <tok> MODVLAD_SYM_SUB
%token <tok> MODVLAD_SYM_ACC
%token <tok> MODVLAD_SYM_OBJ
%token <ident> MODVLAD_SYM_IDENTIFIER
%type <ptr> atom
%type <ptr> boolean_atom
%type <ptr> holds_atom
%type <ptr> subst_atom
%type <ptr> memb_atom
%type <ptr> expression
%type <ptr> impliedby_clause
%type <ptr> withabsence_clause
%type <ptr> if_clause
%type <ptr> trans_var_list
%type <ptr> trans_var_def

%start program

%%

program :
  init body destroy {
  }
  ;

init : {
  }
  ;

body :
  group_section initial_section constraint_section trans_section {
    int retval;
    /* after the body, we must close the kb */
    if ((retval = vlad_kb_close_kb(kbase)) != VLAD_OK) {
      errcode = retval;
      policyerror("unable to close kb"); 
      return retval;
    }
  }
  ;

destroy : {
  }
  ;

group_section : {
    int retval;
    /* after the group section, we must close the symbol table */
    if ((retval = vlad_kb_close_symtab(kbase)) != VLAD_OK) {
      errcode = retval;
      policyerror("unable to close symtab");
      return retval;
    }
  }
  | group_stmt_list {
    int retval;
    /* after the group section, we must close the symbol table */
    if ((retval = vlad_kb_close_symtab(kbase)) != VLAD_OK) {
      errcode = retval;
      policyerror("unable to close symtab");
      return retval;
    }
  }
  ;

initial_section : {
  }
  | initial_stmt_list {
  }
  ;

constraint_section : {
  }
  | constraint_stmt_list {
  }
  ;

trans_section : {
  }
  | trans_stmt_list {
  }
  ;

group_stmt_list :
  group_stmt {
  }
  | group_stmt_list group_stmt {
  }
  ;

initial_stmt_list :
  initial_stmt {
  }
  | initial_stmt_list initial_stmt {
  }
  ;

constraint_stmt_list :
  constraint_stmt {
  }
  | constraint_stmt_list constraint_stmt {
  }
  ;

trans_stmt_list :
  trans_stmt {
  }
  | trans_stmt_list trans_stmt {
  }
  ;

group_stmt :
  MODVLAD_SYM_GROUP group_def_stmt MODVLAD_SYM_SEMICOLON {
  }
  ;

initial_stmt :
  MODVLAD_SYM_INITIALLY expression MODVLAD_SYM_SEMICOLON {
    int retval;
    unsigned int i;
    void *a; 

    /*
     * we must, unfortunetly, go through the expression and add them one
     * at a time to ensure uniqueness and integrity.
     */

    for (i = 0; i < vlad_list_length($2); i++) {
      if ((retval = vlad_exp_get($2, i, &a)) != VLAD_OK) {
        errcode = retval;
        policyerror("invalid atom");
        return retval;
      }
    }

    if ((retval = vlad_kb_add_inittab(kbase, a)) != VLAD_OK) {
      errcode = retval;
      policyerror("could not add atom into initial state table");
      return retval;
    }

    /* when everything's been registered into the kb, delete the expression */
    vlad_exp_destroy($2);
  }
  ;

constraint_stmt :
  MODVLAD_SYM_ALWAYS expression impliedby_clause withabsence_clause 
  MODVLAD_SYM_SEMICOLON {
  }
  ;

trans_stmt :
  MODVLAD_SYM_IDENTIFIER trans_var_def MODVLAD_SYM_CAUSES expression
  if_clause MODVLAD_SYM_SEMICOLON {
  }
  ;

group_def_stmt :
  MODVLAD_SYM_SUB sub_ident_list {
  }
  | MODVLAD_SYM_ACC acc_ident_list {
  }
  | MODVLAD_SYM_OBJ obj_ident_list {
  }
  ;

impliedby_clause : {
  }
  | MODVLAD_SYM_IMPLIED MODVLAD_SYM_BY expression {
  }
  ;

withabsence_clause : {
  }
  | MODVLAD_SYM_WITH MODVLAD_SYM_ABSENCE expression {
  }
  ;

trans_var_def :
  MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_CLOSE_PARENT {
  }
  |
  MODVLAD_SYM_OPEN_PARENT trans_var_list MODVLAD_SYM_CLOSE_PARENT {
  }
  ;

if_clause : {
  }
  | MODVLAD_SYM_IF expression {
  }
  ;

expression :
  boolean_atom {
    int retval;

    if ((retval = vlad_exp_create(&($$))) != VLAD_OK) {
      errcode = retval;
      policyerror("could not create an expression instance");
      return retval;
    }

    if ((retval = vlad_exp_add($$, $1)) != VLAD_OK) {
      errcode = retval;
      policyerror("could not add atom into expression");
      return retval;
    }
  }
  | expression logical_op boolean_atom {
    int retval;

    switch((retval = vlad_exp_add($$, $3))) {
      case VLAD_OK :
      case VLAD_DUPLICATE :
        break;
      default :
        errcode = retval;
        policyerror("could not add atom into expression");
        return retval;
    }
  }
  ;

sub_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | sub_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

acc_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | acc_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

obj_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | obj_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

trans_var_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | trans_var_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

logical_op :
  MODVLAD_SYM_AND {
  }
  ;

boolean_atom :
  atom {
    $$ = $1;
  }
  | MODVLAD_SYM_NOT atom {
    $$ = $2;
    vlad_atom_negate($$);
  }
  ;

atom :
  holds_atom {
    $$ = $1;
  }
  | memb_atom {
    $$ = $1;
  }
  | subst_atom {
    $$ = $1;
  }
  ;

holds_atom :
  MODVLAD_SYM_HOLDS MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_COMMA
  MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
    int retval;

    if ((retval = vlad_atom_create(&($$))) != VLAD_OK) {
      errcode = retval;
      policyerror("could not create an atom instance");
      return retval;
    }

    if ((retval = vlad_atom_init_holds($$, $3, $5, $7, 1)) != VLAD_OK) {
      errcode = retval;
      policyerror("could not initialise holds atom");
      return retval;
    }
  }
  ;

memb_atom :
  MODVLAD_SYM_MEMB MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
    int retval;

    if ((retval = vlad_atom_create(&($$))) != VLAD_OK) {
      errcode = retval;
      policyerror("could not create an atom instance");
      return retval;
    }

    if ((retval = vlad_atom_init_member($$, $3, $5, 1)) != VLAD_OK) {
      errcode = retval;
      policyerror("could not initialise member atom");
      return retval;
    }
  }
  ;

subst_atom :
  MODVLAD_SYM_SUBST MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
    int retval;

    if ((retval = vlad_atom_create(&($$))) != VLAD_OK) {
      errcode = retval;
      policyerror("could not create an atom instance");
      return retval;
    }

    if ((retval = vlad_atom_init_subset($$, $3, $5, 1)) != VLAD_OK) {
      errcode = retval;
      policyerror("could not initialise subset atom");
      return retval;
    }
  }
  ;

%%

/* external functions */
void policy_set_kb(void *a_kb)
{
  kbase = a_kb;
}

/* static functions */
static int policyerror(char *a_msg)
{
  fprintf(stderr, "ERROR (%d): %s\n", errcode, a_msg);
  return 0;
}
