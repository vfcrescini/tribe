// main.cc -- for lparse
// Copyright (C) 1999-2000 Tommi Syrj�nen <Tommi.Syrjanen@hut.fi>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  


#include "../config.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifndef CXX_HAS_NO_EXCEPTIONS
#ifdef MS_WINDOWS
#include <exceptio.h>
#else
#include <exception>
#endif
#endif
//#if HAVE_UNISTD_H
//#include <unistd.h>
//#endif

#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef LIBRARY_H
#include "library.h"
#endif
#ifndef PARSETREE_H
#include "parsetree.h"
#endif
#ifndef GRAPH_H
#include "graph.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif

#include "priority.h"

// returns the position of first input file in command line
extern int yyparse();
int process_arguments(int argc, char *argv[], bool real = true);
void find_libraries();

// lnitialize_static initializes data that doesn't depend on the size
// of the program to parse, initialize_dynamic initializes the rest. 
int initialize_dynamic();
int initialize_static();
void delete_vars();

// print out the output stored in a temporary file
void print_output();
void print_compute();
void print_optimize();

int next_input_file();

void read_option(char *op);



// read in and print out a program that has been already grounded to
// smodels internal format
void read_grounded_program();
void register_grounded_program(char *);
char version[] = "lparse version " VERSION "\nbuild: " __DATE__ ", "
__TIME__; 

// generate the second program for priorities.
void generate_priority_test_program();

StringList grounded_programs;

int main(int argc, char *argv[])
{
  RestrictType res = RT_STRONG;
  initialize_static();
  process_arguments(argc, argv);

  register_functions();

  if (sys_data.print_registered) {
    function_table->PrintRegistered();
    exit (1);
  }

    
#ifndef CXX_HAS_NO_EXCEPTIONS
  try {
#endif
  yyparse();

  if (sys_data.num_errors > 0) {
    fprintf(stderr, "%d error%s found. exiting ...\n",
	    sys_data.num_errors, (sys_data.num_errors > 1) ? "s" : "");
    exit(E_ERROR);
  }
  
  if (!logic_program) {
    exit(E_ERROR);
  }
  logic_program->ProcessTree();

  initialize_dynamic();

  if (sys_data.output_version >= 2) {
    ParseNode::ProcessWeightDeclarations();
  }

  
  read_grounded_program();
  
  // this is commented out because it doesn't work with special rules
  // and it is not really needed. 
  //  if (sys_data.print_rules) {
  //    Predicate::PrintAllRules();
  //  exit(1);
  // }

  if (sys_data.num_errors > 0) {
    fprintf(stderr, "%d error%s found. exiting ...\n",
	    sys_data.num_errors, (sys_data.num_errors > 1) ? "s" : "");
    exit(E_ERROR);
  }

  if (sys_data.has_implicit_domains)
    Predicate::DefineImplicitDomains();

  dependency_graph->CheckCycles();

  if (sys_data.has_implicit_domains)
    Predicate::CheckImplicitDomains();
  
  if (special_rules->Size() > 0) {
    if (sys_data.output_version >= 2) {
      ParseNode::ProcessDelayed();
    } else {
      error(USR_ERR, "Extended rule types may not be used with smodels 1.");
    }
  }

  if (sys_data.use_regular_models) {
    Predicate::DefineAllComplements();
  }
  if (sys_data.num_errors > 0) {
    fprintf(stderr, "%d error%s found. exiting ...\n",
	    sys_data.num_errors, (sys_data.num_errors > 1) ? "s" :
	    "");
    print_warnings();
    exit(E_ERROR);
  }

  
  res = Predicate::CheckAllRestricts();

  switch (res) {
  case RT_NONE:
    print_warnings();
    fprintf(stderr, "\n");
    exit(E_ERROR);
  case RT_WEAK:
    print_warnings();
    fprintf(stderr, "\n");
    exit(E_RANGE);
  default:
    break;
  }

  if (sys_data.warnings & WARN_UNSAT) {
    Predicate::CheckUnsatisfiable();
  }


  if (compute->rl)
    compute->rl->CalculateCompute();


  
  if (sys_data.num_errors > 0) {
    fprintf(stderr, "%d error%s found. exiting ...\n",
	    sys_data.num_errors, (sys_data.num_errors > 1) ? "s" :
	    "");
    print_warnings();
    exit(E_ERROR);
  }

  if (sys_data.use_priorities) {
    priority_data.CreateGeneratorRules();
  }
  
  print_warnings();
  print_optimize();

  Predicate::EmitAll();

  if (sys_data.use_regular_models &&
      (sys_data.regular_level >= REGULAR_NORMAL)) {
    Predicate::EmitComplements();
  }

  if (sys_data.true_negation) {
    Predicate::EmitNegations();
  }

  if (sys_data.use_priorities) {
    priority_data.GroundChoices();
    priority_data.CreateTesterRules();
  }
  
  if (!sys_data.emit_text)
    print_output();

  if (sys_data.emit_text)
    print_compute();

  if (sys_data.use_priorities) {
    priority_data.GroundTester();
  }
  
#ifndef CXX_HAS_NO_EXCEPTIONS
  } catch (exception) {
    error(FATAL_ERR, "couldn't allocate necessary memory\n");
  }
#endif

  delete_vars();
  return E_OK;
}

