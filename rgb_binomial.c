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
 * It >>also<< does the sts_monobit test on the aggregate string across
 * all smaller samples.
 *
 * This (and all further rgb_ tests) are "my own" in that I'm BOTH
 * making them up AND writing them as I go.  If they turn out to be
 * equivalent to STS tests I'll document this and probably convert to
 * the STS versions.  The exercise of conceiving and writing tests is
 * still invaluable.
 *========================================================================
 */

#include "rand_rate.h"

void rgb_binomial()
{

 int i,j,k,nbits,npts;
 unsigned int num_ones,num_zeros,num_bits;
 unsigned int thisbit;
 double onefrac,ptmp;
 Ntest btest;

 /*
  * btest.x[k] is an accumulator to be used to count the frequency with
  *   which we get k 1's in num_bits, over the m runs.
  * btest.y[k] contains the expected/exact result.
  * btest.sigma[k] contains the standard error in btest.y[k].
  *
  * We start by creating all three and filling y[k] and sigma[k]
  * to support the formation of chisq as we run.
  *
  * Number of total bits (from EITHER -b bits OR -s size, -b overrides -s)
  */
 if(bits){
   nbits = bits;
 } else {
   nbits = 8*sizeof(unsigned int)*size;
 }
 npts = nbits+1;
 Ntest_create(&btest,npts,"rgb_binomial",gsl_rng_name(random));
 for(k=0;k<npts;k++){
   ptmp = binomial(nbits,k,0.5);
   btest.y[k] = samples*ptmp;
   btest.sigma[k] = sqrt(btest.y[k]*(1.0 - ptmp));
 }

 /*
  * Next we run the sampling loop to accumulate num_bit results
  * in btest.x[k].  For the moment we print out each sample as well.
  */
 for(i=0;i<samples;i++){
   if(verbose){
     printf("# bitstring: ");
   }
   if(reseed_flag) {
     seed = random_seed();
     gsl_rng_set(random,seed);
   }

   /*
    * Fill vector of "random" integers with selected generator.
    * NOTE WELL:  This can also be done by reading in a file!
    */
   for(j=0;j<size;j++) rand_int[j] = gsl_rng_get(random);
   /*
    * Count the 1 bits and the 0 bits
    * We do this by masking out each bit slot, anding the mask with
    * the rand_int and accumulating the ones.  I think.
    */
   num_bits = 0;
   num_ones = 0;
   num_zeros = 0;
   for(j=0;j<nbits;j++){
     if(thisbit = get_bit(j)) num_ones++;
     if(verbose){
       printf("%1d",thisbit);
     }
   }
   num_zeros = num_bits - num_ones;
   onefrac = 100.0*(double)num_ones/(double)num_bits;
   if(verbose){
     printf("\n");
   }
   /*
    * Increment the counter for the resulting number of ones.
    */
   btest.x[num_ones]++;
 }

 /*
  * RESULTS:  We output chisq, the number of points compared, and a p-value
  * that is the probability of getting this result if the random number
  * generator is good.  Hence values less than 0.01 suggest a "bad" rng.
  * Results MUCH less than 0.01 should Never Happen and we'll say so.
  * If I understand things correctly, p-value from chisq is
  *   p = incomplete_gamma(N/2,chisq/2) (gsl Q version, not P version).
  */
 Ntest_eval(&btest);
 if(!quiet){
   printf("#==================================================================\n");
   printf("#                 %s Test Description\n",btest.testname);
   printf("# Evaluates the number of 1's in %d strings of %d bits and\n",samples,nbits);
   printf("# compares the result histogram to the binomial distribution.\n");
   printf("# From chisq and the incomplete gamma function the probability\n");
   printf("# of this histogram if the rng tested is in fact \"random\" is\n");
   printf("# obtained.  If this p-value is low the hypothesis that it is\n");
   printf("# random safely can be rejected\n");
 }

 Ntest_conclusion(&btest);

 printf("# %10s   %12s    %12s  %12s  %10s\n","Test Name","Generator",
   "# samples","# of bits","p-value");
 printf(" %13s  %12s  %12d  %11d  %13.7f\n",btest.testname,btest.rngname,
   samples,nbits,btest.pvalue);

 Ntest_destroy(&btest);

}

