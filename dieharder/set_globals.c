/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

#include "dieharder.h"

void set_globals()
{

 /*
  * Within reason, the following user-controllable options are referenced
  * by a flag with the same first letter.  In order:
  */
 all = NO;              /* Default is to NOT do all the tests */
 binary = NO;           /* Do output a random stream in binary (with -o) */
 dtest_num = -1;        /* -1 means no test selected */
 dtest_name[0] = (char)0; /* empty test name is also default */
 filename[0] = (char)0; /* No input file */
 fromfile = 0;          /* Not from an input file */
 ks_test = 0;           /* Default is 0, Symmetrized KS test */
 output_file = 0;       /* No output file */
 overlap = 1;           /* Default is to use overlapping samples */
 multiply_p = 1;        /* Default is to use default number of psamples */
 generator = 13;        /* Default is mt19937 as a "good" generator */
 generator_name[0] = (char)0; /* empty generator name is default */
 help_flag = NO;        /* No help requested */
 iterations = -1;	/* For timing loop, set iterations to be timed */
 list = NO;             /* List all generators */
 ntuple = 0;            /* n-tuple size for n-tuple tests (0 means all) */
 psamples = 0;          /* This value precipitates use of test defaults */
 seed = 0;              /* saves the current (possibly randomly selected) seed */
 strategy = 0;          /* Means use seed (random or otherwise) from beginning of run */
 Seed = 0;              /* user selected seed.  != 0 surpresses reseeding per sample.*/
 tsamples = 0;          /* This value precipitates use of test defaults */
 table_separator = '|'; /* Default table separator is | for human readability */
 /*
  * Table flags to turn on all of these outputs are defined in output.h,
  * and can also be added by name or number on the command line as in:
  *    -T seed -T pvalues
  */
 tflag_default = THEADER + TSHOW_RNG + TLINE_HEADER + TTEST_NAME + TNTUPLE +
                 TPSAMPLES + TTSAMPLES + TPVALUES + TASSESSMENT + TRATE +
		 TSEED;
 tflag = 0;             /* We START with this zero so we can accumulate */
 verbose = 0;		/* Default is not to be verbose. */
 x_user = 0.0;          /* x,y,z_user are for "arbitrary" input controls */
 y_user = 0.0;          /* and can be used by any test without having to */
 z_user = 0.0;          /* rewrite parsecl() or add global variables */

}