static char *usage_msg = "usage: lparse [options] [input files]\n"
    "\nOptions:\n"
    "\t-1 -- output smodels 1.x format\n"
    "\t-c const=n -- define constant 'const' as number 'n'.\n"
    "\t-d -- which domain predicates should be emitted:\n"
    "\t\tall - all domain predicates are emitted\n"
    "\t\tfacts - domain predicates in rule tails are omitted\n"
    "\t\tpositive - negative domain predicates are omitted\n"
    "\t\tnone - all domain predicates are omitted.\n"
    "\t-D -- debug internal lparse data structures.\n"
    "\t-g file - read in a grounded file.\n"
    "\t-i -- disable internal functions \n"
    "\t-n number -- number of models\n"
    "\t-r [1 | 2 | 3] -- enable regular model translation (default level 2)\n"
    "\t\t1 -- omit integrity constraints\n"
    "\t\t2 -- normal regular model translation\n"
    "\t\t3 -- disallow undefined atoms (mainly for debugging)\n"
    "\t-t -- output plain text\n"
    "\t-v -- print version information\n"
    "\t-w number --- the default weight of literals\n"
    "\t-W warning -- enable warnings\n"
    "\t\tall - enable all warnings\n"
    "\t\tarity - warn if a predicate symbol has two different arities\n"
    "\t\textended - warn about general problems within extended rules\n"
    "\t\tlibrary - warn problems about user-defined functions\n"
    "\t\tsimilar - warn if similarly named constants and variables are used\n"
    "\t\tunsat - warn if there are unsatisfiable predicate symbols\n"
    "\t\tweight - warn if default weights are used\n"
    "\t\tsyntax - defines 'arity', 'extended', and 'weight'\n"
    "\t\ttypo - defines 'similar' and 'unsat'\n"
    "\t\terror - treat warnings as errors\n"
    "\t--dlp  - disjunctive semantics for '|'\n"
    "\t--drop-quotes - drop quotation marks from identifiers when possible\n"
    "\t--partial - partial model translation\n"
#if PRIORITIES
    "\t--priorities --- enable ordered disjunction priorities\n"
    "\t--pareto-optimal --- use pareto-optimality criterion in priorities\n"
    "\t--inclusion-optimal --- use inclusion preference semantics\n"
    "\t--cardinality-optimal --- use cardinality preference semantics\n"
    "\t--interactive-priorities --- generate rules for interactive\n"
    "\t                             priority resolution\n"
#endif
    "\t--separate-weight-definitions - positive literals in weight "
    "definitions\n\t\tcan never define a weight for a negative literal\n"
    "\t--true-negation - activate the classical negation translation\n"
    "\t--allow-inconsistent-answers - return also inconsistent answer sets\n"
    "\t\twhen using classical negation\n"
    "\t--version - print the version number and exit\n"
    "\nThe default commandline is 'lparse -d facts -s 100 -w 1'\n\n";


static int usage_error(bool real)
{
  if (real) {
    error(FATAL_ERR, usage_msg);
  }
  return 0;
}

