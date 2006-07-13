/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard Craps test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *: This is the CRAPS TEST. It plays 200,000 games of craps, finds::
 *: the number of wins and the number of throws necessary to end  ::
 *: each game.  The number of wins should be (very close to) a    ::
 *: normal with mean 200000p and variance 200000p(1-p), with      ::
 *: p=244/495.  Throws necessary to complete the game can vary    ::
 *: from 1 to infinity, but counts for all>21 are lumped with 21. ::
 *: A chi-square test is made on the no.-of-throws cell counts.   ::
 *: Each 32-bit integer from the test file provides the value for ::
 *: the throw of a die, by floating to [0,1), multiplying by 6    ::
 *: and taking 1 plus the integer part of the result.             ::
 *
 *========================================================================
 */

#define MALLOC_CHECK 1
#include "dieharder.h"

double diehard_craps()
{

 double *pvalue,pks_mean,pks_freq;
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
  * If this test is run by itself, we can ignore tsamples.  If it is
  * part of a "standard run", we have to use specific values.  Either
  * way, we have to adjust the sizes of e.g. the list of integers to
  * be generated and sampled, and (re)allocate memory accordingly.
  * Then at the bottom, we have to put it all back.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 200000;  /* Standard value from diehard */
 }

 /*
  * Safely allocate memory for ks pvalues.
  */
 if(ks_pvalue) free(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 if(ks_pvalue2) free(ks_pvalue2);
 ks_pvalue2 = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_craps();
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test = %u.  Diehard default is 200000\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
 }

 kspi = 0;  /* Always zero first */
 pks_mean = sample((void *)diehard_craps_test);
 /* This is an extra for craps only */
 pks_freq = kstest_kuiper(ks_pvalue2,kspi);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 if(hist_flag){
   histogram(ks_pvalue2,psamples,0.0,1.0,10,"p-values");
 }
 if(!quiet){
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
     printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
   }
 }
 printf("#                          Results\n");
 printf("# p = %8.6f for diehard_craps test (mean) from\n",pks_mean);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks_mean < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_craps (mean).\n",gsl_rng_name(rng));
 } else if(pks_mean < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_craps (mean).\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks_mean < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_craps (mean).\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_craps (mean).\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");
 printf("# p = %8.6f for diehard_craps test (freq) from\n",pks_freq);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks_freq < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_craps (freq).\n",gsl_rng_name(rng));
 } else if(pks_freq < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_craps (freq).\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks_freq < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_craps (freq).\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_craps (freq).\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = tempsamples;
 }

 if(ks_pvalue) free(ks_pvalue);
 if(ks_pvalue2) free(ks_pvalue2);
 

 return(pks_mean);

}

uint roll(){
  uint d = 1 + gsl_rng_uniform_int(rng,6);
  return d;
}

void diehard_craps_test()
{

 uint i,j;
 uint point,throw,tries,wins;
 double sum,p;
 Xtest ptest;
 Btest btest;

 /*
  * ptest.x = number of wins
  *   p = 244.0/495.0 is the probability of winning, so the mean
  * should be normally distributed with a binary distribution
  * sigma (standard stuff).
  * ptest.y = tsamples*p
  * ptest.sigma = sqrt(tsamples*p*(1 - p))
  *
  * HOWEVER, it also counts the number of throws required to win
  * each game, binned according to 1,2,3... 21+ (the last bin is
  * holds all games that require more than 20 throws).  The
  * vector of bin values is subjected to a chi-sq test.  BOTH
  * tests must be passed, making this one a bit more complex to
  * report on, as it is really two tests in one.
  */
 p = 244.0/495.0;
 ptest.y = (double) tsamples*p;
 ptest.sigma = sqrt(ptest.y*(1.0 - p));

 /*
  * Allocate memory for Btest struct vector (length 21) and initialize
  * it with the expected values.
  */
 Btest_create(&btest,21,"diehard_craps",gsl_rng_name(rng));
 sum = 1.0/3.0;
 btest.y[0] = sum;
 for(i=1;i<20;i++){
   btest.y[i] = (27.0*pow(27.0/36.0,i-1) + 40*pow(13.0/18.0,i-1) +
                55.0*pow(25.0/36.0,i-1))/648.0;
   sum += btest.y[i];
   btest.sigma[i] = 0.0;   /* No longer used */
 }
 btest.y[20] = 1.0 - sum;
 /*
  * Normalize the probabilities by the expected number of trials
  */
 for(i=0;i<21;i++){
   btest.y[i] *= tsamples;
 }



 /*
  * Initialize sundry things.  This is short enough I'll use
  * a loop instead of memset.
  */
 for(i=0;i<21;i++) btest.x[i] = 0;
 wins = 0;

 /*
  * We now play tsamples games of craps!
  */
 for(i=0;i<tsamples;i++){

   /*
    * This is the point count we have to make, the sum of two rolled
    * dice.
    */
   point = roll() + roll();
   tries = 0;

   if(point == 7 || point == 11) {
     /*
      * If we rolled 7 or 11, we just win.
      */
     wins++;
     btest.x[tries]++;
   } else if(point == 2 || point == 3 || point == 12){
     /*
      * If we rolled 2, 3, or 12, we just lose.
      */
     btest.x[tries]++;
   } else {
     /*
      * We have to roll until we make the point (win) or roll
      * a seven (lose).  We have to keep going until we win
      * or lose, but have to compress the number of throws
      * to bin 21 for all throw>21.
      */
     while(1){
       /*
        * This little ditty increments tries if it is less than 20
	* then freezes it.
        */
       (tries<20)?tries++:tries;
       throw = roll() + roll();
       if(throw == 7){
         btest.x[tries]++;
	 break;
       } else if(throw == point){
         btest.x[tries]++;
	 wins++;
	 break;
       }
     }
   }
 }

 ptest.x = wins++;
 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_DIEHARD_CRAPS || verbose == D_ALL){
   printf("# diehard_craps(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 Btest_eval(&btest);
 ks_pvalue2[kspi] = btest.pvalue;
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("# diehard_craps_freq(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue2[kspi]);
 }
 kspi++;

}

void help_diehard_craps()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"craps\" test (modified).\n\
#  This is the CRAPS TEST. It plays 200,000 games of craps, finds  \n\
#  the number of wins and the number of throws necessary to end    \n\
#  each game.  The number of wins should be (very close to) a      \n\
#  normal with mean 200000p and variance 200000p(1-p), with        \n\
#  p=244/495.  Throws necessary to complete the game can vary      \n\
#  from 1 to infinity, but counts for all>21 are lumped with 21.   \n\
#  A chi-square test is made on the no.-of-throws cell counts.     \n\
#  Each 32-bit integer from the test file provides the value for   \n\
#  the throw of a die, by floating to [0,1), multiplying by 6      \n\
#  and taking 1 plus the integer part of the result.               \n\
#==================================================================\n");

}
