/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_sts_runs()
 *========================================================================
 */

#include "dieharder.h"

void run_sts_runs()
{


 /*
  * Declare the results struct.
  */
 Test **sts_runs_test;

 /*
  * If Seed is set use it for the entire sequence of permutations
  * tests below (starting here).
  */
 if(Seed){
   gsl_rng_set(rng,Seed);
 }

 /*
  * First we create the test (to set some values displayed in test header
  * correctly).
  */
 sts_runs_test = create_test(&sts_runs_dtest,tsamples,psamples);

 /*
  * Set any GLOBAL data used by the test.  Then call the test itself
  * This fills in the results in the Test struct.
  */
 std_test(&sts_runs_dtest,sts_runs_test);

 /*
  * Output standard test results.
  */
 output(&sts_runs_dtest,sts_runs_test);

 /*
  * Destroy the test and free all dynamic memory it used.
  */
 destroy_test(&sts_runs_dtest,sts_runs_test);

}