int process_arguments(int argc, char *argv[], bool real)
{
  int i = 0;
  char *tmp = NULL;
  char *end = 0;
  char buf[BUFFER_LENGTH] = { 0 } ;
  long val = 0, index = 0;
  int warning = 0;

  for (i=1; i < argc; i++) {
    tmp = argv[i];
    if (*tmp != '-')  // start of input files
      break;
    tmp++;
    switch(*tmp) {
    case '1':
      sys_data.output_version = 1;

      if (sys_data.use_priorities) {
	if (real)
	  error(FATAL_ERR,
		"You may not use priorities with `-1'");
	else
	  return 0;
      }
      break;
    case 'c':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }
      if(!tmp) {
	return usage_error(real);
      }
      strncpy(buf, tmp, BUFFER_LENGTH);
      tmp = buf;
      while(*tmp && (*tmp != '='))
	tmp++;
      if (!*tmp) {
	return usage_error(real);
      }
      *tmp = '\0';
      tmp++;
      if (!*tmp)
	return usage_error(real);
      
      val = strtol(tmp, &end, 10);
      
      if (*end)
	return usage_error(real);
      if (function_table->Lookup(buf) >= 0)
	warn(WARN_LIBRARY, 0, "trying to redefine function '%s' as a "
	      "constant.", buf);
      numeric_constant_table->Insert(buf);

      numeric_constant_table->SetValue(buf, MAKE_NUMBER(val));

      if (!command_line_constants) {
	command_line_constants = new LongList;
      }	
      index = numeric_constant_table->LookIndex(buf);
      command_line_constants->Insert(index+1);

      break;
    case 'v':
      printf("%s\n", version);
      exit(0);
    case 'd':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }
      if (!tmp)
	return usage_error(real);
      switch(*tmp) {
      case 'a':
      case 'A':
	sys_data.print_domains = PR_ALL;
	break;
      case 'f':
      case 'F':
	sys_data.print_domains = PR_HEADS;
	break;
      case 'n':
      case 'N':
	sys_data.print_domains = PR_NONE;
	break;
      case 'p':
      case 'P':
	sys_data.print_domains = PR_POSITIVE;
	break;
      default:
	return usage_error(real);
      }
      break;
    case 'D':
      sys_data.force_small_domains = 1;
      break;
    case 'g':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }
      if (!tmp)
	return usage_error(real);
      register_grounded_program(tmp);
      break;
    case 'i':
    case 'I':
      sys_data.internal_functions = 0;
      break;

    case 'r':
      sys_data.use_regular_models = 1;
      sys_data.allow_only_disjunctive_rules = 1;
      
      // do we want to specify the level?
      if (argv[i+1] && isdigit(*argv[i+1])) {
	i++;
	val = atol(argv[i]);
	if ((val >= REGULAR_ERROR || val <= REGULAR_NONE)) {
	  return usage_error(real);
	} else {
	  sys_data.regular_level = (RegularLevel) val;
	}
      } else {
	sys_data.regular_level = REGULAR_NORMAL;
      }
      break;
	
    case 'n':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }

      if (!tmp || !isdigit(*tmp))
	return usage_error(real);
      compute->models = strtol(tmp, NULL, 10);
      compute->command_line = 1;

      if (compute->models < 0) {
	compute->models = 0;
      }
      break;
    case 't':
    case 'T':
      sys_data.emit_text = 1;
      break;
    case 'w':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }
      if (!tmp) 
	return usage_error(real);
      val = atol(tmp);
      sys_data.default_weight = val;

      
      break;
    case 'W':
      tmp++;
      if (!*tmp) {
	i++;
	tmp = argv[i];
      }
      if (!tmp)
	return usage_error(real);
      
      warning = get_warn_from_string(tmp);

      if (!warnings_table) {
	warnings_table = new SymbolTable;
      }
      
      switch (warning) {
      case WARN_NONE:
	return usage_error(real);
	break;
      case WARN_ERROR:
	sys_data.abort_when_warn = 1;
	break;
      default:
	sys_data.warnings |= warning;
	break;
      break;
      }
      break;
    case '-':
      tmp++;
      if (!strcmp(tmp, "dlp")) {
	sys_data.dlp_semantics = 1;
	sys_data.allow_only_disjunctive_rules = 1;
	break;
      } else if (!strcmp(tmp, "partial")) {
	sys_data.use_regular_models = 1;
	sys_data.allow_only_disjunctive_rules = 1;
	sys_data.regular_level = REGULAR_NORMAL;
	break;
      } else if (!strcmp(tmp, "atom-file")) {
	i++;
	if (!argv[i]) {
	  return usage_error(real);
	}
	sys_data.atom_output_file = argv[i];
	break;
      } else if (!strcmp(tmp, "true-negation")) {
	sys_data.true_negation = 1;
	break;
      } else if (!strcmp(tmp, "allow-inconsistent-answers")) {
	sys_data.true_negation = 1;
	sys_data.inconsistent_answers = 1;
	break;
      }  else if (!strcmp(tmp, "totalize-negations")) {
	sys_data.true_negation = 1;
	sys_data.totalize_negations = 1;
	break;
      } else if (!strcmp(tmp, "drop-quotes")) {
	sys_data.drop_quotes = 1;
	break;
      } else if (!strcmp(tmp, "separate-weight-definitions")) {
	sys_data.default_negative_weight_to_positive = 0;
	break;
      } else if (!strcmp(tmp, "version")) {
	printf("%s\n", version);
	exit(0);
      } else if (!strcmp(tmp, "priorities")) {
	sys_data.use_priorities = 1;
	if (sys_data.output_version < 2) {
	  error(FATAL_ERR, "The options `--priorities' and `-1' are incompatible");
	}
	break;
      } else if (!strcmp(tmp, "pareto-optimal")) {
	sys_data.use_priorities = 1;
	if (sys_data.output_version < 2) {
	  error(FATAL_ERR, "The options `--pareto-optimal' and `-1' are incompatible");
	}
	priority_data.semantics = PRI_PARETO;
	break;
      } else if (!strcmp(tmp, "inclusion-optimal")) {
	sys_data.use_priorities = 1;
	if (sys_data.output_version < 2) {
	  error(FATAL_ERR, "The options `--inclusion-optimal' "
		"and `-1' are incompatible");
	}
	priority_data.semantics = PRI_INCLUSION;
	break;
      } else if (!strcmp(tmp, "cardinality-optimal")) {
	sys_data.use_priorities = 1;
	if (sys_data.output_version < 2) {
	  error(FATAL_ERR, "The options `--cardinality-optimal' "
		"and `-1' are incompatible"); 
	}
	priority_data.semantics = PRI_CARDINALITY;
	break;
      } else if (!strcmp(tmp, "interactive-priorities")) {
	sys_data.use_priorities = 1;
	sys_data.use_interactive_priorities = 1;
	if (sys_data.output_version < 2) {
	  error(FATAL_ERR, "The options `--interactive-priorities' and "
		"`-1' are incompatible");
	}
	break;
      }
      /* fall through */
    default:
      return usage_error(real);
    }
  }

  if (real) {

    default_weight = new Weight;
    default_weight->v.val = sys_data.default_weight;
  
    if ((grounded_programs.Size() > 0) && sys_data.output_version < 2) {
      error(FATAL_ERR, "The command line option '-g' works only "
	    "with smodels 2.x output format");
    }
    if (i == argc)
      return 1;
    
    // store the number of input files and allocate space for names
    sys_data.num_input_files = argc -i;
    sys_data.input_files = new char*[argc-i];
    sys_data.file_start_lines = new long[argc-i];
    int pos = 0;
    
    // store the file names
    for (; i < argc; i++) {
      sys_data.input_files[pos] = clone_string(argv[i]);
      sys_data.file_start_lines[pos++] = LONG_MAX;
    }
    next_input_file();
  }
  return 1;
}

