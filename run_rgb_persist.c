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

void run_rgb_persist()
{

 /*
  * Declare the results struct.
  */
 Rgb_Persist persist;

 test_header(&rgb_persist_data);
 printf("# Samples per test run = %u, tsamples ignored\n",256);
 printf("# Test run %u times to cumulate unchanged bit mask\n",psamples);

 /*
  * Call the actual test that fills in the results struct.
  */
 rgb_persist(&persist);

 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
   printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
 }
 printf("#==================================================================\n");
 printf("#                          Results\n");
 printf("# Results for %s rng, using its %d valid bits:\n",gsl_rng_name(rng),rmax_bits);
 printf("# (Cumulated mask of zero is good.)\n");
 printf("# cumulated_mask = %10u = ",persist.cumulative_mask);
 dumpbits(&persist.cumulative_mask,persist.nbits);
 printf("# randm_mask     = %10u = ",rmax_mask);
 dumpbits(&rmax_mask,persist.nbits);
 printf("# random_max     = %10u = ",random_max);
 dumpbits(&random_max,persist.nbits);
 if(persist.cumulative_mask){
   printf("# rgb_persist test FAILED (bits repeat)\n");
 } else {
   printf("# rgb_persist test PASSED (no bits repeat)\n");
 }
 printf("#==================================================================\n");

}
