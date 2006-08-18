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

void run_rgb_bitdist()
{

 /*
  * Declare the results struct.
  */
 Test bitdist_test;
 double pks;

 test_header(&rgb_bitdist_data);

 /*
  * Call the actual test that fills in the results struct.
  */
 rgb_bitdist(&bitdist_test);

 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
   printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
 }

 /*
  * Test Results, standard form.
  */
 test_footer(&rgb_bitdist_data,bitdist_test.ks_pvalue,bitdist_test.pvalues);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for %1d-tuplets.  rgb_bitdist terminating.\n",gsl_rng_name(rng),ntuple);
   /* return(pks); */
 }
 printf("#========================================================================\n");

}