int initialize_static()
{
  debug(DBG_MAIN, 1, "initialize_static");
  sys_data.num_errors = 0;
  sys_data.emit_text = 0;
  sys_data.abort_when_warn = 0;
  sys_data.warnings = WARN_NONE;
  sys_data.internal_functions = 1;
  sys_data.print_domains = PR_HEADS;
  sys_data.print_registered = 0;
  sys_data.print_rules = 0;
  sys_data.domain_size = DOMAIN_DEFAULT_SIZE;
  sys_data.max_number = 0;
  sys_data.ground_atoms = 0;
  sys_data.ground_rules = 0;
  sys_data.ground_sigma = 0;
  sys_data.hide_all = 0;
  sys_data.output_version = 2;
  sys_data.unknown_vars = 0;
  sys_data.default_weight = DEFAULT_WEIGHT;
  sys_data.force_small_domains = 0;
  sys_data.total_vars = 0;
  sys_data.term_max_arity = 0;
  sys_data.use_regular_models = 0;
  sys_data.dlp_semantics = 0;
  sys_data.allow_only_disjunctive_rules = 0;
  sys_data.atom_output_file = 0;
  sys_data.true_negation = 0;
  sys_data.inconsistent_answers = 0;
  sys_data.totalize_negations = 0;
  sys_data.drop_quotes = 0;
  sys_data.default_negative_weight_to_positive =1;
  sys_data.use_priorities = 0;
  sys_data.use_interactive_priorities = 0;
  sys_data.has_implicit_domains = 0;
  
  function_table = new FunctionTable;
  predicate_table = new SymbolTable;
  constant_table = new SymbolTable(SYMBOL_DEFAULT_SIZE, 1);
  numeric_constant_table = new SymbolTable;
  variable_table = new SymbolTable;
  atom_table = new SymbolTable(ATOM_DEFAULT_SIZE);

  warnings_table = NULL;
  
  dependency_graph = new Graph;
  compute = new ComputeStmt;
  compute->models = 1;
  // initialize the compute rule
  Literal *lt = new Literal;
  if (!lt)
    error(SYS_ERR, "malloc error");
  
  compute->rl = new Rule(lt, OPTIMIZERULE);
  if (!compute->rl)
    error(SYS_ERR, "malloc error");

  warning_list = new StringList(ORDERED_SMALL);
  
  special_rules = new ParseNodeList;
  weight_declarations = new ParseNodeList;
  optimize = new RuleList;
  
  implicit_domain_list = new LiteralList;
  if (!function_table || !predicate_table || !constant_table || !
      atom_table || !variable_table || ! compute || !special_rules ||
      !optimize)
    error(SYS_ERR, "malloc error");

  
#if DEBUG & DBG_PARSE
  extern int yydebug;
  yydebug =1;
#endif
  return 1;
}

