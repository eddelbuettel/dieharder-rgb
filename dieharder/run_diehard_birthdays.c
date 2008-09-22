/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_diehard_birthdays()
 *========================================================================
 */

#include "dieharder.h"

void run_diehard_birthdays()
{


 /*
  * Declare the results struct.
  */
 Test **diehard_birthdays_test;

 /*
  * First we create the test (to set some values displayed in test header
  * correctly).  Note that I think we must then DESTROY THE TEST ON EXIT!
  */
 diehard_birthdays_test = create_test(&diehard_birthdays_dtest,tsamples,psamples);

 /*
  * If Seed is set use it for the entire sequence of permutations
  * tests below (starting here).
  */
 if(Seed){
   gsl_rng_set(rng,Seed);
 }

 /*
  * Set any GLOBAL data used by the test.  Then call the test itself
  * This fills in the results in the Test struct.
  */
 std_test(&diehard_birthdays_dtest,diehard_birthdays_test);

 /*
  * Output standard test results.
  */
 output(&diehard_birthdays_dtest,diehard_birthdays_test);

 /*
  * Destroy the test and free all dynamic memory it used.
  */
 destroy_test(&diehard_birthdays_dtest,diehard_birthdays_test);

}
