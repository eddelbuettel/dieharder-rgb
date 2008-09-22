/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_diehard_craps()
 *========================================================================
 */

#include "dieharder.h"

void run_diehard_craps()
{

 /*
  * Declare the results struct.
  */
 Test **diehard_craps_test;

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
 diehard_craps_test = create_test(&diehard_craps_dtest,tsamples,psamples);

 std_test(&diehard_craps_dtest,diehard_craps_test);

 /*
  * Output standard test results.
  */
 output(&diehard_craps_dtest,diehard_craps_test);

 /*
  * Destroy the test and free all dynamic memory it used.
  */
 destroy_test(&diehard_craps_dtest,diehard_craps_test);

}