int initialize_dynamic()
{
  int test_smodels_1 = 0; // change to 1 to debug smodels1
  debug(DBG_MAIN, 1, "initialize_dynamic");
  /*  predicate_table->CreateSymbolArray();
  variable_table->CreateSymbolArray();
  constant_table->CreateSymbolArray();
  */
  if (c_stmt)
    c_stmt->ProcessCompute(0,0);


  if (sys_data.use_priorities)
    sys_data.internal_variables += 7;
  
  sys_data.total_vars = variable_table->Size() +
    sys_data.internal_variables +
    sys_data.unknown_vars;

  
  variables = new Instance[sys_data.total_vars + 1];
  var_pos = new Variable[sys_data.total_vars + 1];

  if (!var_pos || ! variables)
    error(SYS_ERR, "malloc error");

  if (sys_data.output_version > 1 || sys_data.emit_text ||
      test_smodels_1) { 
    sys_data.output_file = stdout;
  } else {
    sys_data.output_file = tmpfile();
    if (!sys_data.output_file)
      error(SYS_ERR, "cannot create temporary file");
  }

  if (sys_data.use_priorities) {
    priority_atoms = new SymbolTable();
  }
  return 1;
}

void print_output()
{
  long i, atom = 0;
  FILE *atom_file = 0;
  char buf[BUFFER_LENGTH] = { 0 };
  Literal *lt = NULL;

  long skipped_positive_compute = 0;
  long skipped_negative_compute = 0;
  
  // print the size information
  if (sys_data.output_version < 2) {
    printf("%ld\n", atom_table->Size());
    printf("%ld\n", sys_data.ground_rules);
    printf("%ld\n", sys_data.ground_sigma);
    
    // reset tmpfile
    rewind(sys_data.output_file);
    
    // print out the rules
    while(fgets(buf, BUFFER_LENGTH, sys_data.output_file) != NULL)
      printf(buf);
    
    fclose(sys_data.output_file);
    sys_data.output_file = stdout;
  }
  // create symbols array for grounded atoms
  //  atom_table->CreateSymbolArray();

  // print out the atoms
  if (sys_data.atom_output_file) {
    atom_file = fopen(sys_data.atom_output_file, "w");
    if (!atom_file)
      error(USR_ERR, "cannot open file '%s' for writing",
	    atom_file);
 
    // if FALSE_LIT exists, add a rule:
    //  __false :- not __false, false.
    if (false_lit) {
      atom = atom_table->Insert("__false") +1;
      printf("1 %ld 2 1 %ld %ld\n",
      	     atom, atom, atom_table->Lookup(FALSE_NAME) +1);
    }
    // do the same for all atoms in the compute statement
    while (compute->rl && (lt = compute->rl->positive.Iterate())) {
      atom = atom_table->Insert("__false") +1;
      printf("1 %ld 2 2 %ld ", atom, atom);
      lt->EmitGround();
      printf("\n");
    }
    while (compute->rl && (lt = compute->rl->negative.Iterate())) {
      atom = atom_table->Insert("__false") +1;
      printf("1 %ld 2 1 %ld ", atom, atom);
      lt->EmitGround();
      printf("\n");
    }
  } else {
    if (sys_data.output_version >= 2)
      printf("0\n");
    atom_file = stdout;
  }

  
  for (i = 0; i < atom_table->Size(); i++) {
    // check if it is an internal atom
    if ((*atom_table->symbols[i] != '_') ||
	(sys_data.output_version < 2)) {
      if (sys_data.output_version >= 2)
	fprintf(atom_file,"%ld ", i+1);
      fprintf(atom_file,"%s\n", atom_table->symbols[i]);
    }
  }

  
  if (sys_data.output_version >= 2)
    fprintf(atom_file,"0\n");
  // print compute stmt
  fprintf(atom_file,"B+\n");
  if (sys_data.output_version < 2)
    if (compute->rl) {
      while ((lt = compute->rl->positive.Iterate())) {
	if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
	  if (lt->Test()) {
	    skipped_positive_compute++;
	  }
	}
      }
      fprintf(atom_file,"%ld\n", compute->rl->positive.Size() -
	      skipped_positive_compute);
    } else {
      fprintf(atom_file,"0\n");
    }
  while (compute->rl && (lt = compute->rl->positive.Iterate())) {
    if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
      if (lt->Test()) {
	continue;
      }
    }
    lt->EmitGround();
    fprintf(atom_file,"\n");
  }
  while ((atom = compute->positive_atoms.Iterate())) {
    fprintf(atom_file,"%ld\n", atom);
  }

  if (sys_data.output_version >= 2)
    fprintf(atom_file,"0\n");
  
  fprintf(atom_file,"B-\n");
  
  if (sys_data.output_version < 2) {
    while ((lt = compute->rl->negative.Iterate())) {
      if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
	if (!lt->Test()) {
	  skipped_negative_compute++;
	}
      }
    }
    if (compute->rl)
      fprintf(atom_file,"%ld\n", compute->rl->negative.Size() +
	      (false_lit != NULL) - skipped_negative_compute);
    else
      fprintf(atom_file,"0\n");
  }
  while (compute->rl && (lt = compute->rl->negative.Iterate())) {
    if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
      if (!lt->Test()) {
	continue;
      }
    }
    lt->EmitGround();
    fprintf(atom_file,"\n");
  }
  while ((atom = compute->negative_atoms.Iterate())) {
    fprintf(atom_file,"%ld\n", atom);
  }
  
  if (false_lit) {
    i = atom_table->Lookup(FALSE_NAME);
    fprintf(atom_file,"%ld\n", i+1);
  }
  
  if (sys_data.output_version >= 2)
    fprintf(atom_file,"0\n");
  
  // and number of models
  fprintf(atom_file,"%ld\n", compute->models);

  if (atom_file != stdout) {
    fclose(atom_file);
  }
}

