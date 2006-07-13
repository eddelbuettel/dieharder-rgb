/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard Squeeze test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *:               This is the SQEEZE test                                  ::
 *:  Random integers are floated to get uniforms on [0,1). Start- ::
 *:  ing with k=2^31=2147483647, the test finds j, the number of  ::
 *:  iterations necessary to reduce k to 1, using the reduction   ::
 *:  k=ceiling(k*U), with U provided by floating integers from    ::
 *:  the file being tested.  Such j's are found 100,000 times,    ::
 *:  then counts for the number of times j was <=6,7,...,47,>=48  ::
 *:  are used to provide a chi-square test for cell frequencies.  ::
 *
 *                   Comment on SQUEEZE
 * This adaptation fixes a number of screwy fortranisms in the
 * original code.  First of all, the actual bin probabilities were
 * presented scaled up by 10^6 (suitable for 10^6 trials).  Then
 * they were multipled by 0.1.  Finally the test was run for 10^5
 * trials.  Now we just input the vector of actual bin probabilities
 * as doubles (naturally) and scale the probabilities by tsamples.
 * This yields the expected bin frequencies much more simply and
 * in a way that permits tsamples to be varied.
 *
 * Honestly, from my limited experimentation, this test is uselessly
 * insensitive on at least the rng's in the GSL with a few notable
 * exceptions (the worst of the worst).  It passes a number of
 * generators with known, serious flaws though.  Not a lot of
 * tests in diehard that seem to be good at picking out particular
 * flaws in particular generators -- they're struggling to identify
 * bad generators at all.  Sorry, but that's just the way I see it.
 *========================================================================
 */


#include "dieharder.h"

double diehard_squeeze()
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
  * If this test is run by itself, we can ignore tsamples.  If it is
  * part of a "standard run", we have to use specific values.  Either
  * way, we have to adjust the sizes of e.g. the list of integers to
  * be generated and sampled, and (re)allocate memory accordingly.
  * Then at the bottom, we have to put it all back.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 100000;  /* Standard value from diehard */
 }

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_squeeze();
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test = %u.  Diehard default is 100000\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_squeeze_test);

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
 printf("# p = %8.6f for diehard_squeeze test (mean) from\n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_squeeze.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_squeeze.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_squeeze.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_squeeze.\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");

 /*
  * Put back tsamples, free ks_pvalue.
  */
 if(all == YES){
   tsamples = tempsamples;
 }
 free(ks_pvalue);

 return(pks);

}

/*
 * We have to memset to get this into Btest.  Alternatively we could
 * futz a bit with the pointer, but memset is cheap so who cares.
 */
double sdata[]={
0.00002103, 0.00005779, 0.00017554, 0.00046732, 0.00110783,
0.00236784, 0.00460944, 0.00824116, 0.01362781, 0.02096849,
0.03017612, 0.04080197, 0.05204203, 0.06283828, 0.07205637,
0.07869451, 0.08206755, 0.08191935, 0.07844008, 0.07219412,
0.06398679, 0.05470931, 0.04519852, 0.03613661, 0.02800028,
0.02105567, 0.01538652, 0.01094020, 0.00757796, 0.00511956,
0.00337726, 0.00217787, 0.00137439, 0.00084970, 0.00051518,
0.00030666, 0.00017939, 0.00010324, 0.00005851, 0.00003269,
0.00001803, 0.00000982, 0.00001121
};
void diehard_squeeze_test()
{

 int i,j,k;
 Btest btest;

 /*
  * Squeeze counts the iterations required to reduce 2^31 to
  * to 1 with k = ceiling(k*U) where U is a uniform deviate. It
  * does this tsamples times, binning the result in a vector from
  * <= 6 to >= 48 (where it has nontrivial support).  A chisq test
  * on the vector (Btest) then yields a pvalue for the test run.
  */

 /*
  * Allocate memory for Btest struct vector (length 51) and initialize
  * it with the expected values.
  */
 Btest_create(&btest,43,"diehard_squeeze",gsl_rng_name(rng));
 /*
  * Initialize the expected value vector
  */
 for(i=0;i<43;i++){
   btest.y[i] = tsamples*sdata[i];
 }
 memset(btest.x,0,43*sizeof(double));

 /*
  * Test this.
  */
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   for(i=0;i<43;i++){
     printf("%d:   %f    %f\n",i+6,btest.x[i],btest.y[i]);
   }
 }

 /*
  * We now squeeze tsamples times.
  */
 for(i=0;i<tsamples;i++){
   k = 2147483647;
   j = 0;

   /* printf("%d:   %d\n",j,k); */
   while((k != 1) && (j < 48)){
     k = ceil(k*gsl_rng_uniform(rng));
     j++;
     /* printf("%d:   %d\n",j,k); */
   }

   /*
    * keep j in range 6-48 inclusive and increment the test/counting vector.
    */
   j = (j<6)?6:j;
   btest.x[j-6]++;

 }

 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   for(i=0;i<43;i++){
     printf("%d:   %f    %f\n",i+6,btest.x[i],btest.y[i]);
   }
 }

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("# diehard_squeeze_freq(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

}

void help_diehard_squeeze()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"squeeze\" test (modified).\n\
#  Random integers are floated to get uniforms on [0,1). Start- \n\
#  ing with k=2^31=2147483647, the test finds j, the number of  \n\
#  iterations necessary to reduce k to 1, using the reduction   \n\
#  k=ceiling(k*U), with U provided by floating integers from    \n\
#  the file being tested.  Such j's are found 100,000 times,    \n\
#  then counts for the number of times j was <=6,7,...,47,>=48  \n\
#  are used to provide a chi-square test for cell frequencies.  \n\
#==================================================================\n");

}
