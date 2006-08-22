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

void run_rgb_timing()
{

 /*
  * Declare the results struct.
  */
 Rgb_Timing timing;

 printf("%s",rgb_timing_dtest.description);

 /*
  * Call the actual test that fills in the results struct.
  */
 rgb_timing(&timing);

 /*
  * Display the results.
  */
 printf("#========================================================================\n");
 printf("# rgb_timing() test using the %s generator \n",gsl_rng_name(rng));
 printf("# Average time per rand = %e nsec.\n",timing.avg_time_nsec);
 printf("# Rands per second = %e.\n",timing.rands_per_sec);

}