void print_compute()
{
  int first = 1;
  long atom = 0;
  Literal *lt = NULL;
  if ((compute->models == 1) &&
      (compute->rl->positive.Size() == 0) &&
      (compute->rl->negative.Size() == 0) &&
      (compute->positive_atoms.Size() == 0) &&
      (compute->negative_atoms.Size() == 0)) {
    if (false_lit) 
      printf("compute 1 { not _false }.\n");
    return;
  }
  printf("compute %ld { ", compute->models);

  while ((lt = compute->rl->positive.Iterate())) {
    if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
      if (lt->Test()) {
	continue;
      }
    }
    
    if (!first)
      printf(", ");
    else
      first = 0;
    lt->EmitGround();
  }

  while ((atom = compute->positive_atoms.Iterate())) {
    if (!first)
      printf(", ");
    else
      first = 0;
    printf("%s", atom_table->symbols[atom-1]);
  }
  
  while ((lt = compute->rl->negative.Iterate())) {
    if (sys_data.print_domains == PR_NONE && lt->DomainLiteral()) {
      if (!lt->Test()) {
	continue;
      }
    }
    if (!first)
      printf(", not ");
    else {
      first = 0;
      printf("not ");
    }
    lt->EmitGround();
  }
  while ((atom = compute->negative_atoms.Iterate())) {
    if (!first) {
      printf(", not ");
    } else {
      first = 0;
      printf("not ");
    }
    printf("%s", atom_table->symbols[atom-1]);
  }
  
  if (false_lit) {
    printf("%s not %s", first ? "": ",",FALSE_NAME);
  }
  printf(" }.\n");
}

void print_optimize()
{
  Rule *rl = NULL;

  while ((rl = optimize->Iterate())) {
    rl->CalculateOptimize();
  }

}

