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
 *               This is the SQUEEZE test                        ::
 *  Random integers are floated to get uniforms on [0,1). Start- ::
 *  ing with k=2^31=2147483647, the test finds j, the number of  ::
 *  iterations necessary to reduce k to 1, using the reduction   ::
 *  k=ceiling(k*U), with U provided by floating integers from    ::
 *  the file being tested.  Such j's are found 100,000 times,    ::
 *  then counts for the number of times j was <=6,7,...,47,>=48  ::
 *  are used to provide a chi-square test for cell frequencies.  ::
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
/*
 * Test specific data
 */
#include "diehard_squeeze.h"

double diehard_squeeze()
{

 double pks;
 uint ps_save,ts_save;

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
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 }

 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_squeeze_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard Squeeze Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);

 return(pks);

}

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

 printf("%s",dtest->description);

}
