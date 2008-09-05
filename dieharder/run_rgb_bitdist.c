/*
 *========================================================================
 * $Id: dieharder.c 127 2004-11-20 18:17:55Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  run_rgb_bitdist()
 *========================================================================
 */

#include "dieharder.h"

void run_rgb_bitdist()
{

 /*
  * Declare the results struct.
  */
 Test **rgb_bitdist_test;
 uint nt,ntmin,ntmax;

 /*
  * Set any GLOBAL data used by the test.  rgb_ntuple is the value
  * assigned by -n ntuple (max) on the command line, or default 0 which
  * means -- do all ntuples in range 1 to 8.  rgb_bitdist_dtest.nkps is
  * the number of pvalues to be returned, which is just ntuple.
  *
  * Note that we have to do this BEFORE creating the test because
  * dtest.nkps depends on the value of ntuple!
  */
 if(ntuple){
   ntmax = ntuple;
   ntmin = ntuple;
 } else {
   /*
    * Default is to test 1 through 12 bits, which takes
    * around 30 minutes on my laptop but is a VERY thorough
    * test of randomness.
    */
   ntmin = 1;
   ntmax = 12;
 }
 if(all == YES) ntmin = 1;

 /* printf("Setting ntmin = %d ntmax = %d\n",ntmin,ntmax); */
 for(nt = ntmin;nt <= ntmax;nt++){
   /*
    * First we create the test (to set some values displayed in test header
    * correctly).
    */
   rgb_bitdist_test = create_test(&rgb_bitdist_dtest,tsamples,psamples,&rgb_bitdist);
   /*
    * Set this to pass it into the test and on to report() or table().
    */
   rgb_bitdist_test[0]->ntuple = nt;

   /*
    * If not table, then report.
    * Show the standard test header for this test.
    */
   if(!table && !quiet){
     show_test_header(&rgb_bitdist_dtest,rgb_bitdist_test);
   }

   /*
    * Set any GLOBAL data used by the test.  Then call the test itself
    * This fills in the results in the Test struct.
    */
   std_test(&rgb_bitdist_dtest,rgb_bitdist_test);

   /*
    * This almost certainly belongs in the show_test_results section,
    * possibly with additional conditionals rejecting test results involving
    * rewinds, period.
    */
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     if(!quiet && !table){
       fprintf(stdout,"# %u rands were used in this test\n",file_input_get_rtot(rng));
     }
     if(file_input_get_rewind_cnt(rng) != 0){
       fprintf(stderr,"# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
       fflush(stderr);
     }
   }

   /*
    * Show standard test results, for all statistics generated by a single run.
    * Output can either be a report (with histogram and all) or a table
    * line.
    */
   if(table) {
     table_line(&rgb_bitdist_dtest,rgb_bitdist_test);
   } else {
     report(&rgb_bitdist_dtest,rgb_bitdist_test);
   }

   /*
    * Free any GLOBAL data used by the test.
    */
   destroy_test(&rgb_bitdist_dtest,rgb_bitdist_test);

 }

}
