/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 * See also accompanying file STS.COPYING
 *
 */

/*
 *========================================================================
 * This is a test that checks to see if the rng generates bits that are
 * binomially distributed.  It checks the actual distribution of bits for
 * relatively small strings (small enough that n! doesn't overflow and
 * 2^-n doesn't underflow, so we can evaluate meaningful probabilities).
 * 
 * The way it works is the following:  We loop the core "samples" times.
 * The core generates a string of ints large enough to sample a bitstring
 * of length "bits", "size" times.  It accumulates a histogram of bit
 * frequencies over that sample, and then constructs a chisq of that
 * histogram compared to the expected histogram.  It converts the chisq
 * into a pvalue (where we may or may not need to exercise care out in
 * the histogram tails, where the expected value is a fraction of an
 * integer) and saves the value in the vector pvalue in the samples loop.
 * Finally, we run a set of tests on the pvalue vector to arrive at a
 * conclusion as to whether or not the rng is probably non-random.
 *
 * This last bit is interesting, as the final conclusion is itself a p-value.
 * The only way this final step of using e.g. Kolmogorov-Smirnov tests
 * to analyze the distribution of p's themselves to generate a final
 * "aggregate" pvalue is useful is if the test can be extended to where
 * the failure is "obvious" -- the final KS p value is very low, consistently.
 * This, in turn, means that the histogram of p-values that it analyzes has
 * to be consistently non-uniform (probably biased low, but we'll see).
 * It is STILL not at all clear that there is any advantage to effectively
 * partitioning the data (which is what this does) and generating lots of
 * independent samples of a test versus simply running the test samples* as
 * long.  The most that it is likely to yield is "the warm fuzzies" that
 * come from seeing that non-uniform distribution of p-values from many
 * samples as opposed to just one really low p-value that COULD have been
 * a statistical fluke.
 *
 * This (and all the rgb_ tests) are "my own" in that I'm BOTH
 * making them up AND writing them as I go.  If they turn out to be
 * equivalent to STS tests I'll document this and probably convert to
 * the STS versions.  The exercise of conceiving and writing tests is
 * still invaluable.
 *========================================================================
 */

#include "rand_rate.h"

void rgb_binomial()
{

 unsigned int i,j,k,n,nbits,npts,nsize;
 unsigned int num_ones,num_zeros,num_bits;
 unsigned int thisbit,tot_1,tot_bits;
 double tot_onefrac,tot_oneexp,tot_onefrac_sigma,tot_onefrac_delta;
 double *pvalue,pks,ptmp;
 Ntest btest;   /* For binomial test results */

 if(bits > 128){
   nbits = bits = 128;
 } else {
   nbits = bits;
 }
 /*
  * Because each rng is only rmax_bits long (which can be less than
  * 32) we may need to generate more than bits/32 ints per tested
  * string.  We check to make sure that nsize big enough, as if
  * rmax_bits < 32 the int division will truncate instead of round up.
  */
 nsize = 0;
 while(nsize*rmax_bits < nbits) nsize++;

 pvalue = (double *)malloc(samples*sizeof(double));

 /*
  * Each run checks size bitstrings and generates a binomial pvalue
  * from the binomial distribution.  The pvalue is accumulated and saved
  * samples times, then this is vector of pvalues is subjected to a
  * kstest and possibly other checks of uniformity of the p-values
  * generated.
  *
  * btest.x[k] is an accumulator to be used to count the frequency with
  *   which we get k 1's in num_bits, over the m runs.
  * btest.y[k] contains the expected/exact result.
  * btest.sigma[k] contains the standard error in btest.y[k].
  *
  * We start by creating all three and filling y[k] and sigma[k]
  * to support the formation of chisq as we run.
  */
 npts = nbits+1;
 Ntest_create(&btest,npts,"rgb_binomial",gsl_rng_name(rng));
 for(k=0;k<npts;k++){
   ptmp = binomial(nbits,k,0.5);
   btest.y[k] = size*ptmp;
   btest.sigma[k] = sqrt(btest.y[k]*(1.0 - ptmp));
 }

 /*
  * Next we run the sampling loop to accumulate num_bit results
  * in btest.x[k].  For the moment we print out each sample as well.
  */
 if(!quiet){
   printf("#==================================================================\n");
   printf("#                 %s Test Description\n",btest.testname);
   printf("# Evaluates the number of 1's in %d strings of %d bits and\n",samples,nbits);
   printf("# compares the result histogram to the binomial distribution.\n");
   printf("# From chisq and the incomplete gamma function the probability\n");
   printf("# of this histogram if the rng tested is in fact \"random\" is\n");
   printf("# obtained.  If this p-value is low the hypothesis that it is\n");
   printf("# random safely can be rejected\n");
   printf("#==================================================================\n");
   printf("# sample     pvalue\n");
 }
 for(i=0;i<samples;i++){

   /* If requested, reseed the rng per sample */
   if(reseed_flag) {
     seed = random_seed();
     gsl_rng_set(rng,seed);
   }

   /* Clear the results histogram */
   for(k=0;k<npts;k++){
     btest.x[k] = 0.0;
   }

   /*
    * Now we check size (NOT nsize) bitstrings of bits in length,
    * counting the 1's.  At the end we increment the result histogram
    * with the bitcount as an index as a trial that generated that
    * bitcount.
    */
   for(j=0;j<size;j++){

     /*
      * Fill vector of "random" integers with selected generator.
      * NOTE WELL:  This can also be done by reading in a file!
      */
     if(verbose){
       printf("Generating %u bits in rand_int[]\n",nsize*sizeof(uint)*8);
     }
     for(k=0;k<nsize;k++) {
       rand_int[k] = gsl_rng_get(rng);
       if(verbose){
         printf("rand_int[%d] = %u\n",k,rand_int[k]);
         dumpbits(&rand_int[k],8*sizeof(uint));
       }
     }

     if(verbose){
       printf("# Counting bitstring: ");
     }
     num_ones = 0;
     n = 0;
     for(k=0;k<nbits;k++){
       if(thisbit = get_bit(n++)) {
         num_ones++;
       }
       if(verbose){
         printf("%1d",thisbit);
       }
     }
     /*
      * Increment the counter for the resulting number of ones.
      */
     btest.x[num_ones]++;
     if(verbose){
       printf("\n# Sample %u: btest[%u] = %u\n",j,num_ones,(uint)btest.x[num_ones]);
     }
   }

   /*
    * Finally, we save THIS sample's pvalue in the pvalue vector, and
    * move on.
    */
   Ntest_eval(&btest);
   pvalue[i] = btest.pvalue;
   if(!quiet){
     printf("  %u        %6.2f\n",i,pvalue[i]);
   }

 }

 pks = kstest(pvalue,samples);
 printf("p = %6.3f from standard Komogorov-Smirnov test on %u pvalues.\n",pks,samples);
 if(pks>0.01){
   printf("Generator appears to be ok.\n");
 } else {
   printf("Generator fails at 1%% confidence level.\n");
 }

 pks = kstest_kuiper(pvalue,samples);
 printf("p = %6.3f from Kuiper Komogorov-Smirnov test on %u pvalue.\n",pks,samples);
 if(pks>0.01){
   printf("Generator appears to be ok.\n");
 } else {
   printf("Generator fails at 1%% confidence level.\n");
 }
 
 

 Ntest_destroy(&btest);

}

