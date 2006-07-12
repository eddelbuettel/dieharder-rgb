/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard Parking Lot test, rewritten from the description
 * in tests.txt on  George Marsaglia's diehard site.
 *
 * ::               THIS IS A PARKING LOT TEST                      ::
 * :: In a square of side 100, randomly "park" a car---a circle of  ::
 * :: radius 1.   Then try to park a 2nd, a 3rd, and so on, each    ::
 * :: time parking "by ear".  That is, if an attempt to park a car  ::
 * :: causes a crash with one already parked, try again at a new    ::
 * :: random location. (To avoid path problems, consider parking    ::
 * :: helicopters rather than cars.)   Each attempt leads to either ::
 * :: a crash or a success, the latter followed by an increment to  ::
 * :: the list of cars already parked. If we plot n:  the number of ::
 * :: attempts, versus k::  the number successfully parked, we get a::
 * :: curve that should be similar to those provided by a perfect   ::
 * :: random number generator.  Theory for the behavior of such a   ::
 * :: random curve seems beyond reach, and as graphics displays are ::
 * :: not available for this battery of tests, a simple characteriz ::
 * :: ation of the random experiment is used: k, the number of cars ::
 * :: successfully parked after n=12,000 attempts. Simulation shows ::
 * :: that k should average 3523 with sigma 21.9 and is very close  ::
 * :: to normally distributed.  Thus (k-3523)/21.9 should be a st-  ::
 * :: andard normal variable, which, converted to a uniform varia-  ::
 * :: ble, provides input to a KSTEST based on a sample of 10.      ::
 *
 *                         Comments
 *
 *    First, the description above is incorrect in two regards.
 *    As seen in the original code, the test measures
 *    overlap of SQUARES of radius one, a thing I only observed after
 *    actually programming circles as described (which are
 *    also easy, although a bit more expensive to evaluate crashes
 *    for).  Circles produce an altogether different mean and are
 *    probably a bit more sensitive to 2d striping at arbitrary angles.
 *
 *    Note that I strongly suspect that this test is basically
 *    equivalent to Knuth's better conceived hyperplane test, which
 *    measures aggregation of N dimensional sets of "coordinates" in
 *    hyperplanes.  To put it another way, if something fails a
 *    hyperplane test in 2d, it will certainly fail this test as well.
 *    If something fails this test, I'd bet serious money that it
 *    is because of aggregation of points on hyperplanes although
 *    there MAY be other failure patterns as well.
 *
 *    Finally, note that the probability that any given k is
 *    obtained for a normal random distribution is just determined
 *    from the erf() -- this is just an Xtest().
 *
 * As always, we will increase the number of tsamples and hopefully improve
 * the resolution of the test.  However, it should be carefully noted
 * that modern random number generators can almost certainly add many
 * decimal places to the simulation value used in this test.  In other
 * words, test failure at higher resolution can be INVERTED -- it can
 * indicate the relative failure of the generators used to produce the
 * earlier result!  This is really a subject for future research...
 *========================================================================
 */


#include "dieharder.h"

double diehard_parking_lot()
{

 double *pvalue,pks;
 uint tempsamples;

 /*
  * This is the merest shell to set any test-specific variables, call
  * the main test routine (which fills one or more slots in ks_pvalue[]
  * and increments kspi accordingly), and run a Kuiper Kolmogorov-Smirnov
  * test on the vector of pvalues produced and turn it into a single,
  * cumulative p-value (pks) for the entire test.  If the test parameters
  * are set properly, this will USUALLY yield an unambiguous signal of
  * failure.
  */

 /*
  * tsamples are irrelevant to this test, which consumes roughly
  * 12000x2x100 = 2.4x10^6 rands.
  */

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_parking_lot();
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test FIXED at %u.\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
   printf("# Number of rands required is 2,400,000 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_parking_lot_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 if(!quiet){
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
     printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
   }
 }
 printf("#                          Results\n");
 printf("# p = %8.6f for diehard_parking_lot test from\n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues (up runs + down runs).\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_parking_lot.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_parking_lot.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_parking_lot.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_parking_lot.\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");

 free(ks_pvalue);

 return(pks);

}

typedef struct {
  double x;
  double y;
} Cars;

void diehard_parking_lot_test()
{

 /*
  * This is the most that could under any circumstances be parked.
  */
 Cars parked[12000];
 uint k,n,i,crashed;
 double xtry,ytry,knorm;
 Xtest ptest;

 /*
  * ptest.x = (double) k
  * ptest.y = 3523.0
  * ptest.sigma = 21.9
  * This will generate ptest->pvalue when Xtest(ptest) is called
  */
 ptest.y = 3523.0;
 ptest.sigma = 21.9;

 /*
  * Clear the parking lot the fast way.
  */
 memset(parked,0,12000*sizeof(Cars));

 /*
  * Park a single car to have something to avoid and count it.
  */
 parked[0].x = 100.0*gsl_rng_uniform(rng);
 parked[0].y = 100.0*gsl_rng_uniform(rng);
 k = 1;
 

 /*
  * This is now a really simple test.  Park them cars!  We try to park
  * 12000 times, and increment k (the number successfully parked) on
  * successes.  We brute force the crash test.
  */
 for(n=1;n<12000;n++){
   xtry = 100.0*gsl_rng_uniform(rng);
   ytry = 100.0*gsl_rng_uniform(rng);
   crashed = 0;
   for(i=0;i<k;i++){
     /*
      * We do this REASONABLY efficiently.  As soon as we know we didn't
      * crash we move on until we learn that we crashed, trying to skip
      * arithmetic.  Once we've crashed, we break out of the loop.
      * Uncrashed survivors join the parked list.
      */
     if( (fabs(parked[i].x - xtry) <= 1.0) && (fabs(parked[i].y - ytry) <= 1.0)){
       crashed = 1;  /* We crashed! */
       break;        /* So quit the loop here */
     }
   }
   /*
    * Save uncrashed helicopter coordinates.
    */
   if(crashed == 0){
     parked[k].x = xtry;
     parked[k].y = ytry;
     crashed = 0;
     k++;
   }
 }

 ptest.x = (double)k;
 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_DIEHARD_PARKING_LOT || verbose == D_ALL){
   printf("# diehard_parking_lot(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;
     
}


void help_diehard_parking_lot()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"parking lot\" test (modified).\n\
# This tests the distribution of attempts to randomly park a\n\
# square car of length 1 on a 100x100 parking lot without\n\
# crashing.  We plot n (number of attempts) versus k (number of\n\
# attempts that didn't \"crash\" because the car squares \n\
# overlapped and compare to the expected result from a perfectly\n\
# random set of parking coordinates.  This is, alas, not really\n\
# known on theoretical grounds so instead we compare to n=12,000\n\
# where k should average 3523 with sigma 21.9 and is very close\n\
# to normally distributed.  Thus (k-3523)/21.9 is a standard\n\
# normal variable, which converted to a uniform p-value, provides\n\
# input to a KS test with a default 100 samples.\n\
#==================================================================\n");

}
