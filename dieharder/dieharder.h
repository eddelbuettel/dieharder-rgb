/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "copyright.h"

/* To enable large file support */
#define _FILE_OFFSET_BITS 64

#include <dieharder/libdieharder.h>

/*
 * Flags to control all output formatting etc.
 */
#include "output.h"

/*
 * user_template sources are here, not in library
 */
#include "user_template.h"

double rng_avg_time_nsec,rng_rands_per_second;

#ifdef RDIEHARDER
 int histogram(double *input, char *pvlabel, int inum, double min, double max, int nbins, char *label);
 Test *rdh_testptr;		/* kludge: need a global to report back to main + R */
 Dtest *rdh_dtestptr;	/* kludge: need a global to report back to main + R */
#endif

 void show_test_header(Dtest *dtest,Test **test);
 void show_test_header_debug(Dtest *dtest,Test **test);
 void test_header(Dtest *dtest);
 void show_test_results(Dtest *dtest,Test **test);
 void show_test_results_debut(Dtest *dtest,Test **test);
 void test_footer(Dtest *dtest, double pvalue, double *pvalues);

/*
 * List new rng types to be added in startup.c.  Use "empty" or
 * libdieharder rng sources as template, uncomment/clone the lines that
 * add your own type(s) in startup.c.  Consider sending "good" generators
 * that pass all or most tests or "classic" generators good or bad that
 * people might want to test back to me to include in libdieharder.
 */
 GSL_VAR const gsl_rng_type *gsl_rng_empty_random;