void delete_vars()
{
  delete numeric_constant_table;
  delete function_table;
  delete predicate_table;
  delete constant_table;
  delete variable_table;
  delete atom_table;
  delete dependency_graph;
  delete compute;
  delete [] variables;
  delete [] var_pos;
  
  delete condition_set;
  delete condition_weights;
  delete optimize;
  //  if (false_lit)
  //    delete false_lit;
}

StringList *parse_line(char *ln)
{
  char *line = clone_string(ln);
  char *st = NULL;
  StringList *new_list = NULL;
  
  st = strtok(line, " =,\t\n:;");

  if (!st)
    return NULL;

  new_list = new StringList();

  if (*st == '/')
    new_list->Append(st);
  
  if (!new_list)
    error(SYS_ERR, "malloc error");

  while ((st = strtok(NULL, " =,\t\n:"))) {
    new_list->Append(st);
  }
  return new_list;
}

// find the dynamically loaded libraries
void find_libraries()
{
  static int already_done = 0;
  StringList *paths = NULL;
  StringList *libs = NULL;
  StringList *tmp = NULL;
  LibraryNode *new_node = NULL;
  FILE *in = NULL;
  char *st = NULL;
  char *st2 = NULL;
  int pos = 0, found = 0;
  char buf[BUFFER_LENGTH] = { 0 };

  // do only once
  if (already_done)
    return;
  already_done =1;
  
  user_libraries = new LibraryList();
  if (!user_libraries)
    error(SYS_ERR, "malloc error");

  // search order for directories:
  // 	LPARSE_LIBRARY_PATH env. variable
  //	paths in .lparserc
  //	paths in LD_LIBRARY_PATH

  // LPARSE_LIBRARY_PATH:
  st = getenv(LPARSE_LIBRARY_PATH);
  if (st) {
    paths = parse_line(st);
  }

  st = getenv(LPARSE_LIBRARIES);
  if (st) {
    libs = parse_line(st);
  }

  
  // .lparserc
  st = getenv("HOME");
  if (st) {
    pos = sprintf(buf, "%s/%s", st, RC_FILE);
    buf[pos] = '\0';
    in = fopen(buf,  "r");
    if (in) {
      while (fgets(buf, BUFFER_LENGTH, in) != NULL) {
	tmp = parse_line(buf);
	if (tmp && (buf[0] != '#')) { // weed out comments
	  if (strstr(buf, LPARSE_LIBRARIES)) {
	    if (libs)
	      libs->Merge(tmp);
	    else
	      libs = tmp;
	  } else if (strstr(buf, LPARSE_LIBRARY_PATH)) {
	    if (paths)
	      paths->Merge(tmp);
	    else
	      paths = tmp;
	  }
	}
      }
      fclose(in);
    }
  }
  if (!libs) // no point of continuing anymore
    return ;
  
  // LD_LIBRARY_PATH
  st = getenv("LD_LIBRARY_PATH");
  if (st) {
    tmp = parse_line(st);
    if (paths) {
      paths->Merge(tmp);
    } else
      paths = tmp;
  }

  // find the needed library files:
  while ((st = libs->Iterate())) {
    found = 0;
    paths->ClearIterator();
    while ((st2 = paths->Iterate())) {
      pos = sprintf(buf, "%s/%s", st2, st);
      buf[pos] = '\0';
      // check if the file exists
      in = fopen(buf, "r");
      if (in) {
	new_node = new LibraryNode(clone_string(buf));
	if (!new_node)
	  error(SYS_ERR, "malloc error");
	fclose(in);
	found = 1;
	user_libraries->Append(new_node);
	break;
      }
    }
    if (!found) {
      warn(WARN_LIBRARY, 0, "cannot open function library %s", st);
    }
  }

}
    
