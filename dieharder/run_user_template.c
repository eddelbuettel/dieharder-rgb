/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_rgb_timing()
 *========================================================================
 */

#include "dieharder.h"

void run_user_template()
{


 /*
  * Declare the results struct.
  */
 Test **user_template_test;

 /*
  * First we create the test (to set some values displayed in test header
  * correctly).
  */
 user_template_test = create_test(&user_template_dtest,tsamples,psamples,&user_template);

 /*
  * Set any GLOBAL data used by the test.  This is very test specific -- most
  * library tests won't have any but a few do.  The data will usually be input
  * via one of the special command line variables reserved for that purpose,
  * and only can work if the -a(ll) flag is not set -- otherwise test specific
  * defaults are used.
  */
 user_template_lag = (int)x_user;

 /*
  * This is where we can output any test-specific information (such as
  * the value of the lag).   In dieharder, I tend to start each output
  * line with an # to make things all pretty.
  */
 if(!table && !quiet){
   printf("# Lag used in %s test: %u\n",user_template_dtest.name,user_template_lag);
 }

 /*
  * Set any GLOBAL data used by the test.  Then call the test itself
  * This fills in the results in the Test struct.
  */
 std_test(&user_template_dtest,user_template_test);

 /*
  * Output test results.
  */
 output(&user_template_dtest,user_template_test);

 /*
  * Destroy the test and free all dynamic memory it used.
  */
 destroy_test(&user_template_dtest,user_template_test);

}
