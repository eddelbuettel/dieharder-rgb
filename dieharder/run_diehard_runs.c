/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_diehard_runs()
 *========================================================================
 */

#include "dieharder.h"

void run_diehard_runs()
{

 /*
  * Declare the results struct.
  */
 Test **diehard_runs_test;

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
 diehard_runs_test = create_test(&diehard_runs_dtest,tsamples,psamples);

 /*
  * Set any GLOBAL data used by the test.  Then call the test itself
  * This fills in the results in the Test struct.
  */
 std_test(&diehard_runs_dtest,diehard_runs_test);

 /*
  * Output standard test results.
  */
 output(&diehard_runs_dtest,diehard_runs_test);

 /*
  * Destroy the test and free all dynamic memory it used.
  */
 destroy_test(&diehard_runs_dtest,diehard_runs_test);

}