// read in a program in smodels input format, print it
// supposes that if the first atom in the program is hidden, it is
// really _false. 
void read_grounded_program()
{
  char *name = 0;
  char *dummy = "__dummy";
  long pos = 1, atom = 0;
  long pred = 0;
  char *p;
  char line[BUFFER_LENGTH];
  FILE *fp;
  
  while ((name = grounded_programs.Iterate())) {
    fp = fopen(name, "r");
    if (!fp) {
      error(USR_ERR, "cannot open input file '%s'.",  name);
      return;
    }

    pos = 1;
    while (fgets(line, BUFFER_LENGTH, fp)) {
      if (!strcmp(line, "0\n")) {
	// the atoms begin, read them to the memory and stop printing
	// them out
	while (fgets(line, BUFFER_LENGTH, fp)) {
	  if (!strcmp(line, "0\n")) {
	    break;
	  }

	  atom = atol(line);
	  
	  while (pos < atom ) {
	    if (pos == 1) {
	      if (atom_table->symbols[0] &&
		  strcmp(atom_table->symbols[0], FALSE_NAME)) {
		error(FATAL_ERR,
		      "Two pre-grounded programs have "
		      " different sets of atoms:\n"
		      "\tThe input file '%s' has a "
		      "hidden atom in a position where \n"
		      "\tthe atom '%s' occurred previously",
		      name, atom_table->symbols[0]);
	      } else {
		atom_table->Insert(FALSE_NAME);
	      }
	    } else {
	      if (atom_table->symbols[pos-1] &&
		  strcmp(atom_table->symbols[pos-1], dummy)) {
		error(FATAL_ERR,
		      "Two pre-grounded programs have "
		      "different sets of atoms:\n"
		      "\tThe input file '%s' has a "
		      "hidden atom in a position where \n"
		      "\tthe atom '%s' occurred previously",
		      name, atom_table->symbols[pos-1]);
	      } else {
		atom_table->Insert(dummy);
	      }
	    }
	    pos++;
	  }
	  for (p = line; *p != ' '; p++) ;
	  p++;
	  char *q = strchr(p, '\n');
	  if (q)
	    *q = '\0';

	  pred = Predicate::decode_atom(p);
	  if (predicates[pred]->hidden) {
	    p -= 2; // this is always valid because of line format 
	    p[0] = p[1] =  '_';
	  }
	  if (atom_table->Size() >= atom) {
	    if (strcmp(p, atom_table->symbols[atom-1])) {
	      error(USR_ERR, "Two pre-grounded programs have"
		    " different sets of atoms:");
	      if (strcmp(atom_table->symbols[atom-1], dummy) &&
		  strcmp(atom_table->symbols[atom-1], FALSE_NAME)) {
		fprintf(stderr, "\tThe atom '%s' " 
			"in the input file '%s'\n"
			"\tis used in "
			"a position where '%s' occurred previously.\n" 
			, p, name, atom_table->symbols[atom-1]);
	      } else {
		fprintf(stderr, "\tThe atom '%s' "
			"in the input file '%s'\n"
			"\tis used in "
			"a position where a hidden atom occurred"
			" previously.\n",  p, name);
	      }
	      exit(1);
	    }
	  }
	  atom_table->Insert(p);
	  pos++;
	}
	// read the compute statements
	fgets(line, BUFFER_LENGTH, fp);
	while (fgets(line, BUFFER_LENGTH, fp)) {
	  if (!strcmp(line, "0\n"))
	    break;
	  atom = atol(line);
	  compute->positive_atoms.Insert(atom);
	}
	fgets(line, BUFFER_LENGTH, fp);
	while (fgets(line, BUFFER_LENGTH, fp)) {
	  if (!strcmp(line, "0\n"))
	    break;
	  atom = atol(line);
	  compute->negative_atoms.Insert(atom);
	}
	
	break;
      }
      fprintf(sys_data.output_file, "%s", line);
    }
    fclose(fp);
  }
}

void register_grounded_program(char *filename)
{
  grounded_programs.Insert(clone_string(filename));
}



void generate_priority_test_program()
{
  
}


char *combine_argv(int argc, char *argv[]) {
  static char buf[BUFFER_LENGTH] = { 0 };
  int i;
  long pos = 0;
  for (i = 0; i < argc; i++) {
    pos += sprintf(&buf[pos], "%s ", argv[i]);
  }
  // clear the final space
  buf[--pos] = '\0';
  return buf;
}

void read_option(char *op)
{
  // form the argv table
  char *st = NULL;
  char *p = op;
  int argc = 0;
  char **argv = 0;
  
  // count the arguments
  while (*p) {
    if (isspace(*p)) {
      argc++;
      while (*p && isspace(*p)) {
	p++;
      }
      p--;
    }
    p++;
  }
  if (!isspace(*(p-1))) {
    argc++;
  }
  argv = new char*[argc];

  int pos = 0;
  st = op;
  while ((p = strtok(st, " \t\n"))) {
    argv[pos++] = p;
    st = NULL;
  }

  // remove trailing dots
  p = &argv[argc-1][(strlen(argv[argc-1]))-1];
  while ((p >= argv[argc-1]) && *p == '.') {
    *p = '\0';
    p--;
  }
  
  if (!process_arguments(argc, argv, false)) {
    error(USR_ERR, "invalid command line argument string `%s'",
	  combine_argv(argc, argv));
  }
}
