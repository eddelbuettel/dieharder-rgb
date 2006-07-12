/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard OPSO test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *         OPSO means Overlapping-Pairs-Sparse-Occupancy         ::
 * The OPSO test considers 2-letter words from an alphabet of    ::
 * 1024 letters.  Each letter is determined by a specified ten   ::
 * bits from a 32-bit integer in the sequence to be tested. OPSO ::
 * generates  2^21 (overlapping) 2-letter words  (from 2^21+1    ::
 * "keystrokes")  and counts the number of missing words---that  ::
 * is 2-letter words which do not appear in the entire sequence. ::
 * That count should be very close to normally distributed with  ::
 * mean 141,909, sigma 290. Thus (missingwrds-141909)/290 should ::
 * be a standard normal variable. The OPSO test takes 32 bits at ::
 * a time from the test file and uses a designated set of ten    ::
 * consecutive bits. It then restarts the file for the next de-  ::
 * signated 10 bits, and so on.                                  ::
 *
 *                         Comment
 * Damn good test.  Very few generators in the GSL survive it,
 * not even the various glibc generators.  Pretty much mt1997,
 * ranldx, rand48 -- nearly everything else I tried in a very
 * non-exhaustive trial failed -- all the NR generators, R250,
 * and more.  It appears to be related (I think) to Knuth's
 * hyperplanar distribution tests -- a matter for future research --
 * when you code it (as I did) thinking of it as a kind of really
 * big "parking lot" test or as generating a pixel graph in 2d.
 *
 * The tests BITSTREAM, OPSO, OQSO and DNA are all closely related.
 *========================================================================
 */


#include "dieharder.h"

double diehard_opso()
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
  * This test requires a fixed number of tsamples, alas.  Deviation
  * not permitted, whether or not we are running a single test and
  * trying to set -t whatever.
  */
 tempsamples = tsamples;
 tsamples = 2097153;  /* Standard value from diehard */

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
  help_diehard_opso();
  printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test FIXED at %u.\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
   printf("# Number of rands required is around 2^28 for 100 samples.\n");
   if(overlap){
     printf("# Using overlapping samples (diehard).\n");
   } else {
     printf("# Using non-overlapping samples (default).\n");
   }
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_opso_test);

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
 printf("# p = %8.6f for diehard_opso test from\n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_opso.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_opso.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_opso.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_opso.\n",gsl_rng_name(rng));
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

void diehard_opso_test()
{

 uint i,j0,k0,j,k,boffset,t;
 Xtest ptest;
 char **w;

 /*
  * p = 141909, with sigma 290, FOR tsamples 2^21+1 2 letter words.
  * These cannot be varied unless one figures out the actual
  * expected "missing works" count as a function of sample size.  SO:
  *
  * ptest.x = number of "missing words" given 2^21+1 trials
  * ptest.y = 141909
  * ptest.sigma = 290
  */
 ptest.y = 141909.0;
 ptest.sigma = 290.0;

 /*
  * We now make tsamples measurements, as usual, to generate the
  * missing statistic.  The easiest way to proceed, I think, will
  * be to generate a simple char matrix 1024x1024 in size and empty.
  * Each pair of "letters" generated become indices, and a (char) 1
  * is inserted there.  At the end we just loop the matrix and count
  * the zeros.
  *
  * Of course doing it THIS way it is pretty obvious that we could,
  * say, display the 2-color 1024x1024 bitmap this represented graphically.
  * Missing words are just pixels that are still in the background color.
  * Hmmm, sounds a whole lot like Knuth's test looking for hyperplanes
  * in 2 dimensions, hmmm.  At the very least, any generator that produces
  * hyperplanar banding at 2 dimensions should fail this test, but it is
  * possible for it to find distributions that do NOT have banding but
  * STILL fail the test, I suppose.  Projectively speaking, though,
  * I have some fairly serious doubts about this, though.
  */

 w = (char **)malloc(1024*sizeof(char *));
 for(i=0;i<1024;i++){
   w[i] = (char *)malloc(1024*sizeof(char));
   /* Zero the column */
   memset(w[i],0,1024*sizeof(char));
 }

/*
 printf("w is allocated and zero'd\n");
 printf("About to generate %u samples\n",tsamples);
 */
 /*
  * To minimize the number of rng calls, we use each j and k mod 32
  * to determine the offset of the 10-bit long string (with
  * periodic wraparound) to be used for the next iteration.  We
  * therefore have to "seed" the process with a random k.
  */
 k = gsl_rng_get(rng);
 for(t=0;t<tsamples;t++){
   if(overlap){
     /*
      * Let's do this the cheap/easy way first, sliding a 20 bit
      * window along each int for the 32 possible starting
      * positions a la birthdays, before trying to slide it all
      * the way down the whole random bitstring implicit in a
      * long sequence of random ints.  That way we can exit
      * the tsamples loop at tsamples = 2^15...
      */
     if(t%32 == 0) {
       j0 = gsl_rng_get(rng);
       k0 = gsl_rng_get(rng);
       boffset = 0;
     }
     /*
      * Get two "letters" (indices into w)
      */
     j = get_bit_ntuple(&j0,1,10,boffset);
     k = get_bit_ntuple(&k0,1,10,boffset);
     /* printf("%u:   %u  %u  %u\n",t,j,k,boffset); */
     w[j][k]++;
     boffset++;

   } else {
     /*
      * Get two "letters" (indices into w).  Here we use
      * what is basically a random offset sample to sample
      */
     boffset = k%32;
     j = gsl_rng_get(rng);
     j = get_bit_ntuple(&j,1,10,boffset);
     boffset = j%32;
     k = gsl_rng_get(rng);
     k = get_bit_ntuple(&k,1,10,boffset);
     w[j][k]++;
   }
 }
 /*
  * Now we count the holes, so to speak
  */
 ptest.x = 0;
 for(j=0;j<1024;j++){
   for(k=0;k<1024;k++){
     if(w[j][k] == 0){
       ptest.x += 1.0;
       /* printf("ptest.x = %f  Hole: w[%u][%u] = %u\n",ptest.x,j,k,w[j][k]); */
     }
   }
 }
 if(verbose == D_DIEHARD_OPSO || verbose == D_ALL){
   printf("%f %f %f\n",ptest.y,ptest.x,ptest.x-ptest.y);
 }

 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_DIEHARD_OPSO || verbose == D_ALL){
   printf("# diehard_opso(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

 for(i=0;i<1024;i++){
   free(w[i]);
 }
 free(w);

}

void help_diehard_opso()
{

 printf("\n\
#==================================================================\n\
#                    Diehard \"OPSO\" test.\n\
# The OPSO test considers 2-letter words from an alphabet of    \n\
# 1024 letters.  Each letter is determined by a specified ten   \n\
# bits from a 32-bit integer in the sequence to be tested. OPSO \n\
# generates  2^21 (overlapping) 2-letter words  (from 2^21+1    \n\
# \"keystrokes\")  and counts the number of missing words---that  \n\
# is 2-letter words which do not appear in the entire sequence. \n\
# That count should be very close to normally distributed with  \n\
# mean 141,909, sigma 290. Thus (missingwrds-141909)/290 should \n\
# be a standard normal variable. The OPSO test takes 32 bits at \n\
# a time from the test file and uses a designated set of ten    \n\
# consecutive bits. It then restarts the file for the next de-  \n\
# signated 10 bits, and so on.                                  \n\
#==================================================================\n");

}
