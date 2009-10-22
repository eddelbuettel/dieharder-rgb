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
 /*
  * The next three are new (3.x) controls for "Test To Destruction" (TTD)
  * mode(s).  INITIALLY I will probably make Xtreme the p-value considered
  * "failure of a test" and default it to 0.000001 (one in a million
  * runs), implement two strategies: 1 for "if test result is weak,
  * increment psamples until destruction/fail is reached or cumulative p
  * exceeds the weak threshold or the cutoff in psamples is reached"; and
  * 2 for "increment psamples until either the cutoff is reached or
  * failure is reached for all selected tests".  The strategy may be
  * parsed bitwise the same way D is, though, allowing us to add
  * variations such as how psamples is stepped (linear, log, exp).
  * Initially I'm going to just go with linear in steps of 100, doing a
  * per test alloc of cutoff psamples in the original test creation and
  * adding 100 at a time from the test default until it or failure are
  * reached.  This means we need a default cutoff for these strategies,
  * let's start with a very conservative 1000.
  *
  * Note well that the defaults for strategy 1 and 2 are equivalent to
  * a normal run with -m 10 at the moment.
  */
 Xtreme = 0.0;          /* This basically forces Xoff psamples */
 Xtrategy = 0;          /* 0 means "no strategy, off", the default */
 Xoff = 1000;           /* This is not too stressful, actually */
 x_user = 0.0;          /* x,y,z_user are for "arbitrary" input controls */
 y_user = 0.0;          /* and can be used by any test without having to */
 z_user = 0.0;          /* rewrite parsecl() or add global variables */

}

