/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is a test that checks to see if all the bits returned by the
 * rng change.  Surprisingly, several generators have bits that do NOT
 * change, hence the test.  It also reveals tests that for whatever reason
 * return less than the expected uint number of bits (32) as unchanging
 * high bits
 *
 * This (and all further rgb_ tests) are "my own".  Some of them may turn
 * out to be formally equivalent to diehard or sts or knuth tests in the
 * specific sense that failure in one always matches or precedes failure
 * in the other.
 *========================================================================
 */

#include "dieharder.h"
/*
 * Test specific data
 */
#include "rgb_persist.h"

double rgb_persist()
{

 double pks;
 uint ps_save,ts_save;

 int i,j,nbits,csamples;
 static unsigned and_mask,cumulative_mask,last_rand;

 /*
  * Do a standard test if -a(ll) is selected.
  * ALSO use standard values if tsamples or psamples are 0
  */
 if(all == YES){
   ts_save = tsamples;
   tsamples = dtest->tsamples_std;
   ps_save = psamples;
   psamples = dtest->psamples_std;
 }
 if(tsamples == 0){
   tsamples = dtest->tsamples_std;
 }
 if(psamples == 0){
   psamples = dtest->psamples_std;
 }

 /*
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.
  *
  * We arbitrarily choose 256 successive random numbers as enough.
  * This should be plenty -- the probability of any bit slot not
  * changing by CHANCE in 256 tries is 2^-256 or almost 10^-26,
  * so even with 32 bit slots, repeatedly, we aren't horribly likely
  * to see it in our lifetime unless we run this test continuously for
  * months at a time (yes, a dumb idea).
  */
 if(rand_int) nullfree(rand_int);
 rand_int = (unsigned int*)malloc(256 * sizeof(unsigned int));

 test_header(dtest);
 printf("# Samples per test run = %u, tsamples ignored\n",256);
 printf("# Test run %u times to cumulate unchanged bit mask\n",psamples);

 /*
  * Now go through the list and dump the numbers several ways.
  */
 if(bits > 32) {
   nbits = 32;
 } else {
   nbits = bits;
 }

 cumulative_mask = 0;
 for(j=0;j<psamples;j++){
   /*
    * Do not reset the total count of rands or rewind file input
    * files -- let them auto-rewind as needed.  Otherwise try
    * different seeds for different samples.
    */
   if(strncmp("file_input",gsl_rng_name(rng),10)){
     seed = random_seed();
     gsl_rng_set(rng,seed);
   }
   /*
    * Fill rand_int with a string of random numbers
    */
   for(i=0;i<256;i++) rand_int[i] = gsl_rng_get(rng);
   last_rand = rand_int[0];  /* to start it */
   and_mask = ~(last_rand ^ rand_int[0]);
   for(i=0;i<256;i++){
     if(verbose){
       printf("rand_int[%d] = %u = ",i,rand_int[i]);
       dumpbits(&rand_int[i],nbits);
     }

     /*
      * Now we make a mask of bits that coincide. Logic 41, where are you?
      */
     and_mask = and_mask & (~(last_rand ^ rand_int[i]));
     if(verbose){
       printf("and_mask = %u = ",and_mask);
       dumpbits(&and_mask,nbits);
     }

   }
   and_mask = and_mask & rmax_mask;
   cumulative_mask = cumulative_mask | and_mask;
 }


 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
   printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
 }
 printf("#==================================================================\n");
 printf("#                          Results\n");
 printf("# Results for %s rng, using its %d valid bits:\n",gsl_rng_name(rng),rmax_bits);
 printf("# (Cumulated mask of zero is good.)\n");
 printf("# cumulated_mask = %10u = ",cumulative_mask);
 dumpbits(&cumulative_mask,nbits);
 printf("# randm_mask     = %10u = ",rmax_mask);
 dumpbits(&rmax_mask,nbits);
 printf("# random_max     = %10u = ",random_max);
 dumpbits(&random_max,nbits);
 if(cumulative_mask){
   printf("# rgb_persist test FAILED (bits repeat)\n");
 } else {
   printf("# rgb_persist test PASSED (no bits repeat)\n");
 }
 printf("#==================================================================\n");

 if(rand_int) nullfree(rand_int);
 if(cumulative_mask) {
   return(0.0);
 } else {
   return(0.5);
 }

}

void help_rgb_persist()
{

 printf("%s",dtest->description);

}
